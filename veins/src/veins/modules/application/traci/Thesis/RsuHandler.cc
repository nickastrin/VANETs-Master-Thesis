#include "veins/modules/application/traci/Thesis/RsuHandler.h"

using namespace veins;
using namespace omnetpp;

Define_Module(veins::RsuHandler);

// -------------- INITIALIZERS --------------- //

void RsuHandler::initialize(int stage)
{
    // Initializing variables
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0)
    {
        // Initialize centrality metrics
        degree = 0;
        closeness = 0;
        betweenness = 0;

        // Variables for the caching algorithm
        capacity = 15;
        threshold = 30;
        flushed = 3;

        signalRange = 300;
        maxHops = 20;

        policy = cachingPolicy::LRU;
        testCase = scenario::CENTRALITY;

        testScenario(testCase);
        thresholdRequest();
    }
}

void RsuHandler::testScenario(scenario test)
{
    if (test == scenario::CENTRALITY)
    {
        // Create a request for selected centrality
        Message * request = new Message();
        populateWSM(request);

        request->setState(currentState::INITIALIZING);
        request->setCentrality(selectedCentrality::BETWEENNESS);

        scheduleAt(simTime() + 10, request);
    }

    else if (test == scenario::CACHE)
    {
        // Create dummy messages to test the algorithm
        Message * dummy = new Message();
        populateWSM(dummy);

        dummy->setState(currentState::TESTING);
        receiveMessage(dummy, messageList, candidateList);
    }
}

// -------------- SELF MESSAGE --------------- //

void RsuHandler::handleSelfMsg(cMessage * msg)
{
    if (Message * wsm = dynamic_cast<Message *>(msg))
    {
        currentState state = wsm->getState();

        switch (state)
        {
            case currentState::INITIALIZING:
                initializingMessage(wsm);
                break;            
            case currentState::SENDING:
                sendingMessage(wsm);
                break;
            case currentState::COLLECTING:
                collectingMessage(wsm, distanceList, ackList);
                break;
            case currentState::CACHING:
                thresholdControl(threshold, messageList, candidateList);
                break;
            case currentState::REPEATING:
                repeatingMessage(wsm, ackList);
                break;
            default:
                break;
        }
    }
}

void RsuHandler::initializingMessage(Message * wsm)
{
    EV << "Initializing centrality calculations...\n";

    Message * request = new Message();
    populateWSM(request);

    request->setSenderAddress(myId);
    request->setSenderPosition(curPosition);
    request->setSource(myId);
    request->setType(messageType::RSU_REQUEST);

    selectedCentrality centrality = wsm->getCentrality();

    switch (centrality)
    {
        case selectedCentrality::DEGREE:
            request->setCentrality(selectedCentrality::DEGREE);
            break;
        case selectedCentrality::CLOSENESS:
        {
            request->setCentrality(selectedCentrality::CLOSENESS);

            std::list<long> path;
            path.push_front(myId);
            request->setPathList(path);
            request->setMaxHops(maxHops);

            break;
        }
        case selectedCentrality::BETWEENNESS:
        {
            // TODO: Maybe needs changing
            request->setCentrality(selectedCentrality::BETWEENNESS);
            break;
        }
        default:
            break;
    }
    
    scheduleAt(simTime() + 65, request);

    // Call collection after 10s
    Message * collect = new Message();
    populateWSM(collect);

    collect->setTarget(myId);
    collect->setCentrality(wsm->getCentrality());
    collect->setState(currentState::COLLECTING);

    scheduleAt(simTime() + 80, collect);
}

void RsuHandler::sendingMessage(Message * wsm)
{
    // If the message doesn't exceed max hop count, 
    if (wsm->getHops() < wsm->getMaxHops())
    {
        wsm->setHops(wsm->getHops() + 1);
        sendDown(wsm->dup());
    }

    else
    {
        EV << "RSU with ID " << myId << ", deleting message with ID " << wsm->getSenderAddress() << endl;

        stopService();
        delete(wsm);
    }
}

