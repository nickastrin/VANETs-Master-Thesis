import veins.base.utils.Coord;
import veins.modules.messages.BaseFrame1609_4;
import veins.base.utils.SimpleAddress;

namespace veins;

message CarMessage extends BaseFrame1609_4 
{
    // If requesting info
    bool request = false;

    // Message TTL
    int hopCount = 0;

    string demoData;

    // Address of sender. Default is broadcast
    LAddress::L2Type senderAddress = -1;

    // Coordinates of the sender
    Coord senderPosition;
}