#include "veins/modules/application/traci/Thesis/UnitHandler.h"

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
                initializingMsg(wsm);
                break;
            case CurrentState::SENDING:
                sendingMsg(wsm);
                break;
            case CurrentState::REQUESTING:
                requestingMsg(wsm);
                break;            
            case CurrentState::REPEATING:
                repeatingMsg(wsm, pendingAck);
                break;     
            case CurrentState::COLLECTING:
                collectingMsg(wsm, routingTable, pendingAck);
                break;      
            case CurrentState::CACHING:
                cachingMsg(
                    wsm, 
                    threshold, 
                    storedMessages, 
                    candidateMessages);
                break;
            default:
                break;
        }
    }
}

void UnitHandler::sendingMsg(Message *wsm)
{
    // If the message doesn't exceed max hop count
    if (wsm->getHops() < wsm->getTtl())
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

void UnitHandler::requestingMsg(Message *wsm)
{
    // Create request
    Message *req = new Message();
    populateWSM(req);
    // Define message identifiers
    req->setSenderAddress(myId);
    req->setSenderPosition(curPosition);
    req->setSource(myId);
    // Define message properties
    CentralityType centrality = wsm->getCentrality();
    req->setCentrality(centrality);
    req->setType(MessageType::CENTRALITY_REQ);
    req->setTtl(ttl);

    // Handle different centralities
    if (centrality == CentralityType::DEGREE)
        req->setTtl(1);

    else if (centrality == CentralityType::CLOSENESS)
    {
        req->setType(MessageType::ROUTE_REQ);

        pathVector route;
        route.push_back(myId);
        req->setRoute(route);
    }

    // Schedule message to be sent at time = 75s
    scheduleAt(simTime() + 25, req->dup());

    // Create collection message
    Message *collect = new Message();
    populateWSM(collect);
    // Define message ids and properties
    collect->setDest(myId);
    collect->setType(req->getType());
    collect->setCentrality(centrality);
    collect->setState(CurrentState::COLLECTING);

    scheduleAt(simTime() + 40, collect);
    delete(wsm);
}    
    
void UnitHandler::collectingMsg(                
    Message *wsm,
    routingDict &routing,
    dataDeque &acks)
{
    float result = 0;

    // Handle different message types
    MessageType type = wsm->getType();
    if (type == MessageType::ROUTE_REQ)
    {
        for (auto i = routing.begin(); i != routing.end(); i++)
            result += i->second.size();

        result = result / float(routing.size());
        std::cout << "Average shortest route length is " << result << endl;

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
            // If you made the betweenness request
            if (wsm->getDest() == myId)
            {
                result = betweenness;
                std::cout << "Betweenness centrality is " << result << endl;
            }

            else
            { 
                int maxHops = ttl;

                if (routing.find(wsm->getDest()) != routing.end())
                    maxHops = routing[wsm->getDest()].size() + 2;

                // Create results message
                Message *results = new Message();
                populateWSM(results);
                // Define message ids and properties
                results->setSenderAddress(myId);
                results->setSenderPosition(curPosition);
                results->setSource(myId);
                results->setDest(wsm->getDest());
                results->setTtl(maxHops);
                results->setType(MessageType::CENTRALITY_REPLY);
                results->setCentrality(CentralityType::BETWEENNESS); 

                // Calculate result and send it to the RSU
                for (auto i = routing.begin(); i != routing.end(); i++)
                {
                    auto it = std::find(i->second.begin(), i->second.end(), wsm->getDest());
                    if (it != i->second.end())
                        result++;
                }

                results->setMsgInfo(result);
                scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), results);

                // Calculation complete, remove RSU from vector
                auto it = std::find(requestedBy.begin(), requestedBy.end(), wsm->getDest());
                if (it != requestedBy.end())
                {
                    long id = requestedBy.back();
                    std::swap(*it, id);
                    requestedBy.pop_back();
                }

                // TODO: Put acknowledgements here
            }
        }
    }

    if (requestedBy.empty())
        routing.clear();
    delete(wsm);
}

