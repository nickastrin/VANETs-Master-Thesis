#include "UnitHandler.h"

using namespace veins;
using namespace omnetpp;

Define_Module(veins::UnitHandler);

// -------------- Self Message --------------- //

void UnitHandler::handleSelfMsg(cMessage *msg)
{
    if (Message *wsm = dynamic_cast<Message*>(msg))
    {
        CurrentState state = wsm->getState();
        switch (state)
        {
            case CurrentState::INITIALIZING:
                initializingMsg();
                delete(wsm);
                break;
            case CurrentState::SENDING:
                sendingMsg(wsm);
                break;
            case CurrentState::REQUESTING:
                requestingMsg();
                delete(wsm);
                break;            
            case CurrentState::REPEATING:
                repeatingMsg(wsm);
                break;     
            case CurrentState::COLLECTING:
                collectingMsg(wsm);
                break;      
            case CurrentState::CACHING:
                cachingMsg(wsm, threshold);
                break;
            case CurrentState::WRITING:
                writingMsg();
                delete(wsm);
                break;
            case CurrentState::EXTRACTING:
                extractingMsg();
                delete(wsm);
                break;
            default:
                break;
        }
    }
}

void UnitHandler::sendingMsg(Message *wsm)
{
    // If the message doesn't exceed max hop count
    if (wsm->getHops() < wsm->getMaxHops())
    {
        wsm->setHops(wsm->getHops() + 1);
        sendDown(wsm->dup());
        delete(wsm);
    }
    // Delete the message
    else
    {
        stopService();
        delete(wsm);
    }
}
    
void UnitHandler::collectingMsg(Message *wsm)
{
    float result = 0;
    long id = wsm->getDest();

    if (id != myId)
    {
        int ttl = maxHops;

        // Find approximate path length to destination
        if (routingTable.find(wsm->getDest()) != routingTable.end())
            ttl = routingTable[wsm->getDest()].size() + 2;

        // Calculate result
        for (auto i = routingTable.begin(); i != routingTable.end(); i++)
        {
            auto it = std::find(i->second.begin(), i->second.end(), wsm->getDest());
            if (it != i->second.end())
                result++;
        }

        //std::cout << "Node " << myId << " calculated betweenness for RSU " << id << " at " << msgTime << endl;

        // Create results message
        Message *results = new Message();
        populateWSM(results);
        // Define message identifiers
        results->setSenderAddress(myId);
        results->setSenderPosition(curPosition);
        results->setSource(myId);
        results->setDest(id);
        // Define message properties
        results->setMaxHops(ttl);
        results->setType(MessageType::CENTRALITY_REPLY);
        results->setCentrality(CentralityType::BETWEENNESS); 
        results->setMsgInfo(result);

        if (results->getDest() == 34)
        {
            std::cout << "Node " << myId << " result " << result << endl;

            if (myId == 154)
                debugPrint(routingTable);
        }

        simtime_t time = simTime() + 0.1 + uniform(0.01, 2);
        //std::cout << "Node " << myId << " sent result at " << time << endl;
        scheduleAt(time, results);
    }
}

void UnitHandler::repeatingMsg(Message *wsm)
{
    // Search if message in the acknowledgements deque
    auto it = std::find_if(pendingAck.begin(), pendingAck.end(),
        [&wsm, this](MessageData data) 
            { return (wsm->getDest() == data.dest && contentMatch(wsm, data) && wsm->getSegmentNumber() == data.segmentNumber); });

    if (it != pendingAck.end())
    {   
        // Resend the message a limited amount of times before deleting
        if (it->attempts >= 1)
        {
            // To optimize deletion, swap element with last
            // And delete it, taking advantage of deque pop_back
            MessageData lastElem = pendingAck.back();
            std::swap(*it, lastElem);
            pendingAck.pop_back();

            delete(wsm);
        }

        else
        {
            it->attempts++;
            //std::cout << "Attempts " << it->attempts << " Target: " << it->dest <<  endl;

            // If message hasn't been acknowledged, reschedule repeat check
            wsm->setMaxHops(wsm->getMaxHops() + 2);
            scheduleAt(simTime() + uniform(0.01, 0.5), wsm->dup());

            // And resend the message with higher hop count
            wsm->setState(CurrentState::SENDING);
            scheduleAt(simTime() + uniform(0.01, 0.5), wsm);
        }
    } 

    else 
        delete(wsm);
}

