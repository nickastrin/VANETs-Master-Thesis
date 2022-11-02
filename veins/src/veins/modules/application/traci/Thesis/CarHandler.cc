#include "veins/modules/application/traci/Thesis/CarHandler.h"
#include "veins/modules/application/traci/Thesis/Message_m.h"

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
    Message* wsm = check_and_cast<Message*>(frame);
    int distance = int(traci->getDistance(curPosition, wsm->getSenderPosition(), false));

    //Reject message if distance exceeds signal range
    if (distance < radioRange)
    {
        // Change color if you accepted the message
        findHost()->getDisplayString().setTagArg("i", 1, "green");

        messageType type = wsm->getType();

        switch (type)
        {
            // If it's a broadcast
            case messageType::BROADCAST:
                roadInfo = wsm->getMessageData();

                //Resend the message after 2s + delay
                wsm->setSenderPosition(curPosition);
                scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());

                break;

            // If it's an info request
            case messageType::REQUEST:
                // If you have info, send it and continue the broadcast
                if (!roadInfo.empty())
                {
                    Message* reply = new Message();

                    populateWSM(reply);

                    reply->setSenderAddress(myId);
                    reply->setSenderPosition(curPosition);
                    reply->setRecipientAddress(wsm->getSenderAddress());

                    reply->setType(messageType::REPLY);

                    const char* replyInfo = roadInfo.c_str();
                    reply->setMessageData(replyInfo);

                    scheduleAt(simTime() + 1, reply);
                }

                // Then forward the message to reach others who might have info
                wsm->setSenderPosition(curPosition);
                scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());

                break;

            // If it's an info reply
            case messageType::REPLY:
                roadInfo = wsm->getMessageData();

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
               {
                   EV << "Message received after: " << wsm->getHopCount() + 1 << " hops.\n";
               }

               break;
            
            // If it's an RSU Check
            case messageType::RSU_CHECK:
                break;
                
            default:
                break;
        }
    }
}

void CarHandler::handleSelfMsg(cMessage* msg)
{
    // Send this message on the service channel until the counter is 3 or higher.
    // This code only runs when channel switching is enabled
    if (Message* wsm = dynamic_cast<Message*>(msg))
    {
        sendDown(wsm->dup());
        wsm->setHopCount(wsm->getHopCount() + 1);

        if (wsm->getHopCount() >= 3)
        {
            // Stop service advertisements
            stopService();
            delete(wsm);
        }
        
        else
            scheduleAt(simTime() + 1, wsm);
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
            */
        if (lastDroveAt == 85 && myId == 88)
                requestInfo();
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
    
    scheduleAt(simTime() + 1, wsm);
}