void UnitHandler::repeatingMsg(Message *wsm, dataDeque &acks)
{
    // Search if message in the acknowledgemnets deque
    auto it = std::find_if(acks.begin(), acks.end(),
        [&wsm](MessageData data) { return (wsm->getMsgId() == data.id); });

    if (it != acks.end())
    {   
        // Resend the message a limited amount of times before deleting
        if (it->attempts > 2)
        {
            // To optimize deletion, swap element with last
            // And delete it, taking advantage of deque pop_back
            MessageData lastElem = acks.back();
            std::swap(*it, lastElem);
            acks.pop_back();

            delete(wsm);
        }

        else
        {
            it->attempts++;

            // If message hasn't been acknowledged, reschedule repeat check
            wsm->setTtl(wsm->getTtl());
            scheduleAt(simTime() + uniform(0.01, 0.2), wsm->dup());

            // And resend the message with higher hop count
            wsm->setState(CurrentState::SENDING);
            scheduleAt(simTime() + uniform(0.01, 0.2), wsm);
        }
    } 

    else 
        delete(wsm);
}

void UnitHandler::cachingMsg(
    Message* wsm,
    simtime_t limit, 
    dataDeque &messages, 
    dataDeque &candidates)
{
    simtime_t now = simTime();

    // Perform threshold control
    if (!messages.empty())
    {
        // Transfer stale messages to candidate deque
        auto it = messages.rbegin();
        while (now - it->timestamp > limit)
        {
            candidates.push_front(*it);
            messages.pop_back();

            if (messages.empty())
                break;

            it = messages.rbegin();
        }
    }

    delete(wsm);
}

// --------------- On Message ---------------- //

void UnitHandler::onWSM(BaseFrame1609_4* frame)
{
    Message* wsm = check_and_cast<Message*>(frame);
    if (receiveMessage(wsm, storedMessages, candidateMessages, routingTable))
    {
        EV << "Message has been accepted by node with ID " << myId << endl;
        // Change color depending on what type of unit you are
        switch (unit)
        {
            case UnitType::VEHICLE:
                findHost()->getDisplayString().setTagArg("i", 1, "green");
                break;
            case UnitType::RSU:
                findHost()->getDisplayString().setTagArg("i", 1, "gold");
                break;
            case UnitType::ORIGIN:
                findHost()->getDisplayString().setTagArg("i", 1, "red");
                break;
            default:
                break;
        }

        // Handlers for different message types
        MessageType type = wsm->getType();
        switch (type)
        {
            case MessageType::BROADCAST:
                onBroadcast(wsm, roadStatus);
                break;
            case MessageType::REQUEST:
                onRequest(wsm, roadStatus, pendingAck);
                break;
            case MessageType::REPLY:
                onReply(wsm);
                break;
            case MessageType::ROUTE_REQ:
                onRouteReq(wsm, routingTable, pendingAck);
                break;
            case MessageType::ROUTE_REPLY:
                onRouteReply(wsm, routingTable);
                break;
            case MessageType::CENTRALITY_REQ:
                onCentralityReq(wsm, routingTable, pendingAck);
                break;
            case MessageType::CENTRALITY_REPLY:
                onCentralityReply(wsm);
                break;
            case MessageType::ACKNOWLEDGEMENT:
                onAcknowledgement(wsm, pendingAck);
                break;
            default:
                break;
        }
    }
}

void UnitHandler::onRequest(
    Message *wsm, 
    roadsDeque &roads,
    dataDeque &acks)
{
    auto it = std::find(roads.begin(), roads.end(), wsm->getRoadData());

    if (it != roads.end())
    {
        // Create reply message
        Message *reply = new Message();
        populateWSM(reply);
        // Define message ids and properties
        reply->setSenderAddress(myId);
        reply->setSenderPosition(curPosition);
        reply->setSource(myId);
        reply->setDest(wsm->getSource());
        reply->setRoadData(wsm->getRoadData());
        reply->setType(MessageType::REPLY);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), reply->dup());
        
        // Push message in pending acknowledgements deque
        acks.push_front(MessageData(reply));
        reply->setState(CurrentState::REPEATING);

        scheduleAt(simTime() + 5 + uniform(0.01, 0.2), reply);
    }

    // Forward the message to other nodes
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);
    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
}

