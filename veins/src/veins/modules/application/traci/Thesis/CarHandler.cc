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
        capacity = 200;
        threshold = 30;
        flushed = 20;

        maxHops = 55;       // Default maximum ttl

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

        thresholdControl();
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

/*
    if (simTime() == 120 && myId == 58)
        createRequest("5", false);*/
}

// -------------- Self Message --------------- //

void CarHandler::initializingMsg()
{
    // The first node in the simulation is responsible writing the data to file
    cModule *mod = getParentModule()->getModuleByPath("node[0]");
    if (mod == nullptr)
        throw cRuntimeError("Could not find module with path: node[0]");

    long initId = mod->getId() + 2;
    if (myId == initId)
    {
        // Create writing self message
        Message *write = new Message();
        populateWSM(write);
        // Define properties
        write->setState(CurrentState::WRITING);
        scheduleAt(75, write);
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
    scheduleAt(simTime() + 60, write);
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

        if (wsm->getSource() == 34)
            findHost()->getDisplayString().setTagArg("i", 1, "cyan");
            
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

void CarHandler::onRequest(Message *wsm)
{
    // Check if you have the content
    bool found = false;
    bool multimedia = wsm->getMultimedia();

    if (multimedia)
        found = contentSearch(wsm, multimediaData, true);
    else
        found = contentSearch(wsm, roadData, true);

    if (!found)
    {
        // Forward request to reach other cars
        wsm->setSenderAddress(myId);
        wsm->setSenderPosition(curPosition);

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), wsm->dup());
    }
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

// -------------- Auxilary ------------------- //

void CarHandler::createRequest(std::string contentId, bool multimedia)
{
    // Create request message
    Message *wsm = new Message();
    populateWSM(wsm);
    // Define identifiers
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);
    wsm->setSource(myId);
    // Define properties
    wsm->setType(MessageType::REQUEST);
    // Define content
    wsm->setContentId(contentId.c_str());
    wsm->setMultimedia(multimedia);

    // Send request
    simtime_t time = simTime() + 1 + uniform(0.01, 0.2);
    std::cout << "Node " << myId << ", sending request at " << time << std::endl;
    scheduleAt(time, wsm);
}