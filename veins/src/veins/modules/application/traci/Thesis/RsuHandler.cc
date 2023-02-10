#include "veins/modules/application/traci/Thesis/RsuHandler.h"

using namespace veins;
using namespace omnetpp;

Define_Module(veins::RsuHandler);

// -------------- Initializers --------------- //

void RsuHandler::initialize(int stage)
{
    // Initializing variables
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0)
    {
        // Centrality metrics
        degree = 0;
        closeness = 0;
        betweenness = 0;
        // Caching variables
        capacity = 100;
        threshold = 30;
        flushed = 10;

        maxHops = 25;       // Default maximum ttl
        lastUpdated = simTime();

        // Simulation variables
        unit = UnitType::RSU;
        caching = CachingPolicy::FIFO;
        centrality = CentralityType::DEGREE;

        // Network information
        originIp = 0;
        rsuCount = 0;
        // Highest centrality RSU info
        pushRsu = 0;
        highestCentrality = 0;
    }
}

// -------------- Self Message --------------- //

void RsuHandler::collectingMsg(Message *wsm)
{
    UnitHandler::collectingMsg(wsm);
    
    long id = wsm->getDest();
    if (id == myId)
    {
        float result = 0;
        MessageType type = wsm->getType();
        if (type == MessageType::ROUTE_REQ)
        {
            for (auto i = routingTable.begin(); i != routingTable.end(); i++)
                result += i->second.size();

            if (result != 0)
                result = float(routingTable.size()) / result;
            std::cout << "Average shortest route length is " << result << endl;
            debugPrint(routingTable);

            closeness = result; 
        }

        else if (type == MessageType::CENTRALITY_REQ)
        {
            CentralityType centrality = wsm->getCentrality();
            if (centrality == CentralityType::DEGREE)
            {
                result = degree;
                std::cout << "Degree centrality is " << result << endl;
            }

            else if (centrality == CentralityType::BETWEENNESS)
            {
                result = betweenness;
                std::cout << "Node " << myId << " betweenness centrality is " << result << endl;
            }
        }
        // Clear routing table after calculation
        routingTable.clear();

        // Find optimal path to origin
        auto it = rsuRouting.find(originIp);
        pathDeque route = it->second;


        // Create info message to origin
        Message *info = new Message();
        populateWSM(info);
        // Define identifiers
        info->setSenderAddress(myId);
        info->setSenderPosition(curPosition);
        info->setRecipient(route.front());
        info->setSource(myId);
        info->setDest(originIp);
        // Define message properties
        info->setMaxHops(route.size());
        info->setType(MessageType::ORIGIN_CENTRALITY_REPLY);
        info->setOriginMessage(true);
        info->setUpdatePaths(false);
        info->setMsgInfo(result);
        // Edit route details
        route.pop_front();
        info->setRoute(route);

        scheduleAt(simTime() + 1 + uniform(0.01, 0.2), info);
    }

    delete(wsm);
}

// --------------- On Message ---------------- //

void RsuHandler::onWSA(DemoServiceAdvertisment *wsa)
{
    // If this RSU receives a WSA for Service 42, it will tune to the channel
    if (wsa->getPsid() == 42) 
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
}

void RsuHandler::onWSM(BaseFrame1609_4 *frame)
{
    Message *wsm = check_and_cast<Message*>(frame);
    bool accept = false;

    if (wsm->getOriginMessage())
        accept = receiveMessage(wsm, rsuRouting);
    else    
        accept = receiveMessage(wsm, routingTable);

    if (accept)
    {
        // Change RSU color to gold
        findHost()->getDisplayString().setTagArg("i", 1, "gold");
        UnitHandler::onWSM(frame);
    }
}

// ------------- Origin Messages ------------- //

void RsuHandler::onOriginInitReq(Message *wsm)
{
    handleRouteReq(wsm, MessageType::ORIGIN_INIT_REPLY);

    if (originIp == 0)
    {
        originIp = wsm->getSource();
        rsuCount = wsm->getMaxHops();
        pushRsu = myId;

        // Create your own RSU route discovery request
        createRouteReq(MessageType::RSU_INIT_REQ, rsuCount, true);
    }
}

void RsuHandler::onOriginInitReply(Message *wsm)
{
    // Forward to next node in line
    handleRouteTraversal(wsm);
}

void RsuHandler::onRsuInitReq(Message *wsm)
{
    // Handle RSU init request
    handleRouteReq(wsm, MessageType::RSU_INIT_REPLY);
}