void UnitHandler::onReply(Message *wsm)
{
    // If not meant for you, forward
    if (wsm->getDest() != myId)
    {
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);
        scheduleAt(simTime() + uniform(0.01, 0.2), wsm->dup());
    }

    else 
    {
        findHost()->getDisplayString().setTagArg("i", 1, "purple");
        std::cout << "Node " << myId << " received message after " << wsm->getHops() << " hop(s).\n";
        
        // Create acknowledgement message
        Message *ack = new Message();
        populateWSM(ack);
        // Define message ids and properties
        ack->setSenderAddress(myId);
        ack->setSenderPosition(curPosition);
        ack->setSource(myId);
        ack->setDest(wsm->getSource());
        ack->setAckInfo(wsm->getCreationTime());
        ack->setTtl(wsm->getHops());
        ack->setType(MessageType::ACKNOWLEDGEMENT);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), ack);
    }
}

// ------------ On Route Handlers ------------ //

void UnitHandler::onRouteReq(
    Message *wsm, 
    routingDict &routing,
    dataDeque &acks)
{
    pathVector route = wsm->getRoute();
    pathVector previous = wsm->getPreviousNodes();
    
    // Create route reply
    Message *reply = new Message();
    populateWSM(reply);
    // Define reply identifiers
    reply->setSenderAddress(myId);
    reply->setSenderPosition(curPosition);
    reply->setRecipientAddress(route.back());
    reply->setSource(myId);
    reply->setDest(wsm->getSource());
    reply->setTtl(route.size());
    // Change routing details
    previous.push_back(myId);
    reply->setPreviousNodes(previous);
    route.pop_back();
    reply->setRoute(route);
    reply->setType(MessageType::ROUTE_REPLY);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), reply);

    // TODO: Remove from comment to handle acks
    // Push message in pending acknowledgements deque
    //acks.push_front(MessageData(reply));
    //reply->setState(CurrentState::REPEATING);

    //scheduleAt(simTime() + 5 + uniform(0.01, 0.2), reply);

    // Add your id to the route vector and broadcast again
    route = wsm->getRoute();
    route.push_back(myId);
    wsm->setRoute(route);
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 0.2 + uniform(0.01, 0.5), wsm->dup());
}

void UnitHandler::onRouteReply(Message *wsm, routingDict &routing)
{
    // If it's meant for you, accept
    if (wsm->getDest() == myId)
    {
        EV << "Node " << myId << " accepted route reply.\n";

        // TODO: Remove from comment to handle acks
        /*
        // Create acknowledgement message
        Message *ack = new Message();
        populateWSM(ack);
        // Define message ids and properties
        ack->setSenderAddress(myId);
        ack->setSenderPosition(curPosition);
        ack->setSource(myId);
        ack->setDest(wsm->getSource());
        ack->setAckInfo(wsm->getCreationTime());
        ack->setTtl(wsm->getHops());
        ack->setType(MessageType::ACKNOWLEDGEMENT);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), ack);*/
    }

    // Else, forward
    else 
    {
        pathVector route = wsm->getRoute();
        pathVector previous = wsm->getPreviousNodes();

        // Change message identifiers
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);
        wsm->setRecipientAddress(route.back());
        // Change routing details
        previous.push_back(myId);
        wsm->setPreviousNodes(previous);
        route.pop_back();
        wsm->setRoute(route);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup());
    }
}

// ---------- On Centrality Request ---------- //

void UnitHandler::onCentralityReq(
    Message *wsm, 
    routingDict &routing,
    dataDeque &acks)
{
    CentralityType centrality = wsm->getCentrality();
    switch (centrality)
    {
        case CentralityType::DEGREE:
            onDegreeReq(wsm, routing, acks);
            break;
        case CentralityType::BETWEENNESS:
            onBetweennessReq(wsm, acks);
            break;
        default:
            break;
    }
}

