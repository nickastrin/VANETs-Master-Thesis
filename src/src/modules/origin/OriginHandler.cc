#include "OriginHandler.h"

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
        caching = CachingPolicy::FIFO;

        initializeVariables();

        // Rsu centrality variables
        //pushRsu = 0;
        highestCentrality = 0;

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
            scheduleAt(simTime() + 80, extract);
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
    multimediaData[id] = content;

    Message *test_v2 = new Message();
    populateWSM(test_v2);
    test_v2->setContentId("5");
    test_v2->setContent("Rnd");
    test_v2->setMultimedia(true);
    test_v2->setSegments(3);
    content = ContentWrapper(test_v2);
    id = "5";
    multimediaData[id] = content;
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

    std::cout << "Origin " << myId << " requesting centralities, at " << simTime() + 5 << endl;

    std::ofstream file;
    struct stat buffer;   
    if (stat("centrality_time.csv", &buffer) != 0)
    {
        file.open("centrality_time.csv", std::ios_base::app);
        file << "RSU Count, ID, Timestamp, Centrality, Value\n";
        file.close();
    }

    std::string centType = "";

    if (centrality == CentralityType::DEGREE)
        centType = "Degree";
    else if (centrality == CentralityType::CLOSENESS)
        centType = "Closeness";
    else if (centrality == CentralityType::BETWEENNESS)
        centType = "Betweenness";
        
    if (simTime() > 225)
    {
        file.open("centrality_time.csv", std::ios_base::app);
        file << std::to_string(rsuCount) + "," + 
                    std::to_string(myId) + "," +
                    std::to_string(simTime().dbl() + 5) + ","  + 
                    "REQ" + "," + 
                    centType + "," + 
                    "-" + "\n";
        file.close();
    }

    highestCentrality = 0;
    scheduleAt(simTime() + 5, request);

    // Create collection message
    Message *collect = new Message();
    populateWSM(collect);
    // Define message properties
    collect->setState(CurrentState::COLLECTING);
    scheduleAt(simTime() + 55, collect); 
    
    // Create new requesting self message
    Message *wsm = new Message();
    populateWSM(wsm);
    // Define message properties
    wsm->setState(CurrentState::REQUESTING);
    scheduleAt(simTime() + 80, wsm);
}

