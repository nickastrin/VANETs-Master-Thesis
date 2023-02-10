#include "veins/modules/application/traci/Thesis/OriginHandler.h"

using namespace veins;
using namespace omnetpp;

Define_Module(veins::OriginHandler);

// -------------- Initializers --------------- //

void OriginHandler::initialize(int stage)
{
    // Initializing variables
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0)
    {
        // Simulation variables
        unit = UnitType::ORIGIN;
        simulation = Scenario::MACHINE_LEARNING;
        caching = CachingPolicy::FIFO;
        centrality = CentralityType::CLOSENESS;

        // Rsu centrality variables
        pushRsu = 0;
        highestCentrality = 0;
        // Number of RSUs in simulation
        rsuCount = 2;

        // Create initializing self message
        Message *init = new Message();
        populateWSM(init);
        // Define properties
        init->setState(CurrentState::INITIALIZING);
        scheduleAt(simTime() + 5, init);


        // Different handling, depending on scenario
        if (simulation == Scenario::MACHINE_LEARNING)
        {
            // Create writing self message
            Message *extract = new Message();
            populateWSM(extract);
            // Define properties
            extract->setState(CurrentState::EXTRACTING);
            scheduleAt(simTime() + 35, extract);
        }

        else 
        {
            // Create requesting self message
            Message *request = new Message();
            populateWSM(request);
            // Define properties
            request->setState(CurrentState::REQUESTING);
            scheduleAt(simTime() + 65, request);
        }

    }
}

void OriginHandler::initializingMsg()
{
    // Create route discovery request for RSUs
    createRouteReq(MessageType::ORIGIN_INIT_REQ, rsuCount, true);

    Message *test = new Message();
    populateWSM(test);
    test->setContentId("4");
    test->setContent("Test");
    test->setSegments(1);
    test->setMultimedia(true);
    ContentWrapper content = ContentWrapper(test);
    std::string id = "4";
    multimediaData["5"] = content;

    Message *test_v2 = new Message();
    populateWSM(test_v2);
    test_v2->setContentId("5");
    test_v2->setContent("Rnd");
    test_v2->setMultimedia(false);
    test_v2->setSegments(3);
    content = ContentWrapper(test_v2);
    id = "5";
    roadData["5"] = content;
}

void OriginHandler::requestingMsg()
{
    // Create message to request centralities
    Message *request = new Message();
    populateWSM(request);
    // Define message identifiers
    request->setSenderAddress(myId);
    request->setSenderPosition(curPosition);
    request->setSource(myId);
    // Define message properties
    request->setMaxHops(rsuCount);
    request->setCentrality(centrality);
    request->setOriginMessage(true);
    request->setUpdatePaths(false);
    request->setType(MessageType::ORIGIN_CENTRALITY_REQ);

    scheduleAt(simTime() + 5, request);

    // Create collection message
    Message *collect = new Message();
    populateWSM(collect);
    // Define message properties
    collect->setState(CurrentState::COLLECTING);
    scheduleAt(simTime() + 40, collect); 
    
    // Create new requesting self message
    Message *wsm = new Message();
    populateWSM(wsm);
    // Define message properties
    wsm->setState(CurrentState::REQUESTING);
    scheduleAt(simTime() + 60, wsm);
}

void OriginHandler::collectingMsg(Message *wsm)
{
    // Create new message
    Message *info = new Message();
    populateWSM(info);
    // Define message identifiers
    info->setSenderAddress(myId);
    info->setSenderPosition(curPosition);
    info->setSource(myId);
    info->setDest(pushRsu);
    // Define message properties
    info->setType(MessageType::PUSH_CENTRALITY);
    info->setMsgInfo(highestCentrality);
    info->setMaxHops(rsuCount);
    info->setOriginMessage(true);

    scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), info);

    createPushContent();
    delete(wsm);
}

void OriginHandler::extractingMsg()
{
    std::ifstream file;
    file.open("cluster_centers.csv");

    if (file.fail())
        std::cout << "File not ready..." << endl;
    else
    {
        std::string line;
        std::string word;
        std::vector<std::string> row;

        while (getline(file, line))
        {
            row.clear();

            std::stringstream s(line);
            while (getline(s, word, ','))
                row.push_back(word);

            double x = std::stod(row[0]);
            double y = std::stod(row[1]);
            Coord centroidPos = Coord(x, y);

            double distance = 9999;
            for (auto it = rsuPositions.begin(); it != rsuPositions.end(); it++)
            {
                double temp = it->second.distance(centroidPos);
                if (temp < distance)
                {
                    distance = temp;
                    pushRsu = it->first;
                }
            }

            // Create new message
            Message *info = new Message();
            populateWSM(info);
            // Define message identifiers
            info->setSenderAddress(myId);
            info->setSenderPosition(curPosition);
            info->setSource(myId);
            info->setDest(pushRsu);
            // Define message properties
            info->setType(MessageType::PUSH_ML);
            info->setMaxHops(rsuCount);
            info->setOriginMessage(true);

            scheduleAt(simTime() + 0.1 + uniform(0.01, 0.2), info);
            createPushContent();
        }

        std::cout << pushRsu << endl;
        file.close();

        if (!remove("cluster_centers.csv"))
            std::cout << "File cluster_centers.csv deleted.\n";
        else
            std::cout << "File not found...\n";
    }

    // Create new extracting self message
    Message *extract = new Message();
    populateWSM(extract);
    // Define properties
    extract->setState(CurrentState::EXTRACTING);
    scheduleAt(simTime() + 10, extract);
}



