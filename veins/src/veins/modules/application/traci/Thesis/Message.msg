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

enum messageStage
{
    INITIALIZING = 0;
    SENDING = 1;
    COLLECTING = 2;
}

packet Message extends BaseFrame1609_4 
{
    // Message data
    string messageData = "";

    // Message TTL
    int hopCount = 0;
    int maxHops = 2;

    SearchFront searchFront;

    // Address of sender. Default is broadcast
    LAddress::L2Type senderAddress = -1;    

    // Define message type. Default is broadcast
    messageType type = messageType::BROADCAST;
    // Define centrality type for centrality calculations. Default is none
    centralityType centrality = centralityType::NONE;
    // Define message stage. Used for collecting and initializing during selfMessage
    messageStage stage = messageStage::SENDING;

    // Coordinates of the sender
    Coord senderPosition;
}