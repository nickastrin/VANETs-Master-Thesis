#include "veins/modules/application/traci/Thesis/RsuHandler.h"

using namespace veins;
using namespace omnetpp;

Define_Module(veins::RsuHandler);

// ---------------------------------------------------------------------- //

void RsuHandler::initialize(int stage)
{
    // Initializing variables
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0)
    {
        // TODO: Change road info, redundant. Should just send message to self
        radioRange = 300;
        roadInfo = "DUMMY INFO";

        degree = 0;
        closeness = 0;
        betweenness = 0;

        Message* request = new Message();
        populateWSM(request);

        request->setSenderAddress(myId);
        request->setState(procedureState::INITIALIZING);
        request->setCentrality(centralityType::BETWEENNESS);

        scheduleAt(simTime() + 10, request);
    }
}

void RsuHandler::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) 
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
}

void RsuHandler::onWSM(BaseFrame1609_4* frame)
{
    Message* wsm = check_and_cast<Message*>(frame);
    if (acceptMessage(wsm))
    {
        // Pring message list, for debugging purposes
        EV << "Message accepted by the RSU... " << myId << endl;

        std::list<Tuple>::iterator i;
        for (i = messageList.begin(); i != messageList.end(); i++)
            EV << "Message ID is " << i->id << " and generation time is " << i->timestamp << endl;

        // Change color if you accepted the message
        findHost()->getDisplayString().setTagArg("i", 1, "black");

        messageType type = wsm->getType();

        switch (type)
        {
            case messageType::BROADCAST:
                handleBroadcast(wsm);
                break;
            case messageType::REQUEST:
                handleRequest(wsm);
                break;
            case messageType::REPLY:
                handleReply(wsm);
                break;
            case messageType::RSU_REQUEST:
                handleRsuRequest(wsm);
                break;
            case messageType::RSU_REPLY:
                handleRsuReply(wsm);
                break;
            case messageType::CENTRALITY_REQUEST:
                handleCentralityRequest(wsm);
                break;
            case messageType::CENTRALITY_REPLY:
                handleCentralityReply(wsm);
                break;
            default:
                break;
            
        }
    }
}

// ---------------------------------------------------------------------- //

//TODO : Check this
void RsuHandler::handleSelfMsg(cMessage* msg)
{
    // Send this message on the service channel until the counter is 3 or higher.
    // This code only runs when channel switching is enabled
    if (Message* wsm = dynamic_cast<Message*>(msg))
    {
        procedureState stage = wsm->getState();

        switch (stage)
        {
            case procedureState::INITIALIZING:
                initializingMessage(wsm);
                break;
            case procedureState::SENDING:
                sendingMessage(wsm);
                break;
            case procedureState::COLLECTING:   
                collectingMessage(wsm);
                break;
            default:
                break;
        }
    }

    else
        DemoBaseApplLayer::handleSelfMsg(msg);
}

void RsuHandler::initializingMessage(Message* wsm)
{
    EV << "Initilializing centrality calculations...\n"; 

    Message* request = new Message();
    populateWSM(request);

    request->setSenderAddress(myId);
    request->setSenderPosition(curPosition);
    request->setSource(myId);
    request->setType(messageType::RSU_REQUEST);

    // Create request depending on centrality requested
    switch(wsm->getCentrality())
    {
        case (centralityType::NONE):
            break;
        case (centralityType::DEGREE):
            request->setCentrality(centralityType::DEGREE);
            break;
        case (centralityType::CLOSENESS):
        {
            request->setCentrality(centralityType::CLOSENESS);
            
            std::list<LAddress::L2Type> path;
            path.insert(path.begin(), myId);

            request->setPathList(path);
            request->setMaxHops(20);

            break;
        }
        case (centralityType::BETWEENNESS):
        {
            request->setCentrality(centralityType::BETWEENNESS);
            break;
        }
        default:
            break;
    }

    scheduleAt(simTime() + 65, request);

    // Call collection after 10s
    Message* collect= new Message();
    populateWSM(collect);

    collect->setTarget(myId);
    collect->setCentrality(wsm->getCentrality());
    collect->setState(procedureState::COLLECTING);

    scheduleAt(simTime() + 75, collect);

}

void RsuHandler::sendingMessage(Message* wsm)
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

