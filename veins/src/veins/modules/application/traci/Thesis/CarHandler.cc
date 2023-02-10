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
        capacity = 100;
        threshold = 30;
        flushed = 10;

        maxHops = 25;       // Default maximum ttl

        // Misc car variables
        currentSubscribedServiceId = -1;
        lastDroveAt = simTime();
        sentMessage = false;

        // Simulation variables
        unit = UnitType::VEHICLE;
        caching = CachingPolicy::FIFO;

        lastUpdated = simTime();

        // Create initializing self message
        Message *init = new Message();
        populateWSM(init);
        // Define properties
        init->setState(CurrentState::INITIALIZING);
        scheduleAt(simTime() + 5, init);
        // TODO: Initialize threshold control
    }
}

// ------------ Position Handling ------------ //

void CarHandler::handlePositionUpdate(cObject *obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    if (mobility->getSpeed() < 1)
    {
        if (simTime() - lastDroveAt >= 10 && sentMessage == false)
        {
            findHost()->getDisplayString().setTagArg("i", 1, "red");
            sentMessage = true;

            // Create message
            Message *wsm = new Message();
            populateWSM(wsm);
            // Define identifiers
            wsm->setSenderAddress(myId);
            wsm->setSenderPosition(curPosition);
            wsm->setSource(myId);
            // Define content
            wsm->setContentId(mobility->getRoadId().c_str());
            wsm->setContent(mobility->getRoadId().c_str());

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

// -------------- Self Message --------------- //

void CarHandler::initializingMsg()
{
    // The first car to enter simulation
    if (myId == 28)
    {
        // Create writing self message
        Message *write = new Message();
        populateWSM(write);
        // Define properties
        write->setState(CurrentState::WRITING);
        scheduleAt(30, write);
    }
}

void CarHandler::collectingMsg(Message *wsm)
{
    UnitHandler::collectingMsg(wsm);
    delete(wsm);
}

void CarHandler::writingMsg()
{
    std::cout << "Writing message" << std::endl;
    std::map<std::string, cModule*> availableCars = mobility->getManager()->getManagedHosts();
    std::ofstream file;

    file.open("vehicles.csv");
    file << "X position,Y position,Speed,Direction\n";
    //Iterate through collection and find data
    for(auto it = availableCars.begin(); it != availableCars.end(); it++)
    {
        TraCIMobility* carMobility = TraCIMobilityAccess().get(it->second);
        Coord position = carMobility->getPositionAt(simTime());

        double xPosition = position.x;
        double yPosition = position.y;
        double speed = carMobility->getSpeed();
        double direction = carMobility->getHeading().getRad();

        file << std::to_string(xPosition) + "," + std::to_string(yPosition) + "," + std::to_string(speed) + "," + std::to_string(direction) + "\n";
    }

    file.close();   
    
    // Create new writing self message
    Message *write = new Message();
    populateWSM(write);
    // Define properties
    write->setState(CurrentState::WRITING);
    scheduleAt(simTime() + 30, write);
}

// --------------- On Message ---------------- //

void CarHandler::onWSA(DemoServiceAdvertisment *wsa)
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

void CarHandler::onWSM(BaseFrame1609_4* frame)
{
    Message *wsm = check_and_cast<Message*>(frame);
    bool accept = false;

    if (!wsm->getOriginMessage())
        accept = receiveMessage(wsm, routingTable);

    if (accept)
    {
        // Change RSU color to gold
        findHost()->getDisplayString().setTagArg("i", 1, "green");
        UnitHandler::onWSM(frame);
    }
}

void CarHandler::onBroadcast(Message *wsm)
{
    // Extract content from broadcast
    std::string content = extractContent(wsm);
    // If it's a road announcement
    if (!wsm->getMultimedia())
    {   
        // Change road if you can
        if (mobility->getRoadId()[0] != ':') 
            traciVehicle->changeRoute(wsm->getContent(), 9999);
    }

    // Then forward to other nodes in the network
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 1 + uniform(0.01, 0.2), wsm->dup());
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