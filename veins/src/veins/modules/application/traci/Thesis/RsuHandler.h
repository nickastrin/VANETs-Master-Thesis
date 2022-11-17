#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/Thesis/Message_m.h"
#include <string.h>
#include <list>
#include <iterator>

namespace veins 
{
    class VEINS_API RsuHandler : public DemoBaseApplLayer 
    {
        public:
            void initialize(int stage) override;

        protected:
            int degree;
            int closeness;
            int betweenness;

            double radioRange;
            
            std::string roadInfo;
            std::list<Tuple> messageList;               // List used for cached message storage
            std::list<Tuple> distanceList;            // List for the centrality calculation

        protected:
            void onWSM(BaseFrame1609_4* wsm) override;
            void onWSA(DemoServiceAdvertisment* wsa) override;

            void handleSelfMsg(cMessage* msg) override;
            void initializingMessage(Message* wsm);
            void sendingMessage(Message* wsm);
            void collectingMessage(Message* wsm);

            // Message Handlers
            void handleBroadcast(Message* wsm);
            void handleRequest(Message* wsm);
            void handleReply(Message* wsm);
            void handleRsuRequest(Message* wsm);
            void handleRsuReply(Message* wsm);
            void handleCentralityRequest(Message* wsm);
            void handleCentralityReply(Message* wsm);

            // Centrality Calculators
            void degreeRequest(Message* wsm);
            void closenessRequest(Message* wsm);
            void betweennessRequest(Message* wsm);

            // Rsu Reply Handlers
            void degreeReply(Message* wsm);
            void closenessReply(Message* wsm);
            void betweennessReply(Message* wsm);

            //void shortestPaths(Message* wsm);
            bool inPath(std::list<long> path);

            bool acceptMessage(Message* wsm);
            bool isDuplicate(Message* wsm);

    };
}