void UnitHandler::cachingMsg(Message *wsm, simtime_t limit)
{
    simtime_t now = simTime();

    // Perform threshold control
    if (!storedMessages.empty())
    {
        // Transfer stale messages to candidate deque
        auto it = storedMessages.rbegin();
        while (now - it->timestamp > limit)
        {
            candidateMessages.push_front(*it);
            storedMessages.pop_back();

            if (storedMessages.empty())
                break;

            it = storedMessages.rbegin();
        }
    }

    delete(wsm);
    thresholdControl();
}

// --------------- On Message ---------------- //

void UnitHandler::onWSM(BaseFrame1609_4 *frame)
{
    Message *wsm = check_and_cast<Message*>(frame);
    
    MessageType type = wsm->getType();
    bool origin = wsm->getOriginMessage();

    if (origin)
    {
        switch (type)
        {
            case MessageType::ORIGIN_INIT_REQ:
                onOriginInitReq(wsm);
                break;
            case MessageType::ORIGIN_INIT_REPLY:
                onOriginInitReply(wsm);
                break;
            case MessageType::RSU_INIT_REQ:
                onRsuInitReq(wsm);
                break;
            case MessageType::RSU_INIT_REPLY:
                onRsuInitReply(wsm);
                break;
            case MessageType::ORIGIN_CENTRALITY_REQ:
                onOriginCentralityReq(wsm);
                break;
            case MessageType::ORIGIN_CENTRALITY_REPLY:
                onOriginCentralityReply(wsm);
                break;
            case MessageType::PULL_REQ:
                onPullReq(wsm);
                break;
            case MessageType::PULL_REPLY:
                onPullReply(wsm);
                break;
            case MessageType::PUSH_ML:
                onPushML(wsm);
                break;
            case MessageType::PUSH_CENTRALITY:
                onPushCentrality(wsm);
                break;
            case MessageType::PUSH_CONTENT:
                onPushContent(wsm);
                break;
            case MessageType::ACKNOWLEDGEMENT:
                onAcknowledgement(wsm);
                break;
            default:
                break;
        }
    }

    else
    {
        switch (type)
        {
            case MessageType::BROADCAST:
                onBroadcast(wsm);
                break;
            case MessageType::REQUEST:
                onRequest(wsm);
                break;
            case MessageType::REPLY:
                onReply(wsm);
                break;
            case MessageType::ROUTE_REQ:
                onRouteReq(wsm);
                break;
            case MessageType::ROUTE_REPLY:
                onRouteReply(wsm);
                break;
            case MessageType::CENTRALITY_REQ:
                onCentralityReq(wsm);
                break;
            case MessageType::CENTRALITY_REPLY:
                onCentralityReply(wsm);
                break;
            case MessageType::ACKNOWLEDGEMENT:
                onAcknowledgement(wsm);
                break;
            default:
                break;
        }
    }
}

void UnitHandler::onReply(Message *wsm)
{
    // If not meant for you, forward
    if (wsm->getDest() != myId)
    {
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);
        scheduleAt(simTime() + uniform(0.01, 0.5), wsm->dup());
    }

    else 
    {
        extractContent(wsm);
        sendAcknowledgement(wsm);
    }
}

// ------------ On Route Handlers ------------ //

void UnitHandler::onRouteReq(Message *wsm)
{
    // Call route request handler
    handleRouteReq(wsm, MessageType::ROUTE_REPLY);
}

