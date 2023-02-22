#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/Thesis/Message_m.h"
#include "veins/modules/application/traci/Thesis/Enums.h"

#include <string.h>
#include <iterator>
#include <vector>
#include <deque>
#include <map>

namespace veins
{
    class VEINS_API UnitHandler : public DemoBaseApplLayer
    {
        public:
            struct MessageData
            {
                // Struct for storing necessary message data
                long id;                // Message Id 

                // Message route variables
                long sender;
                long source;
                long dest;

                std::string contentId;
                std::string content;
                int segments;
                int segmentNumber;
                bool multimedia;

                // Message time variables
                simtime_t timestamp;
                simtime_t lastUsed;
                simtime_t receivedAt;

                int attempts;
                int usedFrequency;

                MessageType type;
                
                MessageData(Message *wsm)
                {
                    id = wsm->getMsgId(); 

                    sender = wsm->getSenderAddress();
                    source = wsm->getSource();
                    dest = wsm->getDest();

                    contentId = wsm->getContentId();
                    content = wsm->getContent();
                    segments = wsm->getSegments();
                    segmentNumber = wsm->getSegmentNumber();
                    multimedia = wsm->getMultimedia();

                    timestamp = wsm->getCreationTime();
                    lastUsed = simTime();
                    receivedAt = simTime();

                    usedFrequency = 0;      
                    attempts = 0;           

                    type = wsm->getType();
                }

                // Auxilary functions used for the sort function
                static bool sortFIFO(const MessageData &a, const MessageData &b)
                {
                    return a.receivedAt < b.receivedAt;
                }

                static bool sortLRU(const MessageData &a, const MessageData &b)
                {
                    return a.lastUsed > b.lastUsed;
                }

                static bool sortLFU(const MessageData &a, const MessageData &b)
                {
                    return a.usedFrequency > b.usedFrequency;
                }

                static bool sortRestore(const MessageData &a, const MessageData &b)
                {
                    return a.timestamp > b.timestamp;
                }
            };
        
            struct ContentWrapper
            {
                int segments;
                int segmentNumber;
                std::string content;
                simtime_t timestamp;

                ContentWrapper() {};
                ContentWrapper(Message *wsm)
                {
                    segments = wsm->getSegments();
                    segmentNumber = wsm->getSegmentNumber();
                    content = wsm->getContent();
                    timestamp = wsm->getCreationTime();
                }
            };

            typedef std::deque<long> pathDeque;
            typedef std::deque<std::string> roadsDeque;
            typedef std::deque<MessageData> dataDeque;
            typedef std::map<long, std::deque<long>> routingDict;

            typedef std::map<std::string, ContentWrapper> storageDict;
        protected:
            void handleSelfMsg(cMessage *msg) override;
            virtual void initializingMsg() {};
            void sendingMsg(Message *wsm);
            virtual void requestingMsg() {};
            virtual void collectingMsg(Message *wsm);
            void repeatingMsg(Message *wsm);
            void cachingMsg(Message *wsm, simtime_t limit);
            virtual void writingMsg() {};
            virtual void extractingMsg() {};
            
            virtual void onWSM(BaseFrame1609_4 *frame) override;

            virtual void onOriginInitReq(Message *wsm) {};
            virtual void onOriginInitReply(Message *wsm) {};
            virtual void onRsuInitReq(Message *wsm) {};
            virtual void onRsuInitReply(Message *wsm) {};

            virtual void onOriginCentralityReq(Message *wsm) {};
            virtual void onOriginCentralityReply(Message *wsm) {};

            virtual void onPullReq(Message *wsm) {};
            virtual void onPullReply(Message *wsm) {};

            virtual void onPushML(Message *wsm) {};
            virtual void onPushCentrality(Message *wsm) {};
            virtual void onPushContent(Message *wsm);

            virtual void onBroadcast(Message *wsm) {};
            virtual void onRequest(Message *wsm) {};
            void onReply(Message *wsm);

            void onRouteReq(Message *wsm);
            void onRouteReply(Message *wsm);

            void onCentralityReq(Message *wsm);
            void onDegreeReq(Message *wsm);
            void onBetweennessReq(Message *wsm);

            void onCentralityReply(Message *wsm);
            virtual void onDegreeReply(Message *wsm) {};
            virtual void onBetweennessReply(Message *wsm) {};

            void onAcknowledgement(Message *wsm);

            bool receiveMessage(Message *wsm, routingDict &routing);
            bool routingTableUpdate(pathDeque path, routingDict &routing);
            bool insertMessage(Message *wsm);
            bool messageMatch(Message *wsm, MessageData data);
            bool insertSegmented(Message *wsm);
            bool contentMatch(Message *wsm, MessageData data);
            void sendAcknowledgement(Message *wsm);
            void flushList();
            
            void thresholdControl();

            bool inRoute(pathDeque route);
            void createRouteReq(MessageType type, int ttl, bool origin = false, bool update = true);
            void handleRouteReq(Message *wsm, MessageType type);
            void handleRouteTraversal(Message *wsm, bool forward = false);

            bool contentSearch(Message *wsm, storageDict storage, bool origin = false);
            std::string extractContent(Message *wsm);

            void debugPrint(routingDict routing);
        protected:
            // Centrality metrics variables 
            float degree;
            float closeness;
            float betweenness;

            // Variables for message transmission
            double signalRange;
            int maxHops = 50;

            // Variables for caching policy
            int capacity;
            int flushed;
            simtime_t threshold;

            simtime_t lastUpdated;

            pathDeque requestedBy;           // Vector that stores RSU IDs that requested betweenness

            // Variables for message storage
            dataDeque storedMessages;         // Messages stored in cache
            dataDeque candidateMessages;      // Older/Candidate messages for flushing
            dataDeque pendingAck;             // Messages pending acknowledgement

            // Variables for the routing table
            routingDict routingTable;
            routingDict rsuRouting;
            simtime_t routingLastUpdated;
            simtime_t flushingInterval;

            //TODO: Check what to do with collectionInterval
            //simtime_t collectionInterval;       // Time until centrality calculation
            roadsDeque roadStatus;                // Stores roads with accidents

            // Variables for unit function
            CachingPolicy caching;
            OriginPolicy origin;
            UnitType unit;

            storageDict multimediaData;
            storageDict roadData;

            dataDeque segmentedMessages;

            simtime_t msgTime;
    };
}