void RsuHandler::onRsuInitReply(Message *wsm)
{
    // If not for you, forward
    if (wsm->getDest() != myId)
        handleRouteTraversal(wsm);
    // TODO: Remove when finished
    else
        debugPrint(rsuRouting);
}


void RsuHandler::onOriginCentralityReq(Message *wsm)
{
    std::cout << "Received origin request\n";
    if (centrality != wsm->getCentrality())
        centrality = wsm->getCentrality();
    
    // Create centrality request
    Message *request = new Message();
    populateWSM(request);
    // Define message identifiers
    request->setSource(myId);
    request->setSenderAddress(myId);
    request->setSenderPosition(curPosition);
    // Define message properties
    request->setCentrality(centrality);
    request->setType(MessageType::CENTRALITY_REQ);
    request->setMaxHops(maxHops);

    if (centrality == CentralityType::DEGREE)
        request->setMaxHops(1);

    else if (centrality == CentralityType::CLOSENESS)
    {
        request->setType(MessageType::ROUTE_REQ);
        
        pathDeque route;
        route.push_back(myId);
        request->setRoute(route);
    }

    scheduleAt(simTime() + 5 + uniform(0.01, 0.2), request->dup());

    // Create message for results collection
    Message *collect = new Message();
    populateWSM(collect);
    // Define message identifiers
    collect->setDest(myId);
    collect->setType(request->getType());
    collect->setCentrality(centrality);
    collect->setState(CurrentState::COLLECTING);

    scheduleAt(simTime() + 30, collect);
    delete(request);
}

void RsuHandler::onOriginCentralityReply(Message *wsm)
{
    // Forward to reach origin
    handleRouteTraversal(wsm, true);
}


void RsuHandler::onPullReq(Message *wsm)
{
    // If not meant for you, forward
    if (wsm->getDest() != myId)
        handleRouteTraversal(wsm, true);
    
    else
    {
        bool found = false;
        bool multimedia = wsm->getMultimedia();

        if (multimedia)
            found = contentSearch(wsm, multimediaData, true);
        else
            found = contentSearch(wsm, roadData, true);

        // If you don't have requested data, ask Origin
        if (!found)
        {
            std::string contentId = wsm->getContentId();
            long source = wsm->getSource();
            long dest = originIp;

            createPullReq(contentId, source, dest, multimedia);
        }
    }
}

void RsuHandler::onPullReply(Message *wsm)
{
    // If not meant for you, forward
    if (wsm->getDest() != myId)
        handleRouteTraversal(wsm, true);

    else
    {
        std::string content = extractContent(wsm);
        if (!content.empty())
        {
            for (auto i = pendingReply.begin(); i != pendingReply.end(); i++)
            {
                if (wsm->getMultimedia() == i->multimedia && 
                        wsm->getContentId() == i->contentId)
                {
                    int segments = wsm->getSegments();

                    // TODO: Change to contentSearch
                    // Create reply message
                    Message *reply = new Message();
                    populateWSM(reply);
                    // Define identifiers
                    reply->setSource(myId);
                    reply->setDest(i->source);
                    reply->setSenderAddress(myId);
                    reply->setSenderPosition(curPosition);
                    // Define properties
                    reply->setType(MessageType::REPLY);
                    reply->setContentId(i->contentId.c_str());
                    reply->setSegments(segments);
                    reply->setMultimedia(i->multimedia);

                    if (segments > 1)
                    {
                        std::string segmentedContent;
                        float delay = 1;

                        for (int j = 1; j <= segments; j++)
                        {
                            // Break info in smaller parts
                            segmentedContent = content[j - 1];
                            // Edit reply content
                            reply->setSegmentNumber(j);
                            reply->setContent(segmentedContent.c_str());
                            scheduleAt(simTime() + j * delay + uniform(0.01, 0.2), reply->dup());
                        }
                    }
                    // Else, just send content
                    else
                    {
                        reply->setContent(content.c_str());
                        scheduleAt(simTime() + 1 + uniform(0.01, 0.2), reply);
                    }
                }
            }
        }
    }
}


void RsuHandler::onPushML(Message *wsm)
{
    // Extract info
    pushRsu = wsm->getDest();

    // Forward to reach other RSUs
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup()); 
}

void RsuHandler::onPushCentrality(Message *wsm)
{
    // Extract info
    pushRsu = wsm->getDest();
    highestCentrality = wsm->getMsgInfo();

    // Forward to reach other RSUs
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
}