void UnitHandler::onRouteReply(Message *wsm)
{
    // If it's meant for you, accept
    if (wsm->getDest() == myId)
    {
        EV << "Node " << myId << " accepted route reply.\n";
        if (msgTime < simTime() - requestTime)
            msgTime = simTime() - requestTime;
    }

    // Else, forward
    else 
        handleRouteTraversal(wsm);
}

// --------- Centrality Req Handlers --------- //

void UnitHandler::onCentralityReq(Message *wsm)
{
    CentralityType centrality = wsm->getCentrality();
    switch (centrality)
    {
        case CentralityType::DEGREE:
            onDegreeReq(wsm);
            break;
        case CentralityType::BETWEENNESS:
            onBetweennessReq(wsm);
            break;
        default:
            break;
    }
}

void UnitHandler::onDegreeReq(Message *wsm)
{
    // Create reply message
    Message *reply = new Message();
    populateWSM(reply);
    // Define reply identifiers
    reply->setSenderAddress(myId);
    reply->setSenderPosition(curPosition);
    reply->setSource(myId);
    reply->setDest(wsm->getSource());
    // Define reply properties
    reply->setType(MessageType::CENTRALITY_REPLY);
    reply->setCentrality(CentralityType::DEGREE);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), reply);
}

void UnitHandler::onBetweennessReq(Message *wsm)
{    
    // Broadcast the betweenness request to other cars
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup());

    // Create route request
    createRouteReq(MessageType::ROUTE_REQ, maxHops);

    // Create collection message
    Message *collect = new Message();
    populateWSM(collect);
    // Define collection details
    collect->setDest(wsm->getSource());
    collect->setState(CurrentState::COLLECTING);
    collect->setType(MessageType::CENTRALITY_REQ);
    collect->setCentrality(CentralityType::BETWEENNESS);

    scheduleAt(simTime() + 25 + uniform(0.01, 0.5), collect);
}


void UnitHandler::onCentralityReply(Message *wsm)
{
    CentralityType centrality = wsm->getCentrality();
    switch (centrality)
    {
        case CentralityType::DEGREE:
            onDegreeReply(wsm);
            break;
        case CentralityType::BETWEENNESS:
            onBetweennessReply(wsm);
            break;
        default:
            break;
    }
}

// -------------- Misc Handlers -------------- //

void UnitHandler::onAcknowledgement(Message *wsm)
{
    if (wsm->getDest() != myId)
    {
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);
        scheduleAt(simTime() + uniform(0.01, 0.5), wsm->dup());
    }

    else
    {
        //std::cout << "Node " << myId << " received acknowledgement from Node " << wsm->getSource() << endl;
        // Find the message you received acknowledgement for
        auto it = std::find_if(pendingAck.begin(), pendingAck.end(),
            [&wsm, this](MessageData data) 
            { return (wsm->getSource() == data.dest && contentMatch(wsm, data) && wsm->getSegmentNumber() == data.segmentNumber); });


        if (it != pendingAck.end())
        {
            // To optimize deletion, swap element with last
            // And delete it, taking advantage of deque pop_back
            MessageData lastElem = pendingAck.back();
            std::swap(*it, lastElem);
            pendingAck.pop_back();
        }
    }
}

void UnitHandler::onPushContent(Message *wsm)
{
    if (wsm->getDest() != myId)
        handleRouteTraversal(wsm, true);
    
    else
    {
        extractContent(wsm);
        sendAcknowledgement(wsm);
    }
}

// ------------ Message Insertion ------------ //