void RsuHandler::collectingMessage(Message * wsm, std::list<Tuple> & array, std::list<Tuple> & ackArray)
{
    selectedCentrality centrality = wsm->getCentrality();
    float result = 0;

    switch (centrality)
    {
        case selectedCentrality::NONE:
            break;
        case selectedCentrality::DEGREE:
        {
            result = degree;
            EV << "Degree centrality is " << result << endl;

            break;
        }
        case selectedCentrality::CLOSENESS:
        {
            for (auto i = array.begin(); i != array.end(); i++)
                result += i->hops;
            
            result = float(result) / float(array.size());
            EV << "Closeness centrality is " << result << endl;

            break;
        }
        // TODO: Maybe changes are needed
        case selectedCentrality::BETWEENNESS:
        {
            if (wsm->getTarget() == myId)
            {
                result = betweenness;
                EV << "Betweenness centrality is " << result << endl;
            }

            else 
            {
                Message * reply = new Message();
                populateWSM(reply);

                reply->setSenderAddress(myId);
                reply->setSenderPosition(curPosition);
                reply->setTarget(wsm->getTarget());
                reply->setMaxHops(maxHops);
                reply->setType(messageType::RSU_REPLY);
                reply->setCentrality(selectedCentrality::BETWEENNESS);

                for (auto i = array.begin(); i != array.end(); i++)
                {
                    if (!i->rsu.empty())
                    {
                        for (auto j = i->rsu.begin(); j != i->rsu.end(); j++)
                        {
                            if (*j == wsm->getTarget())
                            {
                                reply->setCentralityData(reply->getCentralityData() + 1);
                                break;
                            }
                        }
                    }
                }

                scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), reply->dup());
                
                ackArray.push_front(Tuple(reply));
                reply->setState(currentState::REPEATING);
                scheduleAt(simTime() + 5 + uniform(0.01, 0.2), reply);
            }
        }
    }
}

void RsuHandler::repeatingMessage(Message * wsm, std::list<Tuple> & array)
{
    for (auto i = array.begin(); i != array.end(); i++)
    {
        if (i->retries > 1)
        {
            array.erase(i);
            break;
        }

        if (wsm->getTarget() == i->target && wsm->getCreationTime() == i->timestamp)
        {
            i->retries += 1;
            
            // If message hasn't been acknowledged, reschedule repeat check
            wsm->setMaxHops(wsm->getMaxHops() + 2);
            scheduleAt(simTime() + uniform(0.01, 0.2), wsm->dup());
            // And resend the message with higher hop count
            wsm->setState(currentState::SENDING);
            scheduleAt(simTime() + uniform(0.01, 0.2), wsm->dup());

            break;
        }
    }
}

// --------------- ON MESSAGE ---------------- //

void RsuHandler::onWSA(DemoServiceAdvertisment* wsa)
{
    // If this RSU receives a WSA for Service 42, it will tune to the channel
    if (wsa->getPsid() == 42) 
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
}

void RsuHandler::onWSM(BaseFrame1609_4* frame)
{
    Message* wsm = check_and_cast<Message*>(frame);
    if (receiveMessage(wsm, messageList, candidateList))
    {
        EV << "Message accepted by the RSU... " << myId << endl;

        // Change color if you accepted the message
        findHost()->getDisplayString().setTagArg("i", 1, "black");

        messageType type = wsm->getType();

        switch (type)
        {
            case messageType::BROADCAST:
                handleBroadcast(wsm);
                break;
            case messageType::REQUEST:
                handleRequest(wsm, messageList, candidateList, ackList);
                break;
            case messageType::REPLY:
                handleReply(wsm);
                break;
            case messageType::RSU_REQUEST:
                handleRsuRequest(wsm, ackList);
                break;
            case messageType::RSU_REPLY:
                handleRsuReply(wsm, ackList);
                break;
            case messageType::CENTRALITY_REQUEST:
                handleCentralityRequest(wsm, ackList);
                break;
            case messageType::CENTRALITY_REPLY:
                handleCentralityReply(wsm, distanceList);
                break;
            case messageType::ACKNOWLEDGEMENT:
                handleAcknowledgement(wsm, ackList);
                break;
            default:
                break; 
        }
    }
}

// ------------ MESSAGE HANDLING ------------- //

void RsuHandler::handleBroadcast(Message * wsm)
{
    //Resend the message after 2s + delay
    wsm->setSenderPosition(curPosition);
    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
}

void RsuHandler::handleRequest(Message * wsm, std::list<Tuple> & msgArray, std::list<Tuple> & cndArray, std::list<Tuple> & ackArray)
{
    auxRequestHandler(wsm, msgArray, ackArray);
    auxRequestHandler(wsm, cndArray, ackArray);

    // Forward the message to other nodes
    wsm->setSenderPosition(curPosition);
    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
}

