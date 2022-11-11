#include "veins/modules/application/traci/Thesis/RsuHandler.h"

using namespace veins;
using namespace omnetpp;

Define_Module(veins::RsuHandler);

void RsuHandler::initialize(int stage)
{
    // Initializing variables
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0)
    {
        radioRange = 300;
        roadInfo = "DUMMY INFO";

        Message* request = new Message();

        populateWSM(request);
        request->setState(procedureState::INITIALIZING);
        request->setCentrality(centralityType::CLOSENESS);

        scheduleAt(simTime() + 10, request);
    }
}

void RsuHandler::onWSA(DemoServiceAdvertisment* wsa)
{
    // if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}

void RsuHandler::onWSM(BaseFrame1609_4* frame)
{
    Message* wsm = check_and_cast<Message*>(frame);
    if (acceptMessage(wsm))
    {
        // Pring message list, for debugging purposes
        EV << "MESSAGE ACCEPTED BY RSU " << myId << endl;

        std::list<Tuple>::iterator i;
        for (i = messageList.begin(); i != messageList.end(); i++)
        {
            EV << "MESSAGE ID IS " << i->source << " AND GENERATION TIME IS " << i->timestamp << endl;
        }

        // Change color if you accepted the message
        findHost()->getDisplayString().setTagArg("i", 1, "green");

        messageType type = wsm->getType();

        switch (type)
        {
            // If it's a broadcast
            case messageType::BROADCAST:
                handleBroadcast(wsm);
                break;

            // If it's an info request
            case messageType::REQUEST:
                handleRequest(wsm);
                break;

            // If it's an info reply
            case messageType::REPLY:
                handleReply(wsm);
                break;
                
            // If it's an RSU Check
            case messageType::RSU_CHECK:
                handleRsuCheck(wsm);
                break;

            case messageType::RSU_REPLY:
                handleRsuReply(wsm);
                break;
                    
            default:
                break;
            
        }
    }
}

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
                requestCentrality(wsm->getCentrality());
                break;

            case procedureState::SENDING:
            {
                if (wsm->getHopCount() < wsm->getMaxHops())
                {
                    wsm->setHopCount(wsm->getHopCount() + 1);
                    sendDown(wsm->dup());
                }

                else
                {
                    // Stop service advertisements
                    EV << "NODE WITH ID " << myId << " DELETING MESSAGE WITH ID " << wsm->getSenderAddress() << endl;
                    stopService();
                    delete(wsm);
                }

                break;
            }

            case procedureState::COLLECTING:      
            {      
                int counter = 0;
            
                collectMessages(counter);
            
                EV << "RSU CLOSENESS CENTRALITY IS " << calculateCloseness(counter) << endl;
                break;
            }

            default:
                break;
        }
    }

    else
        DemoBaseApplLayer::handleSelfMsg(msg);
}

// Functions for handling different types of messages
void RsuHandler::handleBroadcast(Message* wsm)
{
    //TODO: Fix some parameters

    //Resend the message after 2s + delay
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);
    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
}

void RsuHandler::handleRequest(Message* wsm)
{
    // If you have info, send it and continue the broadcast
    EV << "SIZE OF MESSAGELIST " << messageList.size() << endl;

    // Iterators for the message list
    std::list<Tuple>::iterator i;
    int spacing = 0;

    // Create a reply for each one of your info messages
    for (i = messageList.begin(); i != messageList.end(); i++)
    {
        if (!i->data.empty())
        {
            EV << "GENERATING REPLY NO. " << spacing + 1 << endl;
            Message* reply = new Message();

            populateWSM(reply);   

            reply->setSenderAddress(myId);
            reply->setSenderPosition(curPosition);
            reply->setRecipientAddress(wsm->getSenderAddress());

            reply->setType(messageType::REPLY);

            reply->setMessageData(i->data.c_str());

            spacing++;
            scheduleAt(simTime() + spacing, reply);
        }
    }

    // Then forward the message to reach others who might have info
    wsm->setSenderPosition(curPosition);
    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
}

void RsuHandler::handleReply(Message* wsm)
{
    // TODO: Remove from commment, when it's actual implementation time
    //Change the route if you can, to avoid the obstacle
    /*
    if (mobility->getRoadId()[0] != ':')
        traciVehicle->changeRoute(roadInfo, 9999);
    */

    // If the message was not meant for you, forward
    if (wsm->getRecipientAddress() != myId)
    {
        //Resend the message after 2s + delay
        wsm->setSenderPosition(curPosition);
        scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
    }

    // If the message was meant for you, accept
    else 
        EV << "Message received after: " << wsm->getHopCount() + 1 << " hops.\n";
}