void UnitHandler::onDegreeReq(
    Message *wsm, 
    routingDict &routing,
    dataDeque &acks)
{
    // Create reply message
    Message *reply = new Message();
    populateWSM(reply);
    // Define reply ids and properties
    reply->setSenderAddress(myId);
    reply->setSenderPosition(curPosition);
    reply->setSource(myId);
    reply->setDest(wsm->getSource());
    reply->setType(MessageType::CENTRALITY_REPLY);
    reply->setCentrality(CentralityType::DEGREE);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), reply->dup());

    // Push message in pending acknowledgements deque
    acks.push_front(MessageData(reply));
    reply->setState(CurrentState::REPEATING);

    scheduleAt(simTime() + 5 + uniform(0.01, 0.2), reply);
}

void UnitHandler::onBetweennessReq(Message *wsm, dataDeque &acks)
{
    auto it = std::find(requestedBy.begin(), requestedBy.end(), wsm->getSource());
    if (it == requestedBy.end())
    {
        // To ensure node doesnt respond twice to the same request
        requestedBy.push_back(wsm->getSource());

        // Create route request
        Message *req = new Message();
        populateWSM(req);
        // Define request identifiers
        req->setSenderAddress(myId);
        req->setSenderPosition(curPosition);
        req->setSource(myId);
        // Define request details
        pathVector route = wsm->getRoute();
        route.push_back(myId);
        req->setRoute(route);
        req->setTtl(ttl);
        req->setType(MessageType::ROUTE_REQ);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), req);

        // Broadcast the betweenness request to other cars
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.2 + uniform(0.01, 0.5), wsm->dup());

        // Create collection message
        Message *collect = new Message();
        populateWSM(collect);
        // Define collection details
        collect->setDest(wsm->getSource());
        collect->setState(CurrentState::COLLECTING);
        collect->setType(MessageType::CENTRALITY_REQ);
        collect->setCentrality(CentralityType::BETWEENNESS);

        scheduleAt(simTime() + 10 + uniform(0.01, 0.5), collect);
    }
}

// ----------- On Centrality Reply ----------- //

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

// ----------- On Acknowledgement ------------ //

void UnitHandler::onAcknowledgement(Message *wsm, dataDeque &acks)
{
    if (wsm->getDest() != myId)
    {
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);
        scheduleAt(simTime() + uniform(0.01, 0.2), wsm->dup());
    }

    else
    {
        EV << "Acknowledgement received..." << endl;
        // Find the message you received acknowledgement for
        auto it = std::find_if(acks.begin(), acks.end(),
            [&wsm](MessageData data) 
            { return (wsm->getSource() == data.dest); });

        if (it != acks.end())
        {
            // To optimize deletion, swap element with last
            // And delete it, taking advantage of deque pop_back
            MessageData lastElem = acks.back();
            std::swap(*it, lastElem);
            acks.pop_back();
        }
    }
}
// ------------ Routing Functions ------------ //

bool UnitHandler::inRoute(pathVector route)
{
    // Search the current route
    auto it = std::find_if(route.begin(), route.end(),
        [this](long id) { return (id == this->myId); });

    // Return if you're already in the route
    return (it != route.end());
}

// ------------ Message Insertion ------------ //

bool UnitHandler::receiveMessage(
    Message *wsm,
    dataDeque &messages,
    dataDeque &candidates,
    routingDict &routing)
{
    if (wsm->getSource() == myId)
        return false;

    bool update = false;
    bool insertion = false;

    // Check if routing table needs an update
    pathVector route = wsm->getRoute();
    pathVector previous = wsm->getPreviousNodes();

    // If you're already in route, return false
    if (inRoute(route))
        return false;

    // Check if you can extract route info
    if (!route.empty() || !previous.empty())
    {
        update = routingTableUpdate(wsm, routing);
        if (!update)
            return false;
    }

    // Flush the message list and check if you need to insert message
    flushList(messages, candidates);
    insertion = insertMessage(wsm, messages, candidates);

    // If an insertion or an update has been made, return true
    return (update || insertion);
}