void RsuHandler::auxRequestHandler(Message * msg, std::list<Tuple> & array, std::list<Tuple> & ackArray)
{
    float delay = 1.0;

    for (auto i = array.begin(); i != array.end(); i++)
    {
        if (!i->roadData.empty())
        {
            i->usedFrequency += 1;
            i->lastUsed = simTime();

            Message * reply = new Message();
            populateWSM(reply);

            reply->setSenderAddress(myId);
            reply->setSenderPosition(curPosition);
            reply->setTarget(msg->getSenderAddress());
            reply->setType(messageType::REPLY);
            reply->setRoadData(i->roadData.c_str());

            delay += 0.1;
            scheduleAt(simTime() + delay + uniform(0.01, 0.2), reply->dup());

            ackArray.push_front(Tuple(reply));
            reply->setState(currentState::REPEATING);
            scheduleAt(simTime() + delay + 5 + uniform(0.01, 0.2), reply);
        }
    }
}

void RsuHandler::handleReply(Message * wsm)
{
    // If the message was not meant for you, forward
    if (wsm->getTarget() != myId)
    {
        //Resend the message after 2s + delay
        wsm->setSenderPosition(curPosition);
        scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
    }

    // If the message was meant for you, accept
    else 
    {
        findHost()->getDisplayString().setTagArg("i", 1, "gold");
        EV << "Message received after: " << wsm->getHops() + 1 << " hops.\n";

        Message * ack = new Message();
        populateWSM(ack);

        ack->setAckData(wsm->getCreationTime());
        ack->setMaxHops(wsm->getMaxHops());
        ack->setType(messageType::ACKNOWLEDGEMENT);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), ack);
    }
}

// --------------- RSU REQUEST --------------- //

void RsuHandler::handleRsuRequest(Message * wsm, std::list<Tuple> & ackArray)
{
    selectedCentrality centrality = wsm->getCentrality();

    switch (centrality)
    {
        case (selectedCentrality::NONE):
            break;
        case selectedCentrality::DEGREE:
            degreeRequest(wsm, ackArray);
            break;
        case selectedCentrality::CLOSENESS:
            closenessRequest(wsm, ackArray);
            break;
        case selectedCentrality::BETWEENNESS:
            betweennessRequest(wsm);
            break;
        default:
            break;
    }
}

void RsuHandler::degreeRequest(Message * msg, std::list<Tuple> & ackArray)
{
    EV << "RSU Algorithm for degree centrality called...\n";

    Message* reply = new Message();

    populateWSM(reply);

    reply->setSenderAddress(myId);
    reply->setSenderPosition(curPosition);
    reply->setTarget(msg->getSenderAddress());
    reply->setType(messageType::RSU_REPLY);
    reply->setCentrality(selectedCentrality::DEGREE);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), reply->dup());

    ackArray.push_front(Tuple(reply));
    reply->setState(currentState::REPEATING);
    scheduleAt(simTime() + 5 + uniform(0.01, 0.2), reply);
}

void RsuHandler::closenessRequest(Message * msg, std::list<Tuple> & ackArray)
{
    EV << "RSU Algorithm for closeness centrality called...\n";
    std::list<long> path = msg->getPathList();

    if (!inPath(path))
    {
        EV << "Creating shortest path reply, node " << myId << endl;

        Message * reply = new Message();
        populateWSM(reply);

        reply->setSenderAddress(myId);
        reply->setSenderPosition(curPosition);
        reply->setRecipientAddress(*path.begin());          // TODO: There might be a problem here
        reply->setTarget(msg->getSource());
        reply->setSource(msg->getSenderAddress());
        reply->setMaxHops(maxHops);

        path.pop_front();

        reply->setPathList(path);
        reply->setType(messageType::RSU_REPLY);
        reply->setCentrality(selectedCentrality::CLOSENESS);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), reply);
        EV << "Reply sent, message ID: " << reply->getSenderAddress() << endl;

        ackArray.push_front(Tuple(reply));
        reply->setState(currentState::REPEATING);
        scheduleAt(simTime() + 5 + uniform(0.01, 0.2), reply);

        // Add your id to the path list and broadcast it again
        path = msg->getPathList();
        path.push_front(myId);

        msg->setPathList(path);
        msg->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.2 + uniform(0.01, 0.2), msg->dup());
        EV << "Message forwarded, message ID: " << msg->getSenderAddress() << endl;
    }
}