void RsuHandler::collectingMessage(Message* wsm)
{
    centralityType centrality = wsm->getCentrality();
    float result = 0;

    if (centrality == centralityType::DEGREE)
    {
        result = degree;

        EV << "Degree Centrality is " << result << endl;
    }

    else if (centrality == centralityType::CLOSENESS)
    {
        std::list<Tuple>::iterator i;

        for (i = distanceList.begin(); i != distanceList.end(); i++)
            result += i->hops;

        result = float(result) / float(distanceList.size());

        EV << "Closeness Centrality is " << result << endl;
    }

    else if (centrality == centralityType::BETWEENNESS)
    {
        if (wsm->getTarget() == myId)
        {
            result = betweenness;

            EV << "Betweenness Centrality is " << result << endl;
        }

        else
        {
            Message* reply = new Message();
            populateWSM(reply);

            reply->setSenderAddress(myId);
            reply->setSenderPosition(curPosition);
            reply->setTarget(wsm->getTarget());
            reply->setMaxHops(20);
            reply->setType(messageType::RSU_REPLY);
            reply->setCentrality(centralityType::BETWEENNESS);

            std::list<Tuple>::iterator i;
            int counter = 0;

            // TODO: Fix this to handle multiple RSUs
            for (i = distanceList.begin(); i != distanceList.end(); i++)
            {
                if (!i->rsu.empty())
                {
                    std::list<long>::iterator j;
                    
                    for (j = i->rsu.begin(); j != i->rsu.end(); j++)
                    {
                        if (*j == wsm->getTarget())
                        {
                            reply->setCentralityData(reply->getCentralityData() + 1);
                            break;
                        }
                    }
                }
            }

            scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), reply);
        }
    }
}

// ---------------------------------------------------------------------- //

// Functions for handling different types of messages
void RsuHandler::handleBroadcast(Message* wsm)
{
    //Resend the message after 2s + delay
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
}

void RsuHandler::handleRequest(Message* wsm)
{    
    // If you have info, send it and continue the broadcast

    // Variables for the message list
    std::list<Tuple>::iterator i;
    float delay = 1.0;

    // Create a reply for each one of your info messages
    for (i = messageList.begin(); i != messageList.end(); i++)
    {
        // Change to roadData
        if (!i->roadData.empty())
        {
            Message* reply = new Message();

            populateWSM(reply);   

            reply->setSenderAddress(myId);
            reply->setSenderPosition(curPosition);
            reply->setTarget(wsm->getSenderAddress());
            reply->setType(messageType::REPLY);
            reply->setRoadData(i->roadData.c_str());

            delay += 0.1;
            
            messageList.push_front(Tuple(reply));
            scheduleAt(simTime() + delay + uniform(0.01, 0.2), reply);
        }
    }

    // Then forward the message to reach others who might have info
    wsm->setSenderPosition(curPosition);
    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
}

void RsuHandler::handleReply(Message* wsm)
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
    }
}

// ---------------------------------------------------------------------- //

void RsuHandler::handleRsuRequest(Message* wsm)
{
    // TODO: Implementation for different centralities, based on Message Info
    centralityType centrality = wsm->getCentrality();

    switch(centrality)
    {
        case (centralityType::NONE):
            break;
        case (centralityType::DEGREE):
            degreeRequest(wsm);
            break;
        case (centralityType::CLOSENESS):
            closenessRequest(wsm);
            break;
        case(centralityType::BETWEENNESS):
            betweennessRequest(wsm);
            break;
        default:
             break;
    }
}

// Functions for handling different centrality calculations
void RsuHandler::degreeRequest(Message* wsm)
{
    EV << "RSU Algorithm for degree centrality called..." << endl;

    Message* reply = new Message();

    populateWSM(reply);

    reply->setSenderAddress(myId);
    reply->setSenderPosition(curPosition);
    reply->setRecipientAddress(wsm->getSenderAddress());
    reply->setType(messageType::RSU_REPLY);
    reply->setCentrality(centralityType::DEGREE);

    messageList.push_front(Tuple(reply));
    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), reply);
}

void RsuHandler::closenessRequest(Message* wsm)
{
    EV << "RSU Algorithm for closeness centrality called...\n";
    std::list<long> path = wsm->getPathList();

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
        reply->setMaxHops(20);                          // Changing the no. of hops, because it has to go through the entire graph
        
        path.pop_front();

        reply->setPathList(path);
        reply->setType(messageType::RSU_REPLY);
        reply->setCentrality(centralityType::CLOSENESS);

        messageList.push_front(Tuple(reply));
        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), reply);
        EV << "Reply sent, message ID: " << reply->getSenderAddress() << endl;

        // Add your id to the path list and broadcast it again
        path = wsm->getPathList();
        path.push_front(myId);

        wsm->setPathList(path);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.2 + uniform(0.01, 0.2), wsm->dup());
        EV << "Message forwarded, message ID: " << wsm->getSenderAddress() << endl;
    }
}

void RsuHandler::betweennessRequest(Message* wsm)
{
    EV << "RSU Algorithm for betweenness centrality called...\n";
    
    // Create request to calculate shortest paths to node
    Message* request = new Message();
    populateWSM(request);

    // TODO: Maybe you dont need the addresses after all...
    request->setSenderAddress(myId);
    request->setSenderPosition(curPosition);
    request->setSource(wsm->getSenderAddress());

    std::list<long> path = wsm->getPathList();
    path.push_front(myId);

    request->setPathList(path);
    request->setMaxHops(20);
    request->setType(messageType::CENTRALITY_REQUEST);
    request->setCentrality(centralityType::BETWEENNESS);

    messageList.push_front(Tuple(request));
    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), request);

    // Broadcast the request of the RSU to other cars
    wsm->setSenderPosition(curPosition);
    scheduleAt(simTime() + 0.2 + uniform(0.01, 0.2), wsm->dup());

    // After 5s, call collection to create RSU Reply
    Message* collect = new Message();
    populateWSM(collect);

    // TODO: Maybe change to getSource()
    collect->setTarget(wsm->getSenderAddress());
    collect->setCentrality(centralityType::BETWEENNESS);
    collect->setState(procedureState::COLLECTING);

    scheduleAt(simTime() + 5 + uniform(0.01, 0.2), collect);
}