void OriginHandler::collectingMsg(Message *wsm)
{
    float delay = 0.5;

    // Create new message
    Message *info = new Message();
    populateWSM(info);
    // Define message identifiers
    info->setSenderAddress(myId);
    info->setSenderPosition(curPosition);
    info->setSource(myId);
    //info->setDest(pushRsu);
    // Define message properties
    info->setType(MessageType::PUSH_CENTRALITY);
    info->setMsgInfo(highestCentrality);
    info->setMaxHops(rsuCount);
    info->setOriginMessage(true);

    for (auto it = centralVec.begin(); it != centralVec.end(); ++it)
    {
        info->setDest(*it);
        scheduleAt(simTime() + delay + uniform(0.01, 0.5), info->dup());
        delay += 0.2;
    }

    //scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), info);

    //if (origin == OriginPolicy::PUSH && pushRsu != 0)
    if (origin == OriginPolicy::PUSH && !centralVec.empty())
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

        std::cout << "CLEARED\n";
        centralVec.clear();

        while (getline(file, line))
        {
            std::cout << "Line: " << line << endl;
            row.clear();

            std::stringstream s(line);
            while (getline(s, word, ','))
                row.push_back(word);

            double x = std::stod(row[0]);
            double y = std::stod(row[1]);
            Coord centroidPos = Coord(x, y);

            long address;

            double distance = 9999;
            for (auto it = rsuPositions.begin(); it != rsuPositions.end(); it++)
            {
                double temp = it->second.distance(centroidPos);
                if (temp < distance)
                {
                    distance = temp;
                    //pushRsu = it->first;
                    address = it->first;
                }
            }

            std::cout << address << endl;

            if (!(std::find(centralVec.begin(), centralVec.end(), address) != centralVec.end()))
            {
                std::cout << "Push RSU Id: " << address << endl;
                centralVec.push_back(address);

                // Create new message
                Message *info = new Message();
                populateWSM(info);
                // Define message identifiers
                info->setSenderAddress(myId);
                info->setSenderPosition(curPosition);
                info->setSource(myId);
                info->setDest(address);
                // Define message properties
                info->setType(MessageType::PUSH_ML);
                info->setMaxHops(rsuCount);
                info->setOriginMessage(true);

                scheduleAt(simTime() + 0.1 + uniform(0.01, 0.5), info);
            }
        }

        if (origin == OriginPolicy::PUSH && !centralVec.empty())
            createPushContent();

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
        double x = wsm->getInitPosition().x;
        double y = wsm->getInitPosition().y;

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
    std::cout << "Origin " << myId << " received centrality reply from RSU " << wsm->getSource() << " at " << simTime() << endl;
    float data = wsm->getMsgInfo();

    if (data == highestCentrality)
    {
        centralVec.push_back(wsm->getSource());
        std::cout << "Additional push RSU: " << centralVec[0] << endl;
    }

    if (data > highestCentrality)
    {
        std::vector<long> newCentralVec;
        newCentralVec.push_back(wsm->getSource());
        centralVec = newCentralVec;

        highestCentrality = data;
        //pushRsu = wsm->getSource();
        for (auto it = centralVec.begin(); it != centralVec.end(); ++it)
        {
            std::cout << "Push RSU: " << *it << endl;
            std::cout << "Highest centrality: " << data << endl;
        }
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

void OriginHandler::onPushContent(Message *wsm)
{
    // If not meant for you, forward
    if (wsm->getDest() != myId)
        handleRouteTraversal(wsm, true);
    
    else
    {
        std::string content = extractContent(wsm);
        sendAcknowledgement(wsm);

        //bool pushContentToRsu = (pushRsu != 0) && (wsm->getSource() != pushRsu);

        bool pushContentToRsu = false;
        if (!centralVec.empty())
            pushContentToRsu = !(std::find(centralVec.begin(), centralVec.end(), wsm->getSource()) != centralVec.end());


        if (!content.empty() && origin == OriginPolicy::PUSH && pushContentToRsu)
        {
            simtime_t time;
            float sequenceDelay = 0.0;

            for (auto j = centralVec.begin(); j != centralVec.end(); j++)
            {
                auto it = rsuRouting.find(*j);
                pathDeque route = it->second;

                // Create push message
                Message *push = new Message();
                populateWSM(push);
                // Define message identifiers
                push->setSource(myId);
                push->setDest(*j);
                push->setSenderAddress(myId);
                push->setSenderPosition(curPosition);
                push->setRecipient(route.front());
                // Define message properties
                push->setMaxHops(route.size());
                push->setOriginMessage(true);
                push->setUpdatePaths(false);
                push->setType(MessageType::PUSH_CONTENT);
                push->setMultimedia(wsm->getMultimedia());
                // Edit route details
                route.pop_front();
                push->setRoute(route);

                // Define content details
                int segments = wsm->getSegments();

                push->setContentId(wsm->getContentId());
                push->setSegments(segments);
                push->setMultimedia(wsm->getMultimedia());

                // If segmented, send in parts
                if (segments > 1)
                {
                    std::string segmentedContent;
                    float delay = 0.1;

                    for (int i = 1; i <= segments; i++)
                    {
                        time = simTime() + i * delay + sequenceDelay +  uniform(0.01, 0.5);
                        std::cout << "Origin " << myId << " sending segmented content to " << *j << " with Content ID " << wsm->getContentId();
                        std::cout << " and segment number " << i << " at " << time << endl;

                        // Break into smaller strings
                        segmentedContent = content[i - 1];

                        push->setSegmentNumber(i);
                        push->setContent(segmentedContent.c_str());
                        scheduleAt(time, push->dup());

                        // Segment pending acknowledgement
                        Message *repeat = push->dup();
                        pendingAck.push_back(MessageData(repeat));            
                        repeat->setState(CurrentState::REPEATING);
                        scheduleAt(simTime() + 5 + i * delay + sequenceDelay, repeat);
                    }
                }

                else
                {   
                    time = simTime() + 0.1 + sequenceDelay + uniform(0.01, 0.5);
                    std::cout << "Origin " << myId << " sending content with Content Id " << wsm->getContentId() << " to " << *j << " at " << time << endl;

                    push->setContent(content.c_str());
                    scheduleAt(time, push->dup());

                    // Segment pending acknowledgement
                    pendingAck.push_back(MessageData(push));
                    push->setState(CurrentState::REPEATING);
                    scheduleAt(simTime() + 5 + sequenceDelay, push);
                }

                sequenceDelay += 0.5;

            }
        }
    }
}

// ----------- Auxiliary Functions ------------ //

void OriginHandler::createPushContent()
{
    float delay = 0.0;
    for (auto i = centralVec.begin(); i != centralVec.end(); i++)
    {
        //long id = pushRsu;
        long id = *i;
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
        
        sendContent(push, roadData, false, delay);
        sendContent(push, multimediaData, true, delay);

        delay += 0.5;
    }
}

void OriginHandler::sendContent(Message *wsm, storageDict &storage, bool multimedia, float centralDelay)
{
    int segments; 
    simtime_t time;
    float delay = 1;

    //std::cout << "Storage size " << storage.size() << endl;
    for (auto i = storage.begin(); i != storage.end(); i++)
    {
        segments = i->second.segments;
        //std::cout << "Content ID " << i->first << endl;
        //std::cout << "Segments " << segments << endl;

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
                time = simTime() + j * delay + centralDelay + 2 + uniform(0.01, 0.5);

                std::cout << "Origin " << myId << " pushing segmented content to RSU " << wsm->getDest();
                std::cout << " segment number " << j << " at " << time << endl;

                // Break info in smaller parts
                segmentedContent = i->second.content[j - 1];
                // Edit reply content
                wsm->setSegmentNumber(j);
                wsm->setContent(segmentedContent.c_str());
                scheduleAt(time, wsm->dup());
                
                // Segment pending acknowledgement
                Message *repeat = wsm->dup();
                pendingAck.push_back(MessageData(repeat));            
                repeat->setState(CurrentState::REPEATING);
                scheduleAt(simTime() + 5 + centralDelay + j * delay, repeat);
            }
        }

        else 
        {
            time = simTime() + 2 + centralDelay + uniform(0.01, 0.5);
            std::cout << "Origin " << myId << " pushing content to RSU " << wsm->getDest() << " at " << time << endl;

            wsm->setContent(i->second.content.c_str());
            scheduleAt(time, wsm->dup());

            // Segment pending acknowledgement
            Message *repeat = wsm->dup();
            repeat->setState(CurrentState::REPEATING);
            pendingAck.push_back(MessageData(repeat));
            scheduleAt(simTime() + centralDelay + 5, repeat);
        }
    }
}