void RsuHandler::betweennessRequest(Message * msg)
{
    EV << "RSU Algorithm for betweenness centrality called...\n";
    
    // Create request to calculate shortest paths to node
    Message* request = new Message();
    populateWSM(request);

    // TODO: Maybe you dont need the addresses after all...
    request->setSenderAddress(myId);
    request->setSenderPosition(curPosition);
    request->setSource(msg->getSenderAddress());

    std::list<long> path = msg->getPathList();
    path.push_front(myId);

    request->setPathList(path);
    request->setMaxHops(maxHops);
    request->setType(messageType::CENTRALITY_REQUEST);
    request->setCentrality(selectedCentrality::BETWEENNESS);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), request);

    // Broadcast the request of the RSU to other cars
    msg->setSenderPosition(curPosition);
    scheduleAt(simTime() + 0.2 + uniform(0.01, 0.2), msg->dup());

    // After 5s, call collection to create RSU Reply
    Message* collect = new Message();
    populateWSM(collect);

    // TODO: Maybe change to getSource()
    collect->setTarget(msg->getSenderAddress());
    collect->setCentrality(selectedCentrality::BETWEENNESS);
    collect->setState(currentState::COLLECTING);

    scheduleAt(simTime() + 5 + uniform(0.01, 0.2), collect);
}

// ----------- CENTRALITY HANDLING ----------- //

void RsuHandler::handleCentralityRequest(Message * wsm, std::list<Tuple> & ackArray)
{
    EV << "RSU Handling centrality request...\n";

    std::list<long> path = wsm->getPathList();
    std::list<long> rsu = wsm->getRsuList();

    // If your id isn't already in the path list
    if (!inPath(path))
    {
        EV << "Creating shortest path reply, node " << myId << endl;

        Message* reply = new Message();
        populateWSM(reply);

        reply->setSenderAddress(myId);
        reply->setSenderPosition(curPosition);
        reply->setRecipientAddress(*path.begin());
        reply->setTarget(wsm->getSource());
        reply->setSource(wsm->getSenderAddress());
        reply->setMaxHops(maxHops);                         

        path.pop_front();

        reply->setPathList(path);
        reply->setType(messageType::CENTRALITY_REPLY);
        reply->setCentrality(selectedCentrality::BETWEENNESS);
        reply->setRsuList(wsm->getRsuList());

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), reply->dup());
        EV << "Reply sent, message ID: " << reply->getSenderAddress() << endl;

        ackArray.push_front(Tuple(reply));
        reply->setState(currentState::REPEATING);
        scheduleAt(simTime() + 5 + uniform(0.01, 0.2), reply);

        // Add your id to the path list and broadcast it again
        path = wsm->getPathList();
        path.push_front(myId);

        if (!inPath(rsu))
        {
            rsu.push_front(myId);
            wsm->setRsuList(rsu);
        }

        wsm->setPathList(path);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.2 + uniform(0.01, 0.2), wsm->dup());
        EV << "Message forwarded, message ID: " << wsm->getSenderAddress() << endl;
    }
}

void RsuHandler::handleCentralityReply(Message * wsm, std::list<Tuple> & array)
{
    std::list<long> path = wsm->getPathList();

    // If you're the one who made the request, then gather the data and send to the RSU
    if (path.empty())
    {
        EV << "Answer to my request received, node " << myId << endl;
        bool insert = true;

        for (auto i = array.begin(); i != array.end(); i++)
        {
            if (i->id == wsm->getSenderAddress())
            {
                insert = false;

                if (i->hops > wsm->getHops())
                {
                    i->hops = wsm->getHops();
                    i->rsu = wsm->getRsuList();
                }

                break;
            }
        }

        if (insert)
            array.push_front(Tuple(wsm));

        for (auto i = array.begin(); i != array.end(); i++)
            EV << "Node with ID " << i->id << " and distance " << i->hops << endl;

        Message * ack = new Message();
        populateWSM(ack);

        ack->setAckData(wsm->getCreationTime());
        ack->setMaxHops(wsm->getMaxHops());
        ack->setType(messageType::ACKNOWLEDGEMENT);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), ack);
    }

    else
    {
        wsm->setSenderPosition(curPosition);
        wsm->setRecipientAddress(*path.begin());

        path.pop_front();
        
        wsm->setPathList(path);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
}

