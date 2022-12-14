import veins.base.utils.Coord;
import veins.modules.messages.BaseFrame1609_4;
import veins.base.utils.SimpleAddress;

cplusplus 
{{
    #include <list>

    typedef std::list<long> List;
}};

class List { @existingClass; };

namespace veins;

enum messageType
{
    BROADCAST = 0;
    REQUEST = 1;
    REPLY = 2;
    RSU_REQUEST = 3;
    CENTRALITY_REQUEST = 4;
    CENTRALITY_REPLY = 5;
    RSU_REPLY = 6;
    ACKNOWLEDGEMENT = 7;
}

enum currentState
{

    INITIALIZING = 0;
    SENDING = 1;
    COLLECTING = 2;
    CACHING = 3;
    TESTING = 4;
    REPEATING = 5;
}

enum cachingPolicy
{
    FIFO = 0;
    LRU = 1;
    LFU = 2;
}

enum scenario
{
    CENTRALITY = 0;
    CACHE = 1;
}

enum selectedCentrality
{
    DEGREE = 0;
    CLOSENESS = 1;
    BETWEENNESS = 2;
    NONE = 3;
}

packet Message extends BaseFrame1609_4
{
    LAddress::L2Type senderAddress = -1;

    LAddress::L2Type source;
    LAddress::L2Type target;

    // Message properties
    int testFlag = 0;
    int maxHops = 1;
    int hops = 0;

    // Message specifications
    messageType type = messageType::BROADCAST;
    currentState state = currentState::SENDING;  
    selectedCentrality centrality = selectedCentrality::NONE;

    // Message data
    string roadData = "";                                
    int centralityData;
    simtime_t ackData;

    List rsuList;                                           // Variable to determine if message has passed from an RSU, might be pointless
    List pathList;                                          // List used for centrality calculations
    Coord senderPosition;                                   // Variable used for distance calculations
}
