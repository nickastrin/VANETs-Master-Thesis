import veins.base.utils.Coord;
import veins.modules.messages.BaseFrame1609_4;
import veins.base.utils.SimpleAddress;

namespace veins;

enum messageType
{
    BROADCAST = 0;
    REQUEST = 1;
    REPLY = 2;
    RSU_CHECK = 3;
};

message Message extends BaseFrame1609_4 
{
    // Message data
    string messageData;

    // Message TTL
    int hopCount = 0;

    // Address of sender. Default is broadcast
    LAddress::L2Type senderAddress = -1;    
    // Define message type. Default is broadcast
    messageType type = messageType::BROADCAST;

    // Coordinates of the sender
    Coord senderPosition;
}