bool UnitHandler::receiveMessage(Message *wsm, routingDict &routing)
{
    if (wsm->getSource() == myId)
        return false;

    bool reject = (wsm->getRecipient() != -1) && (wsm->getRecipient() != myId);
    if (reject) 
        return false;

    bool update = false;
    bool insertion = false;

    // Extract origin info
    bool origin = wsm->getOriginMessage();
    if (simTime() - lastUpdated > 75)
    {
        //std::cout << "Cleared routing table at " << simTime() << " for node " << myId << "\n";
        routingTable.clear();
        lastUpdated = simTime();
    }

    // Check if routing table needs an update
    pathDeque route = wsm->getRoute();
    pathDeque previous = wsm->getPreviousNodes();

    bool discovery = wsm->getUpdatePaths();
    if (discovery)
    {
        // If you're already in route, return false
        if (inRoute(route))
            return false;

        // Check if you can extract route info
        if (!route.empty())
        {
            update = routingTableUpdate(route, routing, origin);
            if (!update) 
                return false;
        }
        if (!previous.empty())
        {        
            update = routingTableUpdate(previous, routing, origin);
            if (!update) 
                return false;
        }
    }

    // Flush the message list if needed
    if (storedMessages.size() + candidateMessages.size() >= capacity)
        flushList();

    // Handle insertion
    std::string content = wsm->getContent();
    if (!content.empty())
    {
        if (wsm->getSource() == 20 && wsm->getDest() == 35)
            std::cout << "Node " << myId << " received message from Node " << wsm->getSource() << " with content: " << content << endl;
        if (wsm->getDest() == myId || wsm->getType() == MessageType::BROADCAST)
            insertion = insertSegmented(wsm);
        else 
            insertion = true;
    }

    else 
        insertion = insertMessage(wsm);

    // If an insertion or an update has been made, return true
    return (update || insertion);
}

bool UnitHandler::routingTableUpdate(pathDeque path, routingDict &routing, bool origin)
{   
    // Variables for map check
    pathDeque route;
    long id;

    // While the path still has nodes in it
    while (!path.empty())
    {
        // Extract path info
        id = path.back();
        path.pop_back();

        // Ignore your id
        if (id == myId)
            continue;

        route.push_back(id);  

        // Search routing table for unit
        auto it = routing.find(id);
        if (it != routing.end())
        {
            // If message path is worse than saved path, abort message
            if (route.size() > it->second.size())
                return false;
        }

        // Update routing table
        routing[id] = route;
        if (!origin)
            lastUpdated = simTime();
    }

    return true;
}

bool UnitHandler::insertMessage(Message *wsm)
{
    // If there are messages, insert accordingly sorted by generated time
    if (!storedMessages.empty())
    {
        simtime_t timestamp = wsm->getCreationTime();
        for (auto i = storedMessages.begin(); i != storedMessages.end(); i++)
        {
            if (i->timestamp > timestamp)
                continue;

            else if (i->timestamp == timestamp)
            {
                if (messageMatch(wsm, *i))
                    return false;
                else 
                    continue;
            }

            else if (i->timestamp < timestamp)
            {
                storedMessages.insert(i, MessageData(wsm));
                return true;
            }
        }
    }

    // If you didn't find message in stored messages, check candidate vector
    if (!candidateMessages.empty())
    {
        auto it = std::find_if(candidateMessages.begin(), candidateMessages.end(),
            [&wsm, this](MessageData data) 
            { return (this->messageMatch(wsm, data)); });
    
        if (it != candidateMessages.end())
            return false;
    }

    storedMessages.push_back(MessageData(wsm));
    return true;
}

bool UnitHandler::messageMatch(Message *wsm, MessageData data)
{
    bool timeMatch = wsm->getCreationTime() == data.timestamp;
    bool sourceMatch = wsm->getSource() == data.source;
    bool typeMatch = wsm->getType() == data.type;

    return (timeMatch && sourceMatch && typeMatch);
}

