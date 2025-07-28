#include "RsuHandler.h"

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
        capacity = 200;
        threshold = 30;
        flushed = 20;

        maxHops = 120;       // Default maximum ttl
        lastUpdated = simTime();

        // Simulation variables
        unit = UnitType::RSU;
        caching = CachingPolicy::FIFO;
        centrality = CentralityType::DEGREE;
        initializeVariables();

        // Network information
        originIp = 0;
        rsuCount = 0;
        // Highest centrality RSU info
        //pushRsu = 0;
        highestCentrality = 0;

        thresholdControl();
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
            //debugPrint(routingTable);

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

        std::cout << "RSU " << myId << " completed centrality calculation at " << msgTime << endl;

        std::string centType = "";
        float val;

        if (centrality == CentralityType::DEGREE)
        {
            centType = "Degree";
            val = degree;
        }
        else if (centrality == CentralityType::CLOSENESS)
        {
            centType = "Closeness";
            val = closeness;
        }
        else if (centrality == CentralityType::BETWEENNESS)
        {
            centType = "Betweenness";
            val = betweenness;
        }
    

        if (simTime() > 225)
        {
            std::ofstream file;
            file.open("centrality_time.csv", std::ios_base::app);
            file << std::to_string(rsuCount) + "," + 
                        std::to_string(idtmp) + "," +
                        std::to_string(msgTime.dbl()) + ","  +
                        centType + "," + 
                        std::to_string(val) + "\n";
            file.close();
        }

        simtime_t time = simTime() + 1 + uniform(0.01, 0.5);
        std::cout << "RSU " << myId << " sending centrality info to origin at " << time << endl;
        scheduleAt(time, info);

        msgTime = 0; 

        degree = 0;
        closeness = 0;
        betweenness = 0;
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
        findHost()->getDisplayString().setTagArg("i", 1, "purple");

        if (wsm->getType() == MessageType::RSU_INIT_REQ && wsm->getSource() == 20)
            std::cout << "G O T   I T\n";

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
        //pushRsu = myId;

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
    {
        if (rsuRouting.size() == rsuCount && !temp)
        {
            std::cout << "Found RSU Routing: " << myId << endl;
            temp = true;
        }
    }
        
}


void RsuHandler::onOriginCentralityReq(Message *wsm)
{
    std::cout << "RSU " << myId << " received origin centrality request, at " << simTime() << endl;
    if (centrality != wsm->getCentrality())
        centrality = wsm->getCentrality();

    idtmp++;

    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);
    
    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup());
    
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

    simtime_t time = simTime() + 5 + uniform(0.01, 3);
    requestTime = time;
    std::cout << "RSU " << myId << " sending centrality request at " << time << endl;
    scheduleAt(time, request->dup());

    // Create message for results collection
    Message *collect = new Message();
    populateWSM(collect);
    // Define message identifiers
    collect->setDest(myId);
    collect->setType(request->getType());
    collect->setCentrality(centrality);
    collect->setState(CurrentState::COLLECTING);

    scheduleAt(simTime() + 45, collect);
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
    {
        std::cout << "RSU " << myId << " received pull reply, forwarding to " << wsm->getDest() << endl;
        handleRouteTraversal(wsm, true);
    }

    else
    {
        std::string content = extractContent(wsm);
        sendAcknowledgement(wsm);

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

                    simtime_t time;
                    if (segments > 1)
                    {
                        std::string segmentedContent;
                        float delay = 0.1;

                        for (int j = 1; j <= segments; j++)
                        {
                            // Break info in smaller parts
                            segmentedContent = content[j - 1];
                            // Edit reply content
                            reply->setSegmentNumber(j);
                            reply->setContent(segmentedContent.c_str());

                            time = simTime() + j * delay + uniform(0.01, 0.5);
                            std::cout << "RSU " << myId << " sending segmented content to " << i->source << " with Content ID " << wsm->getContentId();
                            std::cout << " and segment number " << j << " at " << time << endl;
                            scheduleAt(time, reply->dup());

                            // Segment pending acknowledgement
                            Message *repeat = reply->dup();
                            pendingAck.push_back(MessageData(repeat));            
                            repeat->setState(CurrentState::REPEATING);
                            scheduleAt(simTime() + 5 + j * delay, repeat);
                        }
                    }
                    // Else, just send content
                    else
                    {
                        time = simTime() + 0.1 + uniform(0.01, 0.5);
                        std::cout << "RSU " << myId << " sending content with Content Id " << wsm->getContentId() << " to " << i->source << " at " << time << endl;
                        reply->setContent(content.c_str());
                        scheduleAt(time, reply->dup());

                        // Segment pending acknowledgement
                        pendingAck.push_back(MessageData(reply));
                        reply->setState(CurrentState::REPEATING);
                        scheduleAt(simTime() + 5, reply);
                    }
                }
            }
        }
    }
}


