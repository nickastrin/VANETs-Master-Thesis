import veins.base.utils.Coord;
import veins.modules.messages.BaseFrame1609_4;
import veins.base.utils.SimpleAddress;

cplusplus 
{{
    #include <list>

    typedef std::list<long> SearchFront;
}};

class SearchFront { @existingClass; };

namespace veins;

enum messageType
{
    BROADCAST = 0;
    REQUEST = 1;
    REPLY = 2;
    RSU_CHECK = 3;
    RSU_REPLY = 4;
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
    LAddress::L2Type target;

    // Message properties
    int maxHops = 1;
    int hopCount = 0;

    // Message specifications
    messageType type = messageType::BROADCAST;
    centralityType centrality = centralityType::NONE;
    procedureState state = procedureState::SENDING;         // It might be pointless, but we'll see

    // Message data
    string messageData = "";                                // Might be pointless as well
    bool passedRsu = false;                                 // Variable to determine if message has passed from an RSU, might be pointless
    SearchFront searchFront;                                // List used for centrality calculations
    Coord senderPosition;                                   // Variable used for distance calculations
}