// ---------------- RSU REPLY ---------------- //

void RsuHandler::handleRsuReply(Message* wsm, std::list<Tuple> & ackArray)
{
    selectedCentrality centrality = wsm->getCentrality();

    switch (centrality)
    {
        case (selectedCentrality::NONE):
            break;
        case (selectedCentrality::DEGREE):
            degreeReply(wsm);
            break;
        case (selectedCentrality::CLOSENESS):
            closenessReply(wsm, distanceList);
            break;
        case (selectedCentrality::BETWEENNESS):
            betweennessReply(wsm);
            break;
    }
}

void RsuHandler::degreeReply(Message* wsm)
{
    if (wsm->getTarget() == myId)
    {
        degree++;

        Message * ack = new Message();
        populateWSM(ack);

        ack->setAckData(wsm->getCreationTime());
        ack->setMaxHops(wsm->getMaxHops());
        ack->setType(messageType::ACKNOWLEDGEMENT);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), ack);
    }

    else 
    {
        EV << "Forwarding RSU reply...\n";
        // Forward the message until it reaches the RSU
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
}

void RsuHandler::closenessReply(Message* wsm, std::list<Tuple> & array)
{
    if (wsm->getTarget() == myId)
    {
        EV << "Answer to my request received, node " << myId << endl;
        
        bool insert = true;

        for (auto i = array.begin(); i != array.end(); i++)
        {
            if (i->id == wsm->getSenderAddress())
            {
                insert = false;

                if (i->hops > wsm->getHops())
                    i->hops = wsm->getHops();

                break;
            }
        }

        if (insert)
            array.push_front(Tuple(wsm));


        for (auto i = array.begin(); i != array.end(); i++)
            EV << "Node with ID " << i->id << " and distance " << i->hops << endl;

        Message * ack = new Message();
        populateWSM(ack);

        ack->setAckData(wsm->getCreationTime());
        ack->setMaxHops(wsm->getMaxHops());
        ack->setType(messageType::ACKNOWLEDGEMENT);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), ack);
    }

    else
    {
        std::list<long> path =  wsm->getPathList();

        for (auto i = path.begin(); i != path.end(); i++)
            EV << "Node in path with id: " << *i << endl;

        wsm->setSenderPosition(curPosition);
        wsm->setRecipientAddress(*path.begin());    
        
        path.pop_front();

        wsm->setPathList(path);  

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
}

void RsuHandler::betweennessReply(Message* wsm)
{
    if (wsm->getTarget() == myId)
    {
        betweenness += wsm->getCentralityData();

        Message * ack = new Message();
        populateWSM(ack);

        ack->setAckData(wsm->getCreationTime());
        ack->setMaxHops(wsm->getMaxHops());
        ack->setType(messageType::ACKNOWLEDGEMENT);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), ack);
    }

    else
    {
        EV << "Forwarding RSU reply...\n";

        // Forward the message until it reaches the RSU
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
}

// ------------- ACKNOWLEDGEMENT ------------- //

void RsuHandler::handleAcknowledgement(Message * wsm, std::list<Tuple> & ackArray)
{
    for (auto i = ackArray.begin(); i != ackArray.end(); i++)
    {
        if ((i->target == wsm->getSenderAddress()) && (i->timestamp == wsm->getAckData()))
        {
            ackArray.erase(i);
            break;
        }
    }
}

// ------------- PATH FUNCTIONS -------------- //

bool RsuHandler::inPath(std::list<long> path)
{
    for (auto i = path.begin(); i != path.end(); i++)
    {
        if (*i == myId)
            return true;
    }

    return false;
}

// ------------ MESSAGE INSERTION ------------ //

