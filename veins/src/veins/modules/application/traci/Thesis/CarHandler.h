#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/Thesis/Message_m.h"
#include <string.h>
#include <list>
#include <iterator>

namespace veins
{
    class VEINS_API CarHandler : public DemoBaseApplLayer 
    {
        public:
            void initialize(int stage) override;

        protected:
            bool sentMessage;

            int currentSubscribedServiceId;
            double radioRange;

            std::string roadInfo;
            std::list<Tuple> messageList;

            simtime_t lastDroveAt;
        
        protected:
            void onWSM(BaseFrame1609_4* wsm) override;
            void onWSA(DemoServiceAdvertisment* wsa) override;

            void handleSelfMsg(cMessage* msg) override;
            void handlePositionUpdate(cObject* obj) override;

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
            void handleCloseness(Message* wsm);
            void handleBetweenness(Message* wsm);

            void requestInfo();
            bool acceptMessage(Message* wsm);
            bool isDuplicate(Message* wsm);
    };
}