void RsuHandler::onPushML(Message *wsm)
{
    // Extract info
    //pushRsu = wsm->getDest();

    long address = wsm->getDest();
    if (simTime() - lastUpdateRsu > 75)
        centralVec.clear();

    if (!(std::find(centralVec.begin(), centralVec.end(), address) != centralVec.end()))
    {
        centralVec.push_back(address);
        lastUpdateRsu = simTime();
    }

    // Forward to reach other RSUs
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup()); 
}

void RsuHandler::onPushCentrality(Message *wsm)
{
    // Extract info
    //pushRsu = wsm->getDest();
    long address = wsm->getDest();
    if (simTime() - lastUpdateRsu > 75)
        centralVec.clear();

    if (!(std::find(centralVec.begin(), centralVec.end(), address) != centralVec.end()))
    {
        centralVec.push_back(address);
        lastUpdateRsu = simTime();
    }

    highestCentrality = wsm->getMsgInfo();

    // Forward to reach other RSUs
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup());
}

// ------------- Regular Messages ------------ //

void RsuHandler::onBroadcast(Message *wsm)
{
    // Extract info
    std::string content = extractContent(wsm);

    if (!content.empty())
    {
        simtime_t time;

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
                time = simTime() + i * delay + uniform(0.01, 0.5);
                std::cout << "RSU " << myId << " pushing segmented content to " << originIp << " with Content ID " << wsm->getContentId();
                std::cout << " and segment number " << i << " at " << time << endl;

                // Break into smaller strings
                segmentedContent = content[i - 1];

                push->setSegmentNumber(i);
                push->setContent(segmentedContent.c_str());
                scheduleAt(time, push->dup());

                // Segment pending acknowledgement
                Message *repeat = push->dup();
                pendingAck.push_back(MessageData(repeat));            
                repeat->setState(CurrentState::REPEATING);
                scheduleAt(simTime() + 5 + i * delay, repeat);
            }
        }

        else
        {   
            time = simTime() + 1 + uniform(0.01, 0.5);
            std::cout << "RSU " << myId << " sending content with Content Id " << wsm->getContentId() << " to " << originIp << " at " << time << endl;

            push->setContent(content.c_str());
            scheduleAt(time, push->dup());

            // Segment pending acknowledgement
            pendingAck.push_back(MessageData(push));
            push->setState(CurrentState::REPEATING);
            scheduleAt(simTime() + 5, push);
        }
    }

    // Then forward to other nodes in the network
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 1 + uniform(0.01, 0.5), wsm->dup());
}

void RsuHandler::onRequest(Message *wsm)
{
    bool found = false;
    bool multimedia = wsm->getMultimedia();

    if (multimedia)
    {
        std::cout << "RSU " << myId << " searching for multimedia content with Content Id " << wsm->getContentId() << endl;
        found = contentSearch(wsm, multimediaData, false);
    }
    else
        found = contentSearch(wsm, roadData, false);

    if (!found)
    {
        // Create pull request to RSU with max centrality
        std::string contentId = wsm->getContentId();
        long source = myId;
        long dest;

        /*
        if (pushRsu == myId)
            dest = originIp;
        else    
            dest = pushRsu;
            */

        bool myself = std::find(centralVec.begin(), centralVec.end(), myId) != centralVec.end();
        if (centralVec.empty() || myself)
            dest = originIp;
        else
        {
            int maxHops = 9999;
            std::cout << "Size: " << centralVec.size() << endl;
            for (auto it = centralVec.begin(); it != centralVec.end(); it++)
            {
                std::cout << "Central RSU: " << *it << endl;
                auto it2 = rsuRouting.find(*it);
                pathDeque route = it2->second;

                if (route.size() < maxHops)
                {
                    maxHops = route.size();
                    dest = *it;
                }
            }
        }
        
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
        if (msgTime < simTime() - requestTime)
            msgTime = simTime() - requestTime;
        //std::cout << "node " << wsm->getSource() << endl;
    }

    else
    {
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup());
    }
}

void RsuHandler::onBetweennessReply(Message *wsm)
{
    if (wsm->getDest() == myId)
    {
        betweenness += wsm->getMsgInfo();
        if (msgTime < simTime() - requestTime)
            msgTime = simTime() - requestTime;
    }

    else
    {
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup());
    }
}

// ----------- Auxiliary Functions ------------ //

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

    simtime_t time = simTime() + 1 + uniform(0.01, 0.5);
    std::cout << "RSU " << myId << " sending pull request to " << dest << " at " << time << endl;

    scheduleAt(time, request);
}


void RsuHandler::initializeVariables()
{
    int id = getParentModule()->par("scenarioId");

    if (id % 2 == 0)
        origin = OriginPolicy::PULL;
    else
        origin = OriginPolicy::PUSH;
}