bool RsuHandler::receiveMessage(Message * msg, std::list<Tuple> & msgArray, std::list<Tuple> & cndArray)
{
    // If it's for testing purposes, follow this algorithm
    if (msg->getState() == currentState::TESTING)
    {
        EV << "Inserting test messages...\n";

        Message * dummy = new Message();
        populateWSM(dummy);
        
        Tuple test = Tuple(dummy);

        for (int i = 0; i < 10; i++)
        {
            flushList(msgArray, cndArray);

            test.usedFrequency = 1 + i;
            test.lastUsed = 100 - i * 10;
            test.received = 90 - i * 10;
            test.timestamp = (i + 1) * 3;

            msgArray.push_front(test);
            EV << "Message List: \n";
            for (auto i = msgArray.begin(); i != msgArray.end(); i++)
            {
                EV << "Used frequency " << i->usedFrequency;
                EV << " and Last Time Used " << i->lastUsed;
                EV << " and Generated at " << i->timestamp << endl;
            }
        }

        return true;
    }

    // TODO: EXPERIMENTAL
    if (msg->getSenderAddress() == myId)
        return false;

    flushList(msgArray, cndArray);
    return insertMessage(msg, msgArray, cndArray);
}

void RsuHandler::flushList(std::list<Tuple> & msgArray, std::list<Tuple> & cndArray)
{    
    // If cache flushing needs to be done
    if (msgArray.size() + cndArray.size() >= capacity)
    {
        EV << "Executing Flushing Algorithm...\n";

        if (cndArray.empty())
        {
            if (msgArray.size() < flushed)
                msgArray.clear();

            else
            {
                mergeSort(msgArray);
                
                auto end = msgArray.end();
                auto start = end;

                for (int i = 0; i < flushed; i++)
                    start--;
                
                msgArray.erase(start, end);

                mergeSort(msgArray, true);
            }
        }

        else
        {
            if (cndArray.size() < flushed)
                cndArray.clear();

            else
            {
                mergeSort(cndArray);

                auto end = cndArray.end();
                auto start = end;

                for (int i = 0; i < flushed; i--)
                    start--;

                cndArray.erase(start, end);
            }
        }
    }
}

bool RsuHandler::insertMessage(Message * msg, std::list<Tuple> & msgArray, std::list<Tuple> & cndArray)
{
    EV << "Executing Insertion Algorithm...\n";

    if (!msgArray.empty())
    {
        simtime_t timestamp = msg->getCreationTime();

        for (auto i = msgArray.begin(); i != msgArray.end(); i++)
        {
            // Insert message in the correct position
            if (i->timestamp > timestamp)
                continue;
            
            else if (i->timestamp == timestamp)
            {
                if (i->id == msg->getSenderAddress())
                    return false;
            }

            else if (i->timestamp < timestamp)
            {
                msgArray.insert(i, Tuple(msg));
                return true;
            }
        }
    }

    if (!cndArray.empty())
    {
        simtime_t timestamp = msg->getCreationTime();

        for (auto i = cndArray.begin(); i != cndArray.end(); i++)
        {
            if (i->timestamp == timestamp)
            {
                if (i->id == msg->getSenderAddress())
                    return false;
            }
        }
    }

    msgArray.push_front(Tuple(msg));
    return true;
}

// ---------------- THRESHOLD ---------------- //

void RsuHandler::thresholdRequest()
{
    Message * request = new Message();
    populateWSM(request);

    request->setState(currentState::CACHING);
    
    scheduleAt(simTime() + 15, request);
}

void RsuHandler::thresholdControl(simtime_t time, std::list<Tuple> & msgArray, std::list<Tuple> & cndArray)
{
    simtime_t currentTime = simTime();

    EV << "Threshold control being performed...\n";

    for (auto i = --msgArray.end(); i != --msgArray.begin(); i--)
    {
        if (currentTime - i->timestamp > time)
        {
            cndArray.push_front(*i);
            msgArray.erase(i);
        }

        else 
            break;
    }

    EV << "At time: " << currentTime << " Size of messageList: " << msgArray.size() << endl;
    for (auto i = msgArray.begin(); i != msgArray.end(); i++)
        EV << i->timestamp << endl;
    EV << "And size of candidateList: " << cndArray.size() << endl; 
    for (auto i = cndArray.begin(); i != cndArray.end(); i++)
        EV << i->timestamp << endl;

    thresholdRequest();
}

// ------------ SORTING FUNCTIONS ------------ //

