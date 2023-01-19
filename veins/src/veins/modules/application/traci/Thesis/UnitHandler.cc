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

        std::vector<long> route;
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

/*
    // Print routing table, for debugging
    std::cout << "Node " << myId << endl;
    for (auto i = routing.begin(); i != routing.end(); i++)
    {
        std::cout << i->first << ": { ";
        for (auto j = i->second.begin(); j != i->second.end(); j++)
            std::cout << *j << " ";
        std::cout << "}\n";
    }
*/  
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

    // TODO: check if it works
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

                std::cout << "Node " << myId << endl;
                // TODO: Optimize rsu search
                for (auto i = routing.begin(); i != routing.end(); i++)
                {
                    std::cout << i->first << ": { ";
                    for (auto j = i->second.begin(); j != i->second.end(); j++)
                        std::cout << *j << " ";
                    std::cout << "}\n";

                    auto it = std::find(i->second.begin(), i->second.end(), wsm->getDest());
                    if (it != i->second.end())
                        result++;
                }

                results->setMsgInfo(result);
                scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), results);

                calculating = false;

                // TODO: Put acknowledgements here
            }
        }
    }

    // TODO: Clear routing table
    delete(wsm);
}

void UnitHandler::repeatingMsg(Message *wsm, dataDeque &acks)
{
    // TODO: Check if it works
    // TODO: Maybe change it so that the search happens only when it has surpassed attempts
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
            auto lastElem = acks.back();
            std::swap(*it, lastElem);
            acks.pop_back();
        }

        else
        {
            it->attempts++;

            // If message hasn't been acknowledged, reschedule repeat check
            wsm->setTtl(wsm->getTtl() + 1);
            scheduleAt(simTime() + uniform(0.01, 0.2), wsm->dup());

            // And resend the message with higher hop count
            wsm->setState(CurrentState::SENDING);
            scheduleAt(simTime() + uniform(0.01, 0.2), wsm->dup());
        }
        
        delete(wsm);
    } 
}

void UnitHandler::cachingMsg(
    Message* wsm,
    simtime_t limit, 
    dataDeque &messages, 
    dataDeque &candidates)
{
    simtime_t now = simTime();

    EV << "Threshold control...\n";
    if (!messages.empty())
    {
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
        Message *reply = new Message();
        populateWSM(reply);
        reply->setSenderAddress(myId);
        reply->setSenderPosition(curPosition);
        reply->setSource(myId);
        reply->setDest(wsm->getSource());
        reply->setRoadData(wsm->getRoadData());
        reply->setType(MessageType::REPLY);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), reply);

        // TODO: Put acknowledgements here
    }

    // Forward the message to othre nodes
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

        // TODO: Send acknowledgement here
    }
}

// ------------ On Route Handlers ------------ //

void UnitHandler::onRouteReq(
    Message *wsm, 
    routingDict &routing,
    dataDeque &acks)
{
    std::vector<long> route = wsm->getRoute();
    if (!inRoute(route))
    {
        std::vector<long> previous = wsm->getPreviousNodes();

        // Create route reply
        Message *reply = new Message();
        populateWSM(reply);
        reply->setSenderAddress(myId);
        reply->setSenderPosition(curPosition);
        reply->setRecipientAddress(route.back());
        reply->setSource(myId);
        reply->setDest(wsm->getSource());
        reply->setTtl(route.size());
        std::vector<long> debug = wsm->getRsuRoute();
        wsm->setRsuRoute(debug);
        debug.push_back(0);
        reply->setRsuRoute(debug);
        previous.push_back(myId);
        reply->setPreviousNodes(previous);
        route.pop_back();
        reply->setRoute(route);
        reply->setType(MessageType::ROUTE_REPLY);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), reply);

        // TODO: Put acknowledgement here

        // Add your id to the path list and broadcast again
        route = wsm->getRoute();
        route.push_back(myId);
        wsm->setRoute(route);
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.2 + uniform(0.01, 0.5), wsm->dup());
    }
}

void UnitHandler::onRouteReply(Message *wsm, routingDict &routing)
{
    // If it's meant for you, accept
    if (wsm->getDest() == myId)
    {
        // TODO: Send acknowledgement
    }

    // Else, forward
    else 
    {
        std::vector<long> route = wsm->getRoute();
        std::vector<long> previous = wsm->getPreviousNodes();

        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);
        wsm->setRecipientAddress(route.back());
                
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
    // TODO: Change depending on the function of the routing table

    Message *reply = new Message();
    populateWSM(reply);
    reply->setSenderAddress(myId);
    reply->setSenderPosition(curPosition);
    reply->setSource(myId);
    reply->setDest(wsm->getSource());
    reply->setType(MessageType::CENTRALITY_REPLY);
    reply->setCentrality(CentralityType::DEGREE);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), reply);

    // TODO: Acknowledgement here
}

