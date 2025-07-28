#include "CarHandler.h"

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

        maxHops = 120;       // Default maximum ttl

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
            wsm->setMultimedia(false);

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
    cModule *mod;
    // The first node in the simulation is responsible writing the data to file
    if (simTime() < 250)
        mod = getParentModule()->getModuleByPath("node[0]");
    else 
        mod = getParentModule()->getModuleByPath("node[70]");
    if (mod == nullptr)
        throw cRuntimeError("Could not find module with path specified");

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

    cModule *origin = getParentModule()->getModuleByPath("origin[0]");
    if (origin == nullptr)
        throw cRuntimeError("Could not find module with path: origin[0]");

    int id = origin->par("scenarioId");
    int sc = origin->par("contentId");
    int rsuCount = origin->par("rsuCount");
    int requestVehicle;

    switch (id)
    {
        case 1 ... 8:
            tstamp = 130;
            requestVehicle = 178;
            break;
        case 9 ... 16:
            tstamp = 210;
            requestVehicle = 376;
            break;
        case 17 ... 24:
            tstamp = 290;
            requestVehicle = 394;
            break;
    }

    std::string simType;
    std::string centType;
    std::string origType;

    if (id % 8 == 1)
    {
        simType = "ML";
        centType = "Degree";
        origType = "Push";
    }
    else if (id % 8 == 2)
    {
        simType = "ML";
        centType = "Degree";
        origType = "Pull";
    }
    else if (id % 8 == 3)
    {
        simType = "Manual";
        centType = "Degree";
        origType = "Push";
    }
    else if (id % 8 == 4)
    {
        simType = "Manual";
        centType = "Degree";
        origType = "Pull";
    }
    else if (id % 8 == 5)
    {
        simType = "Manual";
        centType = "Closeness";
        origType = "Push";
    }
    else if (id % 8 == 6)
    {
        simType = "Manual";
        centType = "Closeness";
        origType = "Pull";
    }
    else if (id % 8 == 7)
    {
        simType = "Manual";
        centType = "Betweenness";
        origType = "Push";
    }
    else if (id % 8 == 0)
    {
        simType = "Manual";
        centType = "Betweenness";
        origType = "Pull";
    }

    simtime_t time = tstamp + uniform(0.01, 0.5);

    if (myId == requestVehicle)
    {
        std::ofstream file;
        struct stat buffer;   
        if (stat("response_time.csv", &buffer) != 0)
        {
            file.open("response_time.csv", std::ios_base::app);
            file << "RSU Count, ScenarioId, ContentId, Timestamp, Completion, REQ/REP, SimType, Centrality, OriginPolicy\n";
            file.close();
        }
        
        std::cout << "Requesting content " << sc << std::endl;
        file.open("response_time.csv", std::ios_base::app);

        if (sc == 5 || sc == 05)
        {
            createRequest("5", true, time);
            file << std::to_string(rsuCount) + "," + 
                        std::to_string(id) + "," +
                        "5" + "," +
                        std::to_string(time.dbl()) + ","  + 
                        "-" + "," +
                        "REQ" + "," + 
                        simType + "," +
                        centType + "," + 
                        origType + "\n";
        }
        else if (sc == 4 || sc == 04)
        {
            createRequest("4", true, time + 5);
            file << std::to_string(rsuCount) + "," + 
                        std::to_string(id) + "," +
                        "4" + "," +
                        std::to_string(time.dbl() + 5) + ","  + 
                        "-" + "," +
                        "REQ" + "," + 
                        simType + "," +
                        centType + "," + 
                        origType + "\n";
        }
        else 
            std::cout << "Wrong content id" << std::endl;
            
        
        file.close();
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
    scheduleAt(simTime() + 75, write);
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
        // Change RSU color to green
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

    scheduleAt(simTime() + 1 + uniform(0.01, 0.5), wsm->dup());
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

        scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup());
    }
}

// ----------- On Centrality Reply ----------- //

void CarHandler::onDegreeReply(Message *wsm)
{
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup());
}

void CarHandler::onBetweennessReply(Message *wsm)
{
    wsm->setSenderAddress(myId);
    wsm->setSenderPosition(curPosition);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), wsm->dup());
}

// -------------- Auxiliary ------------------- //

void CarHandler::createRequest(std::string contentId, bool multimedia, simtime_t time)
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

    std::cout << "Node " << myId << ", sending request at " << time << std::endl;
    scheduleAt(time, wsm);
}