void RsuHandler::mergeSort(std::list<Tuple> & array, bool restore)
{
    int midpoint = array.size() / 2;
    if (array.size() == 1)
        return;

    auto i = array.begin();

    std::list<Tuple> left;
    while (left.size() < midpoint)
    {
        left.push_back(*i);
        i++;
    }
        
    std::list<Tuple> right;
    while (right.size() < array.size() - midpoint)
    {
        right.push_back(*i);
        i++;
    }
    
    if (restore)
    {
        mergeSort(left, true);
        mergeSort(right, true);
        mergeRestore(array, left, right);
    }

    else
    {
        mergeSort(left);
        mergeSort(right);
        switch (policy)
        {
            case cachingPolicy::FIFO:
                mergeFIFO(array, left, right);
                break;
            case cachingPolicy::LRU:
                mergeLRU(array, left, right);
                break;
            case cachingPolicy::LFU:
                mergeLFU(array, left, right);
                break;
            default:
                break;
        } 
    }
}

void RsuHandler::mergeRestore(std::list<Tuple> & array, std::list<Tuple> left, std::list<Tuple> right)
{
    auto i = left.begin();
    auto j = right.begin();
    auto k = array.begin();

    while (i != left.end() && j != right.end())
    {
        if (i->timestamp > j->timestamp)
        {
            *k = *i;
            i++;
            k++;
        }
        
        else if (j->timestamp > i->timestamp)
        {            
            *k = *j;
            j++;
            k++;
        }

        else if (i->timestamp == j->timestamp)
        {
            *k = *i;
            i++;
            k++; 
        }
    }

    while (i != left.end())
    {
        *k = *i;
        i++;
        k++;
    }
    
    while (j != right.end())
    {
        *k = *j;
        j++;
        k++;
    }
}

void RsuHandler::mergeFIFO(std::list<Tuple> & array, std::list<Tuple> left, std::list<Tuple> right)
{
    auto i = left.begin();
    auto j = right.begin();
    auto k = array.begin();

    while (i != left.end() && j != right.end())
    {
        if (i->received > j->received)
        {
            *k = *i;
            i++;
            k++;
        }
        
        else if (j->received > i->received)
        {            
            *k = *j;
            j++;
            k++;
        }

        else if (i->received == j->received)
        {
            if (i->timestamp > j->timestamp)
            {
                *k = *i;
                i++;
                k++;  
            }

            else 
            {
                *k = *j;
                j++;
                k++;
            }
        }
    }
    
    while (i != left.end())
    {
        *k = *i;
        i++;
        k++;
    }
    
    while (j != right.end())
    {
        *k = *j;
        j++;
        k++;
    }
}

void RsuHandler::mergeLRU(std::list<Tuple> & array, std::list<Tuple> left, std::list<Tuple> right)
{
    auto i = left.begin();
    auto j = right.begin();
    auto k = array.begin();

    while (i != left.end() && j != right.end())
    {
        if (i->lastUsed > j->lastUsed)
        {
            *k = *i;
            i++;
            k++;
        }
        
        else if (j->lastUsed > i->lastUsed)
        {            
            *k = *j;
            j++;
            k++;
        }

        else if (i->lastUsed == j->lastUsed)
        {
            if (i->timestamp > j->timestamp)
            {
                *k = *i;
                i++;
                k++;  
            }

            else 
            {
                *k = *j;
                j++;
                k++;
            }
        }
    }
    
    while (i != left.end())
    {
        *k = *i;
        i++;
        k++;
    }
    
    while (j != right.end())
    {
        *k = *j;
        j++;
        k++;
    }
}

void RsuHandler::mergeLFU(std::list<Tuple> & array, std::list<Tuple> left, std::list<Tuple> right)
{
    auto i = left.begin();
    auto j = right.begin();
    auto k = array.begin();

    while (i != left.end() && j != right.end())
    {
        if (i->usedFrequency > j->usedFrequency)
        {
            *k = *i;
            i++;
            k++;
        }
        
        else if (j->usedFrequency > i->usedFrequency)
        {            
            *k = *j;
            j++;
            k++;
        }
        
        else if (i->lastUsed == j->lastUsed)
        {
            if (i->timestamp > j->timestamp)
            {
                *k = *i;
                i++;
                k++;  
            }

            else 
            {
                *k = *j;
                j++;
                k++;
            }
        }
    }
    
    while (i != left.end())
    {
        *k = *i;
        i++;
        k++;
    }
    
    while (j != right.end())
    {
        *k = *j;
        j++;
        k++;
    }
}

