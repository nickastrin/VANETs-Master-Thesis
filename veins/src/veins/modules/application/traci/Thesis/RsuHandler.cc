#include "veins/modules/application/traci/Thesis/RsuHandler.h"
#include "veins/modules/application/traci/Thesis/CarMessage_m.h"

using namespace veins;
using namespace omnetpp;

Define_Module(veins::RsuHandler);

void RsuHandler::initialize(int stage)
{
    // Initializing variables
    DemoBaseApplLayer::initialize(stage);

    EV << myId << endl;
    EV << curPosition << endl;

    if (stage == 0)
    {
        sentMessage = false;

        currentSubscribedServiceId = -1;
        radioRange = 300;

        roadInfo = "DUMMY INFO";
    }
}

void RsuHandler::onWSA(DemoServiceAdvertisment* wsa)
{
    EV << "WSA\n";
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

void RsuHandler::onWSM(BaseFrame1609_4* frame)
{
    EV << "WSM\n";

    CarMessage* wsm = check_and_cast<CarMessage*>(frame);
    int distance = int(traci->getDistance(curPosition, wsm->getSenderPosition(), true));

    if (distance < radioRange)
    {
        // Reject the message if distance exceeds signal distance
        EV << distance << "\n";
        EV << "Message from host: " << wsm->getSenderAddress() << " arrived to RSU: " << myId << "\n";

        // Change color, if you received the message
        findHost()->getDisplayString().setTagArg("i", 1, "blue");

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

void RsuHandler::handleSelfMsg(cMessage* msg)
{
    EV << "SELF MSG\n";
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