bool UnitHandler::insertSegmented(Message *wsm)
{
    
    bool multi = wsm->getMultimedia();

    if (multi)
    {
        auto it = multimediaData.find(wsm->getContentId());
        if (it != multimediaData.end())
        {
            if (it->second.timestamp >= wsm->getCreationTime())
                return false;
        }
    }
    else 
    {
        auto it = roadData.find(wsm->getContentId());
        if (it != roadData.end())
        {
            if (it->second.timestamp >= wsm->getCreationTime())
                return false;
        }
    }

    if (!segmentedMessages.empty())
    {
        int segmentNumber = wsm->getSegmentNumber();
        for (auto i = segmentedMessages.begin(); i != segmentedMessages.end(); i ++)
        {
            if (contentMatch(wsm, *i))
            {
                if (segmentNumber > i->segmentNumber)
                    continue;
                else if (segmentNumber == i->segmentNumber)
                    return false;
                else
                {
                    segmentedMessages.insert(i, MessageData(wsm));
                    return true;
                }
            }
        }

        segmentedMessages.push_back(MessageData(wsm));
        return true;
    }

    segmentedMessages.push_back(MessageData(wsm));
    return true;
}

bool UnitHandler::contentMatch(Message *wsm, MessageData data)
{
    bool idMatch = wsm->getContentId() == data.contentId;
    bool segmentsMatch = wsm->getSegments() == data.segments;
    bool typeMatch = wsm->getMultimedia() == data.multimedia;

    return (idMatch && segmentsMatch && typeMatch);
}

void UnitHandler::flushList()
{
    // If flushing needs to be done
    if (storedMessages.size() + candidateMessages.size() >= capacity)
    {
        // If you have no candidate messages for deletion
        if (candidateMessages.empty())
        {
            if (storedMessages.size() < flushed)
                storedMessages.clear();
            
            else 
            {
                if (caching == CachingPolicy::FIFO)
                    sort(storedMessages.begin(), storedMessages.end(), MessageData::sortFIFO);
                else if (caching == CachingPolicy::LRU)
                    sort(storedMessages.begin(), storedMessages.end(), MessageData::sortLRU);
                else if (caching == CachingPolicy::LFU)
                    sort(storedMessages.begin(), storedMessages.end(), MessageData::sortLFU);

                for (int i = 0; i < flushed; i++)
                    storedMessages.pop_front();
                
                sort(storedMessages.begin(), storedMessages.end(), MessageData::sortRestore);
            }
        }

        // If you do, delete from the candidate messages
        else 
        {
            if (candidateMessages.size() < flushed)
                candidateMessages.clear();
            
            else 
            {                
                if (caching == CachingPolicy::FIFO)
                    sort(candidateMessages.begin(), candidateMessages.end(), MessageData::sortFIFO);
                else if (caching == CachingPolicy::LRU)
                    sort(candidateMessages.begin(), candidateMessages.end(), MessageData::sortLRU);
                else if (caching == CachingPolicy::LFU)
                    sort(candidateMessages.begin(), candidateMessages.end(), MessageData::sortLFU);

                for (int i = 0; i < flushed; i++)
                    candidateMessages.pop_front();
                
                sort(candidateMessages.begin(), candidateMessages.end(), MessageData::sortRestore);

            }
        }
    }
}

// ------------ Routing Functions ----------- //

bool UnitHandler::inRoute(pathDeque route)
{
    // Search the current route
    auto it = std::find_if(route.begin(), route.end(),
        [this](long id) { return (id == this->myId); });

    // Return if you're already in the route
    return (it != route.end());
}

void UnitHandler::createRouteReq(MessageType type, int ttl, bool origin, bool update)
{
    // Create request
    Message *request = new Message();
    populateWSM(request);
    // Define identifiers
    request->setSenderAddress(myId);
    request->setSenderPosition(curPosition);
    request->setSource(myId);
    request->setInitPosition(curPosition);
    // Define properties
    request->setType(type);
    request->setMaxHops(ttl);
    request->setUpdatePaths(update);
    request->setOriginMessage(origin);
    // Define routing details
    pathDeque route;
    route.push_back(myId);
    request->setRoute(route);
    
    // TODO: MAYBE REVERT TO 0.1
    scheduleAt(simTime() + 1 + uniform(0.01, 0.5), request);
}