// ---------------------------------------------------------------------- //

void RsuHandler::handleCentralityRequest(Message* wsm)
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
        reply->setMaxHops(20);                          // Changing the no. of hops, because it has to go through the entire graph
        
        path.pop_front();

        reply->setPathList(path);
        reply->setType(messageType::CENTRALITY_REPLY);
        reply->setCentrality(centralityType::BETWEENNESS);
        reply->setRsuList(wsm->getRsuList());

        messageList.push_front(Tuple(reply));
        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), reply);
        EV << "Reply sent, message ID: " << reply->getSenderAddress() << endl;

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

void RsuHandler::handleCentralityReply(Message* wsm)
{
    std::list<long> path = wsm->getPathList();

    // If you're the one who made the request, then gather the data and send to the RSU
    if (path.empty())
    {
        EV << "Answer to my request received, node " << myId << endl;
        std::list<Tuple>::iterator i;
        bool insert = true;

        for (i = distanceList.begin(); i != distanceList.end(); i++)
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
            distanceList.push_front(Tuple(wsm));


        for (i = distanceList.begin(); i != distanceList.end(); i++)
            EV << "Node with ID " << i->id << " and distance " << i->hops << endl;

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

// ---------------------------------------------------------------------- //

void RsuHandler::handleRsuReply(Message* wsm)
{
    centralityType centrality = wsm->getCentrality();

    switch (centrality)
    {
        case (centralityType::NONE):
            break;
        case (centralityType::DEGREE):
            degreeReply(wsm);
            break;
        case (centralityType::CLOSENESS):
            closenessReply(wsm);
            break;
        case (centralityType::BETWEENNESS):
            betweennessReply(wsm);
            break;
    }
}

// Functions for handling replies to centrality requests
void RsuHandler::degreeReply(Message* wsm)
{
    if (wsm->getTarget() == myId)
        degree++;

    else 
    {
        EV << "Forwarding RSU reply...\n";

        // Forward the message until it reaches the RSU
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
}

void RsuHandler::closenessReply(Message* wsm)
{
    if (wsm->getTarget() == myId)
    {
        EV << "Answer to my request received, node " << myId << endl;
        
        std::list<Tuple>::iterator i;
        bool insert = true;

        for (i = distanceList.begin(); i != distanceList.end(); i++)
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
            distanceList.push_front(Tuple(wsm));


        for (i = distanceList.begin(); i != distanceList.end(); i++)
            EV << "Node with ID " << i->id << " and distance " << i->hops << endl;
    }

    else
    {
        std::list<long> path =  wsm->getPathList();
        std::list<long>::iterator i;

        for (i = path.begin(); i != path.end(); i++)
            EV << "Node in path with id: " << *i << endl;

        wsm->setSenderPosition(curPosition);
        wsm->setRecipientAddress(*path.begin());    
        
        path.pop_front();

        wsm->setPathList(path);  

        // TODO: Check the dup method
        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
}

void RsuHandler::betweennessReply(Message* wsm)
{
    if (wsm->getTarget() == myId)
        betweenness += wsm->getCentralityData();

    else
    {
        EV << "Forwarding RSU reply...\n";

        // Forward the message until it reaches the RSU
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
}

// ---------------------------------------------------------------------- //

/*
void RsuHandler::shortestPaths(Message* wsm)
{    
    std::list<long> path = wsm->getPathList();
    std::list<long> rsu = wsm->getRsuList();

    // If your id isn't already in the path list
    if (!inPath(path))
    {
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
*/

bool RsuHandler::acceptMessage(Message* wsm)
{
    EV << "RSU with ID " << myId << " received message from " << wsm->getSenderAddress() << endl;

    if (!messageList.empty())
    {
        std::list<Tuple>::iterator i;
        for (i = messageList.begin(); i != messageList.end(); i++)
        {
            // Depending on the timestamp of the message, put it in the correct position
            if (i->timestamp > wsm->getCreationTime())
                continue;

            if (i->timestamp == wsm->getCreationTime())
                if (i->id == wsm->getSenderAddress())
                    return false;

            if (i->timestamp < wsm->getCreationTime())
            {
                messageList.insert(i--, Tuple(wsm));
                return true;
            }
        }
    }

    messageList.push_front(Tuple(wsm));
    return true;
}

bool RsuHandler::inPath(std::list<long> path)
{
    EV << "I'm here, RSU with ID " << myId << endl;

    std::list<long>::iterator i;

    for (i = path.begin(); i != path.end(); i++)
    {
        if (*i == myId)
            return true;
    }

    return false;
}

// ---------------------------------------------------------------------- //

