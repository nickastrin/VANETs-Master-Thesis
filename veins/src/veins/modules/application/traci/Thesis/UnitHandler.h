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

                // Message time variables
                simtime_t timestamp;
                simtime_t lastUsed;
                simtime_t receivedAt;

                int attempts;
                int usedFrequency;
                
                std::string roadData;        // Variable for LFU policy
                std::vector<long> rsu;  // Variable for betweenness

                MessageData(Message *wsm)
                {
                    id = wsm->getMsgId(); 

                    sender = wsm->getSenderAddress();
                    source = wsm->getSource();
                    dest = wsm->getDest();

                    timestamp = wsm->getCreationTime();
                    lastUsed = simTime();
                    receivedAt = simTime();

                    usedFrequency = 0;      
                    attempts = 0;           

                    roadData = wsm->getRoadData();
                }

                // Auxilary functions used for the sort function
                static bool sortFIFO(const MessageData &a, const MessageData &b)
                {
                    return a.receivedAt < b.receivedAt;
                }

                static bool sortLRU(const MessageData &a, const MessageData &b)
                {
                    return a.lastUsed < b.lastUsed;
                }

                static bool sortLFU(const MessageData &a, const MessageData &b)
                {
                    return a.usedFrequency < b.usedFrequency;
                }

                static bool sortRestore(const MessageData &a, const MessageData &b)
                {
                    return a.timestamp < b.timestamp;
                }
            };
        
            typedef std::deque<std::string> roadsDeque;
            typedef std::deque<MessageData> dataDeque;
            typedef std::map<long, std::vector<long>> routingDict;
        protected:
            /** @brief Handler for self messages */
            void handleSelfMsg(cMessage *msg) override;

            /** @brief Self message for initialization*/
            virtual void initializingMsg(Message *wsm){};

            /** @brief Handler for sending messages */
            void sendingMsg(Message *wsm);

            /** @brief Self message to request centralities */
            void requestingMsg(Message *wsm);

            /**
             * @brief Calculates centralities from collected messages
             * 
             * @param wsm message received
             * @param routing routing table
             * @param acks pending acknowledgements
             */
            void collectingMsg(
                Message *wsm,
                routingDict &routing,
                dataDeque &acks
                );

            /** @brief Repeats message if acknowledgement hasn't been received */
            void repeatingMsg(Message *wsm, dataDeque &acks);

             /**
             * @brief Handler for message transfer to candidate vector
             * 
             * @param limit time threshold to determine information validity
             */
            void cachingMsg(    
                Message* wsm,
                simtime_t limit, 
                dataDeque &messages, 
                dataDeque &candidates
                );

            /** @brief Function called when a BaseFrame1609_4 is received */
            void onWSM(BaseFrame1609_4* frame) override;

            /** @brief On broadcast handler */
            virtual void onBroadcast(Message *wsm, roadsDeque &roads){};

            /** @brief On request handler
             * 
             *  @param wsm message received
             *  @param roads blocked roads
             *  @param acks pending acknowledgements
            */
            void onRequest(
                Message *wsm,
                roadsDeque &roads,
                dataDeque &acks
                );

            /** @brief On reply handler*/
            void onReply(Message *wsm);

            /** @brief Handler for route request */
            void onRouteReq(
                Message *wsm, 
                routingDict &routing,
                dataDeque &acks
                );

            /** @brief Handler for route replies */
            void onRouteReply(Message *wsm, routingDict &routing);

            /** @brief Handler for centrality requests */
            void onCentralityReq(
                Message *wsm, 
                routingDict &routing,
                dataDeque &acks);

            /** @brief Degree request handler */
            void onDegreeReq(
                Message *wsm, 
                routingDict &routing, 
                dataDeque &acks);

            /** @brief Betweenness request handler */
            void onBetweennessReq(Message *wsm, dataDeque &acks);

            /** @brief Handler for centrality replies */
            void onCentralityReply(Message *wsm);

            /** @brief Degree reply handler */
            virtual void onDegreeReply(Message *wsm){};

            /** @brief Betweenness reply handler */
            virtual void onBetweennessReply(Message *wsm){};

            /** @brief Acknowledgement handler */
            void onAcknowledgement(Message *wsm, dataDeque &acks);

            /** @brief Auxilary routing functions */
            bool inRoute(std::vector<long> route);

            /** @brief Handler for accepting incoming messages */
            bool receiveMessage(
                Message *wsm,
                dataDeque &messages,
                dataDeque &candidates,
                routingDict &routing
                );

            /** @brief Updates routing table with new shortest paths*/
            bool routingTableUpdate(Message *wsm, routingDict &routing);

            /** @brief Auxilary function for routing table update*/
            bool auxilaryRoute(std::vector<long> &path, routingDict &routing);

            /** @brief Handler for insertion of accepted messages */
            bool insertMessage(
                Message *wsm, 
                dataDeque &messages,
                dataDeque &candidates
                );
            
            /** @brief Handler for flushing storage at max capacity */
            void flushList(dataDeque &messages, dataDeque &candidates);

            /** @brief Creates a request for message transfer to candidate vector */
            void thresholdControl();

        protected:
            // Centrality metrics variables 
            int degree;
            int closeness;
            int betweenness;

            // Variables for message transmission
            double signalRange;
            int ttl;

            // Variables for caching policy
            int capacity;
            int flushed;
            simtime_t threshold;

            bool calculating;

            // Variables for message storage
            dataDeque storedMessages;         // Messages stored in cache
            dataDeque candidateMessages;      // Older/Candidate messages for flushing
            dataDeque pendingAck;             // Messages pending acknowledgement

            // Variables for the routing table
            routingDict routingTable;
            simtime_t routingLastUpdated;
            simtime_t flushingInterval;

            //TODO: Check what to do with collectionInterval
            //simtime_t collectionInterval;       // Time until centrality calculation
            roadsDeque roadStatus;              // Stores roads with accidents

            // Variables for unit function
            CachingPolicy policy;
            UnitType unit;

    };
}