void UnitHandler::handleRouteReq(Message *wsm, MessageType type)
{
    pathDeque route = wsm->getRoute();
    pathDeque previous = wsm->getPreviousNodes();
    
    // Create route reply
    Message *reply = new Message();
    populateWSM(reply);
    // Define identifiers
    reply->setSenderAddress(myId);
    reply->setSenderPosition(curPosition);
    reply->setRecipient(route.back());
    reply->setSource(myId);
    reply->setDest(wsm->getSource());
    // Define properties
    reply->setMaxHops(route.size());
    reply->setType(type);
    reply->setOriginMessage(wsm->getOriginMessage());
    reply->setUpdatePaths(wsm->getUpdatePaths());

    // Change routing details
    route.pop_back();
    previous.push_back(myId);
    reply->setRoute(route);
    reply->setPreviousNodes(previous);

    // TODO: MAYBE INVERT THE VALUES
    scheduleAt(simTime() + 1 + uniform(0.01, 0.5), reply);

    // Forward original message to other nodes
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);
    // Edit routing details
    route = wsm->getRoute();
    route.push_back(myId);
    wsm->setRoute(route);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup());
}

void UnitHandler::handleRouteTraversal(Message *wsm, bool forward)
{
    // Extract routing info
    pathDeque route = wsm->getRoute();
    pathDeque previous = wsm->getPreviousNodes();

    // Edit message identifiers
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);
    // Traverse route according to argument
    if (forward)
    {
        wsm->setRecipient(route.front());
        route.pop_front();
    }

    else
    {
        wsm->setRecipient(route.back());
        route.pop_back();
    }
    // Edit route details
    previous.push_back(myId);
    wsm->setRoute(route);
    wsm->setPreviousNodes(previous);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup());
}

// ------------ Content Functions ------------ //   

bool UnitHandler::contentSearch(Message *wsm, storageDict storage, bool origin)
{
    std::string id = wsm->getContentId();
    auto it = storage.find(id);

    // If content is found, send reply
    if (it != storage.end())
    {
        // Create content reply
        Message *reply = new Message();
        populateWSM(reply);

        if (origin)
        {
            // Get optimal path fron RSU routing table
            auto rsu = rsuRouting.find(wsm->getSource());
            pathDeque route = rsu->second; 

            // Define identifier
            reply->setRecipient(route.front());
            // Define properties
            reply->setMaxHops(route.size());
            reply->setType(MessageType::PULL_REPLY);
            reply->setOriginMessage(true);
            reply->setUpdatePaths(false);
            // Edit route details
            route.pop_front();
            reply->setRoute(route);
        }

        else 
            reply->setType(MessageType::REPLY);

        // Extract content
        std::string content = it->second.content;
        int segments = it->second.segments;
        //std::cout << segments << endl;
        // Define message identifiers
        reply->setSenderAddress(myId);
        reply->setSenderPosition(curPosition);
        reply->setSource(myId);
        reply->setDest(wsm->getSource());
        // Define properties
        reply->setContentId(id.c_str());
        reply->setSegments(segments);
        reply->setMultimedia(wsm->getMultimedia());

        simtime_t time;
        // If segmented, send in parts
        if (segments > 1)
        {
            std::string segmentedContent;

            float delay = 0.1;

            for (int i = 1; i <= segments; i++)
            {
                time = simTime() + i * delay + uniform(0.01, 0.5);

                std::cout << "Node " << myId << " sending segmented content to " << wsm->getSource() << " with Content ID " << wsm->getContentId();
                std::cout << " and segment number " << i << " at " << time << endl;

                // Break into smaller strings
                segmentedContent = content[i - 1];

                reply->setSegmentNumber(i);
                reply->setContent(segmentedContent.c_str());
                scheduleAt(time, reply->dup());

                // Segment pending acknowledgement
                Message *repeat = reply->dup();
                pendingAck.push_back(MessageData(repeat));            
                repeat->setState(CurrentState::REPEATING);
                scheduleAt(simTime() + 5 + i * delay, repeat);
            }
        }

        else
        {
            time = simTime() + 0.1 + uniform(0.01, 0.5);
            std::cout << "Node " << myId << " sending content with Content Id " << wsm->getContentId() << " to " << wsm->getSource() << " at " << time << endl;

            reply->setContent(content.c_str());
            scheduleAt(time, reply->dup());

            // Segment pending acknowledgement
            pendingAck.push_back(MessageData(reply));
            reply->setState(CurrentState::REPEATING);
            scheduleAt(simTime() + 5, reply);
        }

        return true;
    }

    return false;
}