// ------------- Regular Messages ------------ //

void RsuHandler::onBroadcast(Message *wsm)
{
    // Extract info
    std::string content = extractContent(wsm);
    if (!content.empty())
    {
        auto it = rsuRouting.find(originIp);
        pathDeque route = it->second;

        // Create push message
        Message *push = new Message();
        populateWSM(push);
        // Define message identifiers
        push->setSource(myId);
        push->setDest(originIp);
        push->setSenderAddress(myId);
        push->setSenderPosition(curPosition);
        push->setRecipient(route.front());
        // Define message properties
        push->setMaxHops(route.size());
        push->setOriginMessage(true);
        push->setUpdatePaths(false);
        push->setType(MessageType::PUSH_CONTENT);
        push->setMultimedia(wsm->getMultimedia());
        // Edit route details
        route.pop_front();
        push->setRoute(route);

        // Define content details
        int segments = wsm->getSegments();

        push->setContentId(wsm->getContentId());
        push->setSegments(segments);
        push->setMultimedia(wsm->getMultimedia());

        // If segmented, send in parts
        if (segments > 1)
        {
            std::string segmentedContent;
            float delay = 0.1;

            for (int i = 1; i <= segments; i++)
            {
                // Break into smaller strings
                segmentedContent = content[i - 1];

                push->setSegmentNumber(i);
                push->setContent(segmentedContent.c_str());
                scheduleAt(simTime() + i * delay + uniform(0.01, 0.2), push->dup());

                pendingAck.push_back(MessageData(push));
            }
        }

        else
        {
            push->setContent(content.c_str());

            pendingAck.push_back(MessageData(push));
            scheduleAt(simTime() + 1 + uniform(0.01, 0.2), push);
        }
    }

    // Then forward to other nodes in the network
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 1 + uniform(0.01, 0.2), wsm->dup());
}

void RsuHandler::onRequest(Message *wsm)
{
    bool found = false;
    bool multimedia = wsm->getMultimedia();

    if (multimedia)
        found = contentSearch(wsm, multimediaData, true);
    else
        found = contentSearch(wsm, roadData, true);

    if (!found)
    {
        // Create pull request to RSU with max centrality
        std::string contentId = wsm->getContentId();
        long source = myId;
        long dest;

        if (pushRsu == myId)
            dest = originIp;
        else    
            dest = pushRsu;
        
        createPullReq(contentId, source, dest, multimedia);
        pendingReply.push_back(MessageData(wsm));
    }
}

// ----------- On Centrality Reply ----------- //

void RsuHandler::onDegreeReply(Message *wsm)
{
    if (wsm->getDest() == myId)
    {
        degree++;
        //std::cout << "node " << wsm->getSource() << endl;

        // Create acknowledgement message
        Message *ack = new Message();
        populateWSM(ack);
        // Define message ids and properties
        ack->setSenderAddress(myId);
        ack->setSenderPosition(curPosition);
        ack->setSource(myId);
        ack->setDest(wsm->getSource());
        ack->setAckInfo(wsm->getCreationTime());
        ack->setMaxHops(wsm->getHops());
        ack->setType(MessageType::ACKNOWLEDGEMENT);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), ack);
    }

    else
    {
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
}

void RsuHandler::onBetweennessReply(Message *wsm)
{
    if (wsm->getDest() == myId)
    {
        betweenness += wsm->getMsgInfo();

        // TODO: Send acknowledgement
    }

    else
    {
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
}

// ----------- Auxilary Functions ------------ //

void RsuHandler::createPullReq(std::string contentId, long source, 
    long dest, bool multimedia)
{
    // Look up path in RSU routing table
    auto it = rsuRouting.find(dest);
    pathDeque route = it->second;

    // Create pull request
    Message *request = new Message();
    populateWSM(request);
    // Define message identifiers
    request->setSenderAddress(myId);
    request->setSenderPosition(curPosition);
    request->setRecipient(route.front());
    request->setSource(source);
    request->setDest(dest);
    // Edit properties
    request->setMaxHops(route.size());
    request->setType(MessageType::PULL_REQ);
    request->setOriginMessage(true);
    request->setUpdatePaths(false);
    // Edit route
    route.pop_front();
    request->setRoute(route);
    // Edit request data
    request->setContentId(contentId.c_str());
    request->setMultimedia(multimedia);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), request);
}
