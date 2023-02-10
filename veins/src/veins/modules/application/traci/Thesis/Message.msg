import veins.base.utils.Coord;
import veins.modules.messages.BaseFrame1609_4;
import veins.base.utils.SimpleAddress;

cplusplus 
{{
    #include <deque>
    typedef std::deque<long> Deque;
}};

class Deque { @existingClass; };

enum MessageType 
{
    BROADCAST = 0;
    REQUEST = 1;
    REPLY = 2;
    ROUTE_REQ = 3;
    ROUTE_REPLY = 4;
    CENTRALITY_REQ = 5;
    CENTRALITY_REPLY = 6;
    ACKNOWLEDGEMENT = 7;
    ORIGIN_INIT_REQ = 8;
    ORIGIN_INIT_REPLY = 9;
    RSU_INIT_REQ = 10;
    RSU_INIT_REPLY = 11;
    PULL_REQ = 12;
    PULL_REPLY = 13;
    PUSH_CENTRALITY = 14;
    PUSH_ML = 15;
    PUSH_CONTENT = 16;
    ORIGIN_CENTRALITY_REQ = 17;
    ORIGIN_CENTRALITY_REPLY = 18;
}

enum CurrentState
{
    INITIALIZING = 0;
    SENDING = 1;
    REQUESTING = 2;
    REPEATING = 3;
    COLLECTING = 4;
    CACHING = 5;
    WRITING = 6;
    EXTRACTING = 7;
}

enum CentralityType 
{
    DEGREE = 0;
    CLOSENESS = 1;
    BETWEENNESS = 2;
}

namespace veins;

packet Message extends BaseFrame1609_4
{
    LAddress::L2Type msgId;

    // Message identifiers
    LAddress::L2Type senderAddress = -1;
    LAddress::L2Type recipient = -1;
    LAddress::L2Type source;
    LAddress::L2Type dest;
    Coord senderPosition;

    // Message properties
    MessageType type = MessageType::BROADCAST;
    CurrentState state = CurrentState::SENDING;
    CentralityType centrality;
    // Variables used for origin transmission
    bool originMessage = false;
    bool updatePaths = true;
    // Variables used for message transmission
    int hops = 0;
    int maxHops = 2;
    // Message data
    float msgInfo = 0;
    simtime_t ackInfo;

    // Route variables
    Deque route;
    Deque previousNodes;
    // Content variables
    string contentId = "";
    string content = "";
    int segments = 1;
    int segmentNumber = 1;
    bool multimedia = false;
}
