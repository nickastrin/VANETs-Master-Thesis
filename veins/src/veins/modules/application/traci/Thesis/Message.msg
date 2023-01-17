import veins.base.utils.Coord;
import veins.modules.messages.BaseFrame1609_4;
import veins.base.utils.SimpleAddress;

cplusplus 
{{
    #include <vector>
    typedef std::vector<long> Vector;
}};

class Vector { @existingClass; };

enum MessageType 
{
    HELLO = 0;
    BROADCAST = 1;
    REQUEST = 2;
    REPLY = 3;
    ROUTE_REQ = 4;
    ROUTE_REPLY = 5;
    CENTRALITY_REQ = 6;
    CENTRALITY_REPLY = 7;
    ACKNOWLEDGEMENT = 8;
}

enum CurrentState
{
    INITIALIZING = 0;
    SENDING = 1;
    REQUESTING = 2;
    REPEATING = 3;
    COLLECTING = 4;
    CACHING = 5;
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
    long msgId;                 // Message Id

    // Sender variables
    LAddress::L2Type senderAddress = -1;
    Coord senderPosition;

    // Message route variables
    LAddress::L2Type source;
    LAddress::L2Type dest;

    // Message properties
    int ttl = 1;                // Time to live, default value 1
    int hops = 0;               // Hops, initialized at 0

    // Message specifications
    MessageType type = MessageType::BROADCAST;
    CurrentState state = CurrentState::SENDING;
    CentralityType centrality;


    // Message data
    string roadData;            // Information about road
    int msgInfo;                // Misc message info

    // Route variables
    Vector route;               // Route taken
    Vector rsuRoute;            // Rsus in the route, used for betweenness
    Vector previousNodes;       // Used for acknowledgements
}