std::string UnitHandler::extractContent(Message *wsm)
{
    int segments = wsm->getSegments();
    std::string reconstructed = "";
    int segmentCount = 0;

    // Print that you got the segment depending on message type
    bool multi = wsm->getMultimedia();
    std::string printType = multi ? "multimedia" : "road";

    std::cout << "Node " << myId << " received " << printType << " content segment. Content Id: " << wsm->getContentId();
    std::cout << ". Segment number: " << wsm->getSegmentNumber() << " from " << wsm->getSource() << " at " << simTime() << endl;

    // Find first occurrence of content requested
    auto start = std::find_if(segmentedMessages.begin(), segmentedMessages.end(),
        [&wsm, this](MessageData data) { return this->contentMatch(wsm, data); });

    auto end = start;
    while (end != segmentedMessages.end())
    {
        if (contentMatch(wsm, *end))
        {
            // Count segments and start reconstructing
            reconstructed += end->content;

            segmentCount++;
            end++;
        }
        else
            break;
    }

    if (!recFirst && segments > 1 && segmentCount == 1 && wsm->getMultimedia())
    {
        recFirst = true;
        recFirstTime = simTime();
    }

    if (segmentCount == segments)
    {
        std::cout << "Reconstructed result: \n";
        std::cout << reconstructed << endl;

        Message *data = wsm->dup();
        // Define message properties
        data->setContent(reconstructed.c_str());

        if (unit == UnitType::VEHICLE && wsm->getMultimedia())
        {
            if (!sec && segments == 1)
            {
                cModule *origin = getParentModule()->getModuleByPath("origin[0]");
                if (origin == nullptr)
                    throw cRuntimeError("Could not find module with path: origin[0]");

                int id = origin->par("scenarioId");
                int rsuCount = origin->par("rsuCount");

                std::string simType;
                std::string centType;
                std::string origType;

                if (id % 8 == 1)
                {
                    simType = "ML";
                    centType = "Degree";
                    origType = "Push";
                }
                else if (id % 8 == 2)
                {
                    simType = "ML";
                    centType = "Degree";
                    origType = "Pull";
                }
                else if (id % 8 == 3)
                {
                    simType = "Manual";
                    centType = "Degree";
                    origType = "Push";
                }
                else if (id % 8 == 4)
                {
                    simType = "Manual";
                    centType = "Degree";
                    origType = "Pull";
                }
                else if (id % 8 == 5)
                {
                    simType = "Manual";
                    centType = "Closeness";
                    origType = "Push";
                }
                else if (id % 8 == 6)
                {
                    simType = "Manual";
                    centType = "Closeness";
                    origType = "Pull";
                }
                else if (id % 8 == 7)
                {
                    simType = "Manual";
                    centType = "Betweenness";
                    origType = "Push";
                }
                else if (id % 8 == 0)
                {
                    simType = "Manual";
                    centType = "Betweenness";
                    origType = "Pull";
                }
                    std::ofstream file;
                    file.open("response_time.csv", std::ios_base::app);
                    file << std::to_string(rsuCount) + "," + 
                        std::to_string(id) + "," +
                        wsm->getContentId() + ","  +
                        std::to_string(simTime().dbl()) + ","  +
                        std::to_string(simTime().dbl()) + ","  +
                        "REP" + "," + 
                        simType + "," +
                        centType + "," + 
                        origType + "\n";
                    file.close();
                    
                    sec = true;
            }

            if (!rec && segments > 1)
            {
                cModule *origin = getParentModule()->getModuleByPath("origin[0]");
                if (origin == nullptr)
                    throw cRuntimeError("Could not find module with path: origin[0]");

                int id = origin->par("scenarioId");
                int rsuCount = origin->par("rsuCount");

                std::string simType;
                std::string centType;
                std::string origType;

                if (id % 8 == 1)
                {
                    simType = "ML";
                    centType = "Degree";
                    origType = "Push";
                }
                else if (id % 8 == 2)
                {
                    simType = "ML";
                    centType = "Degree";
                    origType = "Pull";
                }
                else if (id % 8 == 3)
                {
                    simType = "Manual";
                    centType = "Degree";
                    origType = "Push";
                }
                else if (id % 8 == 4)
                {
                    simType = "Manual";
                    centType = "Degree";
                    origType = "Pull";
                }
                else if (id % 8 == 5)
                {
                    simType = "Manual";
                    centType = "Closeness";
                    origType = "Push";
                }
                else if (id % 8 == 6)
                {
                    simType = "Manual";
                    centType = "Closeness";
                    origType = "Pull";
                }
                else if (id % 8 == 7)
                {
                    simType = "Manual";
                    centType = "Betweenness";
                    origType = "Push";
                }
                else if (id % 8 == 0)
                {
                    simType = "Manual";
                    centType = "Betweenness";
                    origType = "Pull";
                }
                    std::ofstream file;
                    file.open("response_time.csv", std::ios_base::app);
                    file << std::to_string(rsuCount) + "," + 
                        std::to_string(id) + "," +
                        wsm->getContentId() + ","  +
                        std::to_string(recFirstTime.dbl()) + ","  +
                        std::to_string(simTime().dbl()) + ","  +
                        "REP" + "," + 
                        simType + "," +
                        centType + "," + 
                        origType + "\n";
                    file.close();
                    
                    rec = true;
            }
        }
        
        if (wsm->getMultimedia())
            multimediaData[wsm->getContentId()] = ContentWrapper(data);
        else 
            roadData[wsm->getContentId()] = ContentWrapper(data);

        segmentedMessages.erase(start, end);
        //for (auto i = segmentedMessages.begin(); i != segmentedMessages.end(); i++)
            //std::cout << "Message " << i->contentId << endl;
    }

    else
        reconstructed = "";

    return reconstructed;
}

