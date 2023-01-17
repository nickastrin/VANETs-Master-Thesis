#include "veins/modules/application/traci/Thesis/RsuHandler.h"

using namespace veins;
using namespace omnetpp;

Define_Module(veins::RsuHandler);

// -------------- Initializers --------------- //

void RsuHandler::initialize(int stage)
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
        capacity = 25;
        threshold = 30;
        flushed = 4;

        ttl = 25;       // Default maximum ttl
        calculating = false;

        // Simulation variables
        unit = UnitType::RSU;
        policy = CachingPolicy::LFU;
        test = Scenario::CENTRALITY;

        // Send initializing self message
        Message *init = new Message();
        populateWSM(init);
        init->setState(CurrentState::INITIALIZING);
        init->setCentrality(CentralityType::BETWEENNESS);
        scheduleAt(simTime() + 45, init);
    }
}

// -------------- Self Message --------------- //

void RsuHandler::initializingMsg(Message *wsm)
{
    if (test == Scenario::CENTRALITY)
    {   
        Message *req = new Message();
        populateWSM(req);
        req->setState(CurrentState::REQUESTING);
        req->setCentrality(wsm->getCentrality());

        scheduleAt(simTime() + 5, req);
    }

    // For testing the caching policies
    else if (test == Scenario::CACHE)
    {
        Message *dummy = new Message();
        populateWSM(dummy);

        MessageData dummyData = MessageData(dummy);
        for (int i = 0; i < 10; i++)
        {
            flushList(storedMessages, candidateMessages);

            dummyData.usedFrequency = 1 + i;
            dummyData.lastUsed = 100 - i * 10;
            dummyData.receivedAt = 90 - i * 10;
            dummyData.timestamp = (i + 1) * 3;

            storedMessages.push_back(dummyData);
            EV << "Message List: \n";
            for (auto i = storedMessages.begin(); i != storedMessages.end(); i++)
            {
                EV << "Used frequency " << i->usedFrequency;
                EV << " and Last Time Used " << i->lastUsed;
                EV << " and Generated at " << i->timestamp << endl;
            }
        }

        delete(dummy);
    }

    delete(wsm);
}

// --------------- On Message ---------------- //

void RsuHandler::onWSA(DemoServiceAdvertisment* wsa)
{
    // If this RSU receives a WSA for Service 42, it will tune to the channel
    if (wsa->getPsid() == 42) 
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
}

void RsuHandler::onBroadcast(Message *wsm, roadsDeque &roads)
{
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);
    
    // If there are road data in the message, store them
    std::string roadData = wsm->getRoadData();
    if (!roadData.empty())
        roads.push_front(roadData);

    // And resend the message after 2s + delay
    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
}

// ----------- On Centrality Reply ----------- //

void RsuHandler::onDegreeReply(Message *wsm)
{
    if (wsm->getDest() == myId)
    {
        degree++;

        // TODO: Send acknowledgement
    }

    else
    {
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
}

void RsuHandler::onBetweennessReply(Message *wsm)
{
    if (wsm->getDest() == myId)
    {
        betweenness += wsm->getMsgInfo();

        // TODO: Send acknowledgement
    }

    else
    {
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
}