void RsuHandler::handleRsuCheck(Message* wsm)
{
    // TODO: Implementation for different centralities, based on Message Info
    centralityType centrality = wsm->getCentrality();

    switch(centrality)
    {
        case (centralityType::NONE):
            break;
        case (centralityType::DEGREE):
            degreeCentrality(wsm);
            break;
        case (centralityType::CLOSENESS):
            closenessCentrality(wsm);
            break;
        case(centralityType::BETWEENNESS):
            betweennessCentrality(wsm);
            break;
        default:
             break;
    }
}

void RsuHandler::handleRsuReply(Message* wsm)
{
    centralityType centrality = wsm->getCentrality();

    switch (centrality)
    {
        case (centralityType::NONE):
            break;
        case (centralityType::DEGREE):
            handleDegree(wsm);
            break;
        case (centralityType::CLOSENESS):
            handleCloseness(wsm);
            break;
        case (centralityType::BETWEENNESS):
            handleBetweenness(wsm);
            break;
    }
}

// Functions for handling different centrality calculations
void RsuHandler::degreeCentrality(Message* wsm)
{
    Message* reply = new Message();

    populateWSM(reply);

    reply->setSenderAddress(myId);
    reply->setSenderPosition(curPosition);
    reply->setRecipientAddress(wsm->getSenderAddress());

    reply->setType(messageType::RSU_REPLY);
    reply->setCentrality(centralityType::DEGREE);

    scheduleAt(simTime() + 0.1, reply);
}

void RsuHandler::closenessCentrality(Message* wsm)
{
    // If your id isn't already in the path list
    if (!isDuplicate(wsm))
    {
        // Create a reply to give the RSU the path distance
        std::list<LAddress::L2Type> searchList =  wsm->getSearchFront();

        Message* reply = new Message();

        populateWSM(reply);

        reply->setSenderAddress(myId);
        reply->setSenderPosition(curPosition);
        reply->setRecipientAddress(*searchList.begin());
        
        searchList.pop_front();
        reply->setSearchFront(searchList);

        reply->setType(messageType::RSU_REPLY);
        reply->setCentrality(centralityType::CLOSENESS);
        
        scheduleAt(simTime() + 0.1, reply);

        // Add your id to the path list and broadcast it again
        searchList = wsm->getSearchFront();
        searchList.insert(searchList.end(), myId);

        wsm->setSearchFront(searchList);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.2, wsm->dup());
    }
}

void RsuHandler::betweennessCentrality(Message* wsm)
{
    // If your id isn't already in the path list
    if (!isDuplicate(wsm))
    {
        std::list<LAddress::L2Type> searchList =  wsm->getSearchFront();

        // If you only have the RSU in the list, just forward the request
        if (searchList.size() == 1)
        {
            searchList.insert(searchList.end(), myId);

            wsm->setSearchFront(searchList);
            wsm->setSenderAddress(myId);
            wsm->setSenderPosition(curPosition);

            scheduleAt(simTime() + 0.2, wsm->dup());
        }

        // Else create a reply and forward the original message
        else
        {
            // The aforementioned reply is created here
            Message* reply = new Message();

            populateWSM(reply);

            reply->setSenderAddress(myId);
            reply->setSenderPosition(curPosition);
            reply->setRecipientAddress(*searchList.begin());
            
            searchList.pop_front();
            reply->setSearchFront(searchList);

            reply->setType(messageType::RSU_REPLY);
            reply->setCentrality(centralityType::BETWEENNESS);
            
            scheduleAt(simTime() + 0.1, reply);

            // And here, the message is being forwarded 
            searchList = wsm->getSearchFront();
            searchList.insert(searchList.end(), myId);

            wsm->setSearchFront(searchList);
            wsm->setSenderPosition(curPosition);

            scheduleAt(simTime() + 0.2, wsm->dup());
        }
    }
}

// Functions for handling replies to centrality requests
void RsuHandler::handleDegree(Message* wsm)
{
    return;
}