void UnitHandler::sendAcknowledgement(Message *wsm)
{
    //std::cout << "Node " << myId << " sending acknowledgement to Node " << wsm->getSource() << endl;

    // Create acknowledgement message
    Message *ack = new Message();
    populateWSM(ack);
    // Define message identifiers
    ack->setSource(myId);
    ack->setDest(wsm->getSource());
    ack->setSenderAddress(myId);
    ack->setSenderPosition(curPosition);
    // Define message properties
    ack->setType(MessageType::ACKNOWLEDGEMENT);
    ack->setOriginMessage(wsm->getOriginMessage());
    ack->setContentId(wsm->getContentId());
    ack->setSegments(wsm->getSegments());
    ack->setSegmentNumber(wsm->getSegmentNumber());
    ack->setMultimedia(wsm->getMultimedia());
    // Schedule message
    scheduleAt(simTime() + 1 + uniform(0.01, 0.5), ack);
}

void UnitHandler::debugPrint(routingDict routing)
{
    std::cout << "Node " << myId << endl;
    for (auto i = routing.begin(); i != routing.end(); i++)
    {
        std::cout << "{ ";
        for (auto j = i->second.begin(); j != i->second.end(); j++)
            std::cout << *j << " ";
        std::cout << "}" << endl;
    }
}

// ---------------- Threshold ---------------- //

void UnitHandler::thresholdControl()
{
    // Create threshold request message
    Message *request = new Message();
    populateWSM(request);
    // Define request properties
    request->setState(CurrentState::CACHING);
    scheduleAt(simTime() + 30, request);
}