void OriginHandler::initializeVariables()
{
    int id = getParentModule()->par("scenarioId");

    switch(id)
    {
        case 1:
            simulation = Scenario::MACHINE_LEARNING;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PUSH;
            tstamp = 120;
            break;
        case 2:
            simulation = Scenario::MACHINE_LEARNING;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PULL;
            tstamp = 120;
            break;
        case 3:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PUSH;
            tstamp = 120;
            break;
        case 4: 
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PULL;
            tstamp = 120;
            break;
        case 5:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::CLOSENESS;
            origin = OriginPolicy::PUSH;
            tstamp = 120;
            break;
        case 6:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::CLOSENESS;
            origin = OriginPolicy::PULL;
            tstamp = 120;
            break;
        case 7:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::BETWEENNESS;
            origin = OriginPolicy::PUSH;
            tstamp = 120;
            break;
        case 8:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::BETWEENNESS;
            origin = OriginPolicy::PULL;
            tstamp = 120;
            break;
        case 9:
            simulation = Scenario::MACHINE_LEARNING;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PUSH;
            tstamp = 180;
            break;
        case 10:
            simulation = Scenario::MACHINE_LEARNING;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PULL;
            tstamp = 180;
            break;
        case 11:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PUSH;
            tstamp = 180;
            break;
        case 12: 
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PULL;
            tstamp = 180;
            break;
        case 13:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::CLOSENESS;
            origin = OriginPolicy::PUSH;
            tstamp = 180;
            break;
        case 14:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::CLOSENESS;
            origin = OriginPolicy::PULL;
            tstamp = 180;
            break;
        case 15:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::BETWEENNESS;
            origin = OriginPolicy::PUSH;
            tstamp = 180;
            break;
        case 16:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::BETWEENNESS;
            origin = OriginPolicy::PULL;
            tstamp = 180;
            break;        
        case 17:
            simulation = Scenario::MACHINE_LEARNING;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PUSH;
            tstamp = 240;
            break;
        case 18:
            simulation = Scenario::MACHINE_LEARNING;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PULL;
            tstamp = 240;
            break;
        case 19:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PUSH;
            tstamp = 240;
            break;
        case 20: 
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::DEGREE;
            origin = OriginPolicy::PULL;
            tstamp = 240;
            break;
        case 21:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::CLOSENESS;
            origin = OriginPolicy::PUSH;
            tstamp = 240;
            break;
        case 22:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::CLOSENESS;
            origin = OriginPolicy::PULL;
            tstamp = 240;
            break;
        case 23:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::BETWEENNESS;
            origin = OriginPolicy::PUSH;
            tstamp = 240;
            break;
        case 24:
            simulation = Scenario::MANUAL_CENTRALITY;
            centrality = CentralityType::BETWEENNESS;
            origin = OriginPolicy::PULL;
            tstamp = 240;
            break;
    }

    rsuCount = getParentModule()->par("rsuCount");
    std::cout << "RSU count: " << rsuCount << endl;
}