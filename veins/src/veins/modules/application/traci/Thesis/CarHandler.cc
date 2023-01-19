#include "veins/modules/application/traci/Thesis/CarHandler.h"

using namespace veins;
using namespace omnetpp;

Define_Module(veins::CarHandler);

// -------------- Initializers --------------- //

void CarHandler::initialize(int stage)
{
    // Initializing variables
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0)
    {
        // Centrality metrics
        degree = 0;
        closeness = 0;
        betweenness = 0;

        // Caching variables
        capacity = 15;
        threshold = 30;
        flushed = 3;

        ttl = 25;       // Default maximum ttl
        calculating = false;

        // Misc car variables
        currentSubscribedServiceId = -1;
        lastDroveAt = simTime();
        sentMessage = false;

        // Simulation variables
        unit = UnitType::VEHICLE;
        policy = CachingPolicy::LFU;

        // TODO: Initialize threshold control
    }
}

// ------------ Postition Handling ------------ //

void CarHandler::handlePositionUpdate(cObject *obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

    if (mobility->getSpeed() < 1)
    {
        if (simTime() - lastDroveAt >= 10 && sentMessage == false)
        {
            findHost()->getDisplayString().setTagArg("i", 1, "red");
            sentMessage = true;

            Message *wsm = new Message();
            populateWSM(wsm);

            wsm->setSenderAddress(myId);
            wsm->setSenderPosition(curPosition);
            wsm->setSource(myId);
            wsm->setRoadData(mobility->getRoadId().c_str());

            // The host is standing still due to crash
            if (dataOnSch) 
            {
                startService(Channel::sch2, 42, "Traffic Information Service");
                // started service and server advertising, schedule message to self to send later
                scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
            }
            else 
                sendDown(wsm);
        }
    }

    else
    {
        lastDroveAt = simTime();
        sentMessage = false;
    }
}

// --------------- On Message ---------------- //

void CarHandler::onWSA(DemoServiceAdvertisment* wsa)
{
    // If no assigned service, assign one
    if (currentSubscribedServiceId == -1)
    {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));

        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) 
        {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void CarHandler::onBroadcast(Message *wsm, roadsDeque &roads)
{
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    // If there are road data in the message, store them
    std::string roadData = wsm->getRoadData();
    if (!roadData.empty())
    {
        // And change route, if possible
        if (mobility->getRoadId()[0] != ':') 
            traciVehicle->changeRoute(roadData, 9999);
        roads.push_front(roadData);
    }

    // And resend the message after 2s + delay
    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
}

// ----------- On Centrality Reply ----------- //

void CarHandler::onDegreeReply(Message *wsm)
{
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
}

void CarHandler::onBetweennessReply(Message *wsm)
{
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
}