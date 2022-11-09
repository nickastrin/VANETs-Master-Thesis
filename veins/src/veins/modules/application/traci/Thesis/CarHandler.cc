#include "veins/modules/application/traci/Thesis/CarHandler.h"

using namespace veins;
using namespace omnetpp;

Define_Module(veins::CarHandler);

void CarHandler::initialize(int stage)
{
    // Initializing variables
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0)
    { 
        // TODO: Check what this variable actually does
        sentMessage = false;

        currentSubscribedServiceId = -1;
        radioRange = 300;

        lastDroveAt = simTime();
    }
}

void CarHandler::onWSA(DemoServiceAdvertisment* wsa)
{
    // If no assigned service, assign one
    if (currentSubscribedServiceId == -1)
    {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));

        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void CarHandler::onWSM(BaseFrame1609_4* frame)
{
    Message* wsm = check_and_cast<Message*>(frame);
    int distance = int(traci->getDistance(curPosition, wsm->getSenderPosition(), false));

    //Reject message if distance exceeds signal range
    if (distance < radioRange)
    {
        if (acceptMessage(wsm))
        {
            // Pring message list, for debugging purposes
            EV << "MESSAGE ACCEPTED BY NODE " << myId << endl;

            std::list<Tuple>::iterator i;
            for (i = messageList.begin(); i != messageList.end(); i++)
                EV << "MESSAGE ID IS " << i->id << endl;

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
}

void CarHandler::handleSelfMsg(cMessage* msg)
{
    // Send this message on the service channel until the counter is 3 or higher.
    // This code only runs when channel switching is enabled
    if (Message* wsm = dynamic_cast<Message*>(msg))
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
    }

    else
        DemoBaseApplLayer::handleSelfMsg(msg);
}

void CarHandler::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

    // If the cas has stopped for at least 10s, it has crashed
    if (mobility->getSpeed() < 1)
    {
        if (simTime() - lastDroveAt >= 10 && sentMessage == false)
        {
           // Change car color to red, and start broadcasting
            findHost()->getDisplayString().setTagArg("i", 1, "red");
            sentMessage = true;

            // Create new message
            Message* wsm = new Message();

            populateWSM(wsm);

            wsm->setSenderAddress(myId);
            wsm->setSenderPosition(curPosition);

            wsm->setMessageData(mobility->getRoadId().c_str());

            // The host is standing still due to crash
            if (dataOnSch) 
            {
                startService(Channel::sch2, 42, "Traffic Information Service");

                // started service and server advertising, schedule message to self to send later
                scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
            }
            else 
            {
                // Send right away on CCH, because channel switching is disabled
                sendDown(wsm);
            }
        }
    }

    else 
    {
        lastDroveAt = simTime();

        // Random chance of requesting infomation
        /*
        int willRequest = uniform(0, 400); 

        if (willRequest == 0)
            requestInfo();
        
        // Alternate condition, for debugging
        if (lastDroveAt == 85 && myId == 58)
                requestInfo();
        */
    }
}

// Functions for handling different types of messages
void CarHandler::handleBroadcast(Message* wsm)
{
    //TODO: Fix some parameters

    //Resend the message after 2s + delay
    wsm->setSenderPosition(curPosition);
    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
}

void CarHandler::handleRequest(Message* wsm)
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

void CarHandler::handleReply(Message* wsm)
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

void CarHandler::handleRsuCheck(Message* wsm)
{
    EV << wsm->getCentrality() << endl;
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

void CarHandler::handleRsuReply(Message* wsm)
{
    centralityType centrality = wsm->getCentrality();

    switch (centrality)
    {
        case (centralityType::NONE):
            break;
        case (centralityType::DEGREE):
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
void CarHandler::degreeCentrality(Message* wsm)
{
    Message* reply = new Message();

    populateWSM(reply);

    reply->setSenderAddress(myId);
    reply->setSenderPosition(curPosition);
    reply->setRecipientAddress(wsm->getSenderAddress());

    reply->setType(messageType::RSU_REPLY);
    reply->setCentrality(centralityType::DEGREE);

    scheduleAt(simTime() + 1 + uniform(0.01, 0.2), reply);
}

void CarHandler::closenessCentrality(Message* wsm)
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

void CarHandler::betweennessCentrality(Message* wsm)
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
void CarHandler::handleCloseness(Message* wsm)
{
    std::list<LAddress::L2Type> searchList =  wsm->getSearchFront();

    wsm->setSenderPosition(curPosition);
    wsm->setRecipientAddress(*searchList.begin());

    searchList.pop_front();
    wsm->setSearchFront(searchList);    

    // TODO: Check the dup method
    scheduleAt(simTime() + 0.1, wsm->dup());
}

void CarHandler::handleBetweenness(Message* wsm)
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
        scheduleAt(simTime() + 1 + uniform(0.01, 0.2), wsm->dup());   
    }
}

// Function for requesting info about network status
void CarHandler::requestInfo()
{
    // Change color of requesting car to purple
    findHost()->getDisplayString().setTagArg("i", 1, "purple");
    sentMessage = true;

    Message* wsm = new Message();

    populateWSM(wsm);

    wsm->setType(messageType::REQUEST);

    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);
    
    messageList.insert(messageList.begin(), Tuple(wsm));
    scheduleAt(simTime() + 1, wsm);
}

// Misc auxilary functions
bool CarHandler::acceptMessage(Message* wsm)
{
    EV << "NODE WITH ID " << myId << " RECEIVED MSG " << wsm->getSenderAddress() << endl;
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

bool CarHandler::isDuplicate(Message* wsm)
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

