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
            double radioRange;
            
            std::string roadInfo;
            std::list<Tuple> messageList;               // List used for cached message storage
            std::list<Tuple> collectionList;            // List for the centrality calculation

        protected:
            void onWSM(BaseFrame1609_4* wsm) override;
            void onWSA(DemoServiceAdvertisment* wsa) override;

            void handleSelfMsg(cMessage* msg) override;

            // Message Handlers
            void handleBroadcast(Message* wsm);
            void handleRequest(Message* wsm);
            void handleReply(Message* wsm);
            void handleRsuCheck(Message* wsm);
            void handleRsuReply(Message* wsm);

            // Centrality Calculators
            void degreeCentrality(Message* wsm);
            void closenessCentrality(Message* wsm);
            void betweennessCentrality(Message* wsm);

            // Rsu Reply Handlers
            void handleDegree(Message* wsm);
            void handleCloseness(Message* wsm);
            void handleBetweenness(Message* wsm);

            bool acceptMessage(Message* wsm);
            bool isDuplicate(Message* wsm);
            void requestCentrality(centralityType centrality);

            void collectMessages(int &counter);
            double calculateCloseness(int counter);
    };
}