void RsuHandler::handleCloseness(Message* wsm)
{
    std::list<Tuple>::iterator i;

    for (i = messageList.begin(); i != messageList.end(); i++)
    {
        if (i->type == messageType::RSU_REPLY)
        {
            EV << "SOURCE: " << i->source << "AND HOPS: " << i->hops << endl;
        }
    }
}

void RsuHandler::handleBetweenness(Message* wsm)
{
    std::list<LAddress::L2Type> searchList =  wsm->getSearchFront();

    // TODO: Maybe add check for multiple RSUs asking for centralities
    if (searchList.size() == 1)
        return;
    
    else 
    {
        wsm->setSenderPosition(curPosition);
        wsm->setRecipientAddress(*searchList.begin());

        searchList.pop_front();
        wsm->setSearchFront(searchList);    

        // TODO: Check the dup method
        scheduleAt(0.1, wsm->dup());   
    }
}

// Misc auxilary functions
bool RsuHandler::acceptMessage(Message* wsm)
{
    EV << "RSU WITH ID " << myId << " RECEIVED MSG " << wsm->getSenderAddress() << endl;
    if (!messageList.empty())
    {
        std::list<Tuple>::iterator i;
        for (i = messageList.begin(); i != messageList.end(); i++)
        {
            if (i->timestamp > wsm->getCreationTime())
                continue;

            if (i->timestamp == wsm->getCreationTime())
            {
                if (i->id == wsm->getSenderAddress())
                    return false;
            }

            if (i->timestamp < wsm->getCreationTime())
            {
                messageList.insert(i--, Tuple(wsm));
                return true;
            }
        }
    }

    else 
    {
        messageList.insert(messageList.begin(), Tuple(wsm));
        return true;
    }
}

bool RsuHandler::isDuplicate(Message* wsm)
{
    std::list<LAddress::L2Type> searchList =  wsm->getSearchFront();
    std::list<LAddress::L2Type>::iterator i;

    for (i = searchList.begin(); i != searchList.end(); i++)
    {
        if (*i == myId)
            return true;
    }

    return false;
}

void RsuHandler::requestCentrality(centralityType centrality)
{
    Message* request = new Message();

    populateWSM(request);
    request->setSenderAddress(myId);
    request->setSenderPosition(curPosition);

    EV << "RSU CURRENT POSITION IS " << curPosition << endl;

    request->setType(messageType::RSU_CHECK);

    switch(centrality)
    {
        case (centralityType::NONE):
            break;
        case (centralityType::DEGREE):
            request->setCentrality(centralityType::DEGREE);
            break;
        case (centralityType::CLOSENESS):
        {
            request->setCentrality(centralityType::CLOSENESS);
            
            std::list<LAddress::L2Type> searchList;
            searchList.insert(searchList.begin(), myId);
            request->setSearchFront(searchList);
            request->setMaxHops(10);
            break;
        }
        case (centralityType::BETWEENNESS):
            request->setCentrality(centralityType::BETWEENNESS);
            break;
        default:
            break;
    }

    scheduleAt(simTime() + 65, request->dup());
    delete(request);

    Message* result = new Message();

    populateWSM(result);
    result->setState(procedureState::COLLECTING);

    scheduleAt(simTime() + 75, result);

}

// TODO: Name subject to change
void RsuHandler::collectMessages(int &counter)
{
    std::list<Tuple>::iterator i;

    for (i = messageList.begin(); i != messageList.end(); i++)
    {
        if (i->type == messageType::RSU_REPLY)
        {
            bool insert = true;

            if (!collectionList.empty())
            {                
                // TODO: Maybe create different struct for this list, with less data
                // TODO: The data required are: source, hops (and centralityType ?)

                std::list<Tuple>::iterator j;
                
                for (j = collectionList.begin(); j != collectionList.end(); j++)
                {
                    if (i->source == j->source)
                    {
                        insert = false;

                        if (j->hops > i->hops)
                        {
                            counter -= j->hops;
                            counter += i->hops;

                            j->hops = i->hops;

                            EV << "COUNTER IS NOW: " << counter << endl;
                        }

                        break;
                    }
                }
            }

            if (insert)
            {
                collectionList.insert(collectionList.begin(), *i);
                counter += i->hops;  
                
                EV << "COUNTER IS NOW: " << counter << endl;
            }
        }
    }
}

double RsuHandler::calculateCloseness(int counter)
{   
    EV << "SIZE OF LIST IS " << collectionList.size() << endl;
    double result = double(counter) / double(collectionList.size());

    collectionList.clear();

    return result;
}

