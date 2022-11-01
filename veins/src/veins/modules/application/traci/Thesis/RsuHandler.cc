#include "veins/modules/application/traci/Thesis/RsuHandler.h"
#include "veins/modules/application/traci/Thesis/Message_m.h"

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
    EV << "I'm here, printing from the RSU\n";
    EV << curPosition;
    Message* wsm = check_and_cast<Message*>(frame);

    findHost()->getDisplayString().setTagArg("i", 1, "black");

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

                scheduleAt(simTime() + 1, reply->dup());
            }

            // Then forward the message to reach others who might have info
            wsm->setSenderPosition(curPosition);
            scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());

            break;

        // If it's an info reply
        case messageType::REPLY:
            roadInfo = wsm->getMessageData();

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
               EV << "Message received after: " << wsm->getHopCount() << " hops.\n";
               delete(wsm);
           }

           break;
        
        // If it's an RSU Check
        case messageType::RSU_CHECK:
            break;
            
        default:
            break;
    }
}

void RsuHandler::handleSelfMsg(cMessage* msg)
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