void OriginHandler::onWSM(BaseFrame1609_4 *frame)
{
    Message *wsm = check_and_cast<Message*>(frame);
    bool accept = false;

    if (wsm->getOriginMessage())
        accept = receiveMessage(wsm, rsuRouting);

    if (accept)
    {
        // Change RSU color to gold
        findHost()->getDisplayString().setTagArg("i", 1, "purple");
        UnitHandler::onWSM(frame);
    }
}

void OriginHandler::onRsuInitReq(Message *wsm)
{
    if (rsuPositions.find(wsm->getSource()) == rsuPositions.end())
    {
        double x = wsm->getSenderPosition().x;
        double y = wsm->getSenderPosition().y;

        rsuPositions[wsm->getSource()] = Coord(x, y);
    }

    // Reply to RSU init request
    handleRouteReq(wsm, MessageType::RSU_INIT_REPLY);
}

void OriginHandler::onRsuInitReply(Message *wsm)
{
    // Forward init request
    handleRouteTraversal(wsm);
}


void OriginHandler::onOriginCentralityReply(Message *wsm)
{
    float data = wsm->getMsgInfo();

    if (data > highestCentrality)
    {
        highestCentrality = data;
        pushRsu = wsm->getSource();
        std::cout << "Highest centrality: " << data << endl;
        std::cout << "Push RSU: " << pushRsu << endl;
    }
} 


void OriginHandler::onPullReq(Message *wsm)
{
    // If not meant for you, forward
    if (wsm->getDest() != myId)
        handleRouteTraversal(wsm, true);
    
    else
    {
        bool multimedia = wsm->getMultimedia();

        if (multimedia)
            contentSearch(wsm, multimediaData, true);
        else
            contentSearch(wsm, roadData, true);
    }
}

void OriginHandler::onPullReply(Message *wsm)
{
    // Forward to next in line
    handleRouteTraversal(wsm, true);
    std::cout << "Handling pull reply" << endl;
}

// ----------- Auxilary Functions ------------ //

void OriginHandler::createPushContent()
{
    long id = pushRsu;
    auto it = rsuRouting.find(id);
    pathDeque route = it->second;

    // Create content message
    Message *push = new Message();
    populateWSM(push);
    // Define message identifiers
    push->setSource(myId);
    push->setDest(id);
    push->setSenderAddress(myId);
    push->setSenderPosition(curPosition);
    push->setRecipient(route.front());
    // Define message details
    push->setMaxHops(route.size());
    push->setOriginMessage(true);
    push->setUpdatePaths(false);
    push->setType(MessageType::PUSH_CONTENT);
    // Edit route
    route.pop_front();
    push->setRoute(route);
    
    sendContent(push, roadData, false);
    sendContent(push, multimediaData, true);
}

void OriginHandler::sendContent(Message *wsm, storageDict &storage, bool multimedia)
{
    int segments; 
    float delay = 1;

    std::cout << "Sending\n"; 
    for (auto i = storage.begin(); i != storage.end(); i++)
    {
        segments = i->second.segments;
        std::cout << "Segments " << segments << endl;
        std::cout << "Size " << storage.size() << endl;
        // Add flags and identifiers
        wsm->setMultimedia(multimedia);
        wsm->setContentId(i->first.c_str());
        wsm->setSegments(segments);

        // If multiple segments, break it down
        if (segments > 1)
        {
            std::string segmentedContent;

            for (int j = 1; j <= segments; j++)
            {
                // Break info in smaller parts
                segmentedContent = i->second.content[j - 1];
                // Edit reply content
                wsm->setSegmentNumber(j);
                wsm->setContent(segmentedContent.c_str());
                scheduleAt(simTime() + j * delay + 2 + uniform(0.01, 0.2), wsm->dup());
                
                // Segment pending acknowledgement
                pendingAck.push_back(MessageData(wsm));
            }
        }

        else 
        {
            wsm->setContent(i->second.content.c_str());

            // Segment pending acknowledgement
            pendingAck.push_back(MessageData(wsm));
            scheduleAt(simTime() + 2 + uniform(0.01, 0.2), wsm->dup());
        }
    }
}