void UnitHandler::onBetweennessReq(Message *wsm, dataDeque &acks)
{
    if (!calculating)
    {
        std::cout << "GOT REQUEST, NODE " << myId << endl;
        calculating = true;

        // Create route request
        Message *req = new Message();
        populateWSM(req);

        req->setSenderAddress(myId);
        req->setSenderPosition(curPosition);
        req->setSource(myId);

        std::vector<long> route = wsm->getRoute();
        route.push_back(myId);
        req->setRoute(route);
        req->setTtl(ttl);
        req->setType(MessageType::ROUTE_REQ);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), req);

        // Broadcast the betweenness request to other cars
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.2 + uniform(0.01, 0.5), wsm->dup());

        Message *collect = new Message();
        populateWSM(collect);

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
    if (wsm->getDest() == myId)
    {
        EV << "Acknowledgement received..." << endl;
        // TODO: Validate if it works
        // Find the message you received acknowledgement for
        auto it = std::find_if(acks.begin(), acks.end(),
            [&wsm](MessageData data) { return (wsm->getMsgInfo() == data.id); });

        if (it != acks.end())
        {
            // To optimize deletion, swap element with last
            // And delete it, taking advantage of deque pop_back
            auto lastElem = acks.back();
            std::swap(*it, lastElem);
            acks.pop_back();
        }
    }

    else 
    {
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);
        scheduleAt(simTime() + uniform(0.01, 0.2), wsm->dup());
    }

    delete(wsm);
}

// ------------ Routing Functions ------------ //

bool UnitHandler::inRoute(std::vector<long> route)
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
    // TODO: Check if this is working
    if (wsm->getSource() == myId)
        return false;

    bool update = false;
    bool insertion = false;

    // Check if routing table needs an update
    std::vector<long> route = wsm->getRoute();
    std::vector<long> previous = wsm->getPreviousNodes();
    std::vector<long> debug = wsm->getRsuRoute();
    if (debug.empty())
        debug.push_back(wsm->getSource());
    debug.push_back(myId);
    wsm->setRsuRoute(debug);

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
    std::vector<long> path = wsm->getRoute();
    bool update = false;

    update = auxilaryRoute(wsm, path, routing);
    if (!update)
        return false;

    val = wsm->getSource();
    send = wsm->getSenderAddress();
    heh= wsm->getType();

    path = wsm->getPreviousNodes();
    return auxilaryRoute(wsm, path, routing);
/*
    if (wsm->getType() == MessageType::ROUTE_REQ)
    {
        long id = wsm->getRoute().front();
        if (routing.find(id) != routing.end())
        {
            if (wsm->getHops() > routing[id].size())
                return false;
        }

        routing[id] = wsm->getRoute();
    }
    
    else if (wsm->getType() == MessageType::ROUTE_REPLY)
    {
        long id = wsm->getPreviousNodes().front();
        if (routing.find(id) != routing.end())
        {
            if (wsm->getHops() > routing[id].size())
                return false;
        }

        routing[id] = wsm->getPreviousNodes();
    }

    return true;*/
}

bool UnitHandler::auxilaryRoute(Message *wsm, std::vector<long> &path, routingDict &routing)
{   
    // Variables for map check
    std::vector<long> returnPath;
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
            /*
            if ((myId == 22 || myId == 124))
            {   std::cout << "MessageType " << heh << endl;
                std::cout << "Node " << myId << " Old path: { ";
                for (auto j = it->second.begin(); j != it->second.end(); j++)
                    std::cout<< *j << " ";
                std::cout << "} New path: { ";
                for (auto k = returnPath.begin(); k != returnPath.end(); k++)
                    std::cout<< *k << " ";
                std::cout << "} Source " << val << " Sender " << send << "\n";
            }*/

            if ((myId == 22 || myId == 124))
            {
                std::vector<long> debug = wsm->getRsuRoute();

                std::cout << "Node " << myId;
                std::cout << " Message path: { ";
                for (auto k = debug.begin(); k != debug.end(); k++)
                    std::cout << *k << " ";
                
                std::cout << "}\n";
            }
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

    if (!candidates.empty())
    {
        auto it = std::find_if(candidates.begin(), candidates.end(),
            [&wsm](MessageData data) 
            { return (wsm->getCreationTime() == data.timestamp && wsm->getSource() == data.source); });
    
        if (it != candidates.end())
            return false;
    }

    messages.push_front(MessageData(wsm));
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
    Message *req = new Message();
    populateWSM(req);

    req->setState(CurrentState::CACHING);
    scheduleAt(simTime() + 15, req);
}