bool UnitHandler::routingTableUpdate(Message *wsm, routingDict &routing)
{
    pathVector path = wsm->getRoute();
    bool update = false;

    update = auxilaryRoute(path, routing);
    if (!update)
        return false;

    path = wsm->getPreviousNodes();
    return auxilaryRoute(path, routing);
}

bool UnitHandler::auxilaryRoute(pathVector &path, routingDict &routing)
{   
    // Variables for map check
    pathVector returnPath;
    long id;

    // While the path still has nodes in it
    while (!path.empty())
    {
        id = path.back();
        path.pop_back();
        returnPath.push_back(id);

        if (id == myId)
            continue;

        auto it = routing.find(id);
        if (it != routing.end())
        {
            // If message path is worse than saved path, abort message
            if (returnPath.size() > it->second.size())
                return false;
        }

        routing[id] = returnPath;
    }

    return true;
}

bool UnitHandler::insertMessage(
    Message *wsm,
    dataDeque &messages,
    dataDeque &candidates)
{
    // If there are messages, insert accordingly sorted by generated time
    if (!messages.empty())
    {
        simtime_t timestamp = wsm->getCreationTime();
        for (auto i = messages.begin(); i != messages.end(); i++)
        {
            if (i->timestamp > timestamp)
                continue;

            else if (i->timestamp == timestamp)
            {
                if (i->source == wsm->getSource() && i->type == wsm->getType())
                    return false;
                else 
                    continue;
            }

            else if (i->timestamp < timestamp)
            {
                messages.insert(i, MessageData(wsm));
                return true;
            }
        }
    }

    // If you didn't find message in stored messages, check candidate vector
    if (!candidates.empty())
    {
        auto it = std::find_if(candidates.begin(), candidates.end(),
            [&wsm](MessageData data) 
            { return (wsm->getCreationTime() == data.timestamp && wsm->getSource() == data.source); });
    
        if (it != candidates.end())
            return false;
    }

    messages.push_back(MessageData(wsm));
    return true;
}

void UnitHandler::flushList(dataDeque &messages, dataDeque &candidates)
{
    // If flushing needs to be done
    if (messages.size() + candidates.size() >= capacity)
    {
        // If you have no candidate messages for deletion
        if (candidates.empty())
        {
            if (messages.size() < flushed)
                messages.clear();
            
            else 
            {
                if (policy == CachingPolicy::FIFO)
                    sort(messages.begin(), messages.end(), MessageData::sortFIFO);
                else if (policy == CachingPolicy::LRU)
                    sort(messages.begin(), messages.end(), MessageData::sortLRU);
                else if (policy == CachingPolicy::LFU)
                    sort(messages.begin(), messages.end(), MessageData::sortLFU);

                for (int i = 0; i < flushed; i++)
                    messages.pop_front();
                
                sort(messages.begin(), messages.end(), MessageData::sortRestore);
            }
        }

        // If you do, delete from the candidate messages
        else 
        {
            if (candidates.size() < flushed)
                candidates.clear();
            
            else 
            {                
                if (policy == CachingPolicy::FIFO)
                    sort(candidates.begin(), candidates.end(), MessageData::sortFIFO);
                else if (policy == CachingPolicy::LRU)
                    sort(candidates.begin(), candidates.end(), MessageData::sortLRU);
                else if (policy == CachingPolicy::LFU)
                    sort(candidates.begin(), candidates.end(), MessageData::sortLFU);

                for (int i = 0; i < flushed; i++)
                    candidates.pop_front();
                
                sort(candidates.begin(), candidates.end(), MessageData::sortRestore);

            }
        }
    }
}

// ---------------- Threshold ---------------- //

void UnitHandler::thresholdControl()
{
    // Create threshold request message
    Message *req = new Message();
    populateWSM(req);
    // Define request properties
    req->setState(CurrentState::CACHING);
    scheduleAt(simTime() + 15, req);
}