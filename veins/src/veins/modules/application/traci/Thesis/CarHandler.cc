#include "veins/modules/application/traci/Thesis/CarHandler.h"
#include "veins/modules/application/traci/Thesis/CarMessage_m.h"

using namespace veins;
using namespace omnetpp;

Define_Module(veins::CarHandler);

void CarHandler::initialize(int stage)
{
    // Initializing variables
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0)
    {
        sentMessage = false;

        currentSubscribedServiceId = -1;
        radioRange = 300;

        lastDroveAt = simTime();

        roadInfo = "";
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
    CarMessage* wsm = check_and_cast<CarMessage*>(frame);
    int distance = int(traci->getDistance(curPosition, wsm->getSenderPosition(), true));

    if (distance < radioRange)
    {
        // Reject the message if distance exceeds signal distance
        
        // Change color, if you received the message
        findHost()->getDisplayString().setTagArg("i", 1, "green");

        // If it's an info request
        if (wsm->getRequest())
        {
            // If you have info, send it and then continue the broadcast
            if (!roadInfo.empty())
            {
                CarMessage* reply = new CarMessage();

                populateWSM(reply);

                reply->setSenderAddress(myId);
                reply->setSenderPosition(curPosition);

                const char* replyInfo = roadInfo.c_str();
                reply->setDemoData(replyInfo);

                reply->setRecipientAddress(wsm->getSenderAddress());

                scheduleAt(simTime() + 1, reply->dup());
            }

            // Then forward message to reach others who might have info
            EV << "Forwarding Message" << "\n";
            // Resend the message after 2s + delay

            wsm->setSenderPosition(curPosition);
            scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
        }

        // Else, it's an info reply
        else 
        {
            roadInfo = wsm->getDemoData();
            EV << roadInfo;

            // Change route if you can, to avoid the obstacle
            /*//TODO: Remove from comment, when actual implementation time
            if (mobility->getRoadId()[0] != ':') 
                traciVehicle->changeRoute(roadInfo, 9999);*/

            if (wsm->getRecipientAddress() != myId)
            {
                EV << "Forwarding Reply Message" << "\n";
                // Resend the message after 2s + delay
                
                wsm->setSenderPosition(curPosition);
                scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
            }

            // If the message was meant for you, accept it
            else 
            {
                EV << "Message received after: " << wsm->getHopCount() << " hops.\n";
            }
        }
    }
}

void CarHandler::handleSelfMsg(cMessage* msg)
{
    if (CarMessage* wsm = dynamic_cast<CarMessage*>(msg)) {
        // Send this message on the service channel until the counter is 3 or higher.
        // This code only runs when channel switching is enabled
        sendDown(wsm->dup());
        wsm->setHopCount(wsm->getHopCount() + 1);

        if (wsm->getHopCount() >= 3) {
            // Stop service advertisements
            stopService();
            delete (wsm);
        }
        else {
            scheduleAt(simTime() + 1, wsm);
        }
    }
    else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void CarHandler::handlePositionUpdate(cObject* obj)
{
    //TODO: CHECK DemoBaseApplLayer
    DemoBaseApplLayer::handlePositionUpdate(obj);

    // If the car is stopped for at least 10s, it has crashed
    if (mobility->getSpeed() < 1)
    {
        if (simTime() - lastDroveAt >= 10 && sentMessage == false)
        {
            // Change car color to red, and start broadcasting
            findHost()->getDisplayString().setTagArg("i", 1, "red");
            sentMessage = true;

            // Create new message
            CarMessage* wsm = new CarMessage();

            populateWSM(wsm);

            wsm->setSenderAddress(myId);
            wsm->setSenderPosition(curPosition);

            wsm->setDemoData(mobility->getRoadId().c_str());

            // The host is standing still due to crash
            if (dataOnSch) {
                startService(Channel::sch2, 42, "Traffic Information Service");
                // started service and server advertising, schedule message to self to send later
                scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
            }
            else {
                // Send right away on CCH, because channel switching is disabled
                sendDown(wsm);
            }
        }
    }

    else
    {
        lastDroveAt = simTime();

        // Random chance of requesting infomation
        int willRequest = uniform(0, 200); 

        if (willRequest == 0)
            requestInfo();
    }
}

// Function for requesting info about network status
void CarHandler::requestInfo()
{
    // Change color of requesting car to purple
    findHost()->getDisplayString().setTagArg("i", 1, "purple");

    CarMessage* wsm = new CarMessage();

    populateWSM(wsm);

    wsm->setRequest(true); 

    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);
    
    scheduleAt(simTime() + 1, wsm->dup());
}


