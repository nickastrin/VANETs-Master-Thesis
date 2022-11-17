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
};

enum centralityType
{
    NONE = 0;
    DEGREE = 1;
    CLOSENESS = 2;
    BETWEENNESS = 3;
};

enum procedureState
{
    INITIALIZING = 0;
    SENDING = 1;
    COLLECTING = 2;
}

packet Message extends BaseFrame1609_4
{
    LAddress::L2Type senderAddress = -1;

    LAddress::L2Type source;
    LAddress::L2Type target;

    // Message properties
    int maxHops = 1;
    int hops = 0;

    // Message specifications
    messageType type = messageType::BROADCAST;
    procedureState state = procedureState::SENDING;  
    centralityType centrality = centralityType::NONE;

    // Message data
    string roadData = "";                                
    int centralityData;

    List rsuList;                                 // Variable to determine if message has passed from an RSU, might be pointless
    List pathList;                                          // List used for centrality calculations
    Coord senderPosition;                                   // Variable used for distance calculations
}
