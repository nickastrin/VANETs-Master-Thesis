#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/Thesis/Message_m.h"
#include <string.h>
#include <list>
#include <iterator>
#include <algorithm>

namespace veins
{
    class VEINS_API CarHandler : public DemoBaseApplLayer 
    {
        public:
            void initialize(int stage) override;

        protected:
            bool sentMessage;

            int currentSubscribedServiceId;
            int messagesToDelete;
            double radioRange;

            std::string roadInfo;
            std::list<Tuple> messageList;
            std::list<Tuple> distanceList;

            simtime_t lastDroveAt;
            cachingPolicy policy;
        
        protected:
            void onWSM(BaseFrame1609_4* wsm) override;
            void onWSA(DemoServiceAdvertisment* wsa) override;

            void handleSelfMsg(cMessage* msg) override;
            void sendingMessage(Message* wsm);
            void collectingMessage(Message* wsm);
            void cachingMessage();

            void cachingFIFO();
            void cachingLRU();
            void cachingLFU();

            void handlePositionUpdate(cObject* obj) override;

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
            void closenessReply(Message* wsm);
            void betweennessReply(Message* wsm);

            void requestInfo();
            void shortestPaths(Message* wsm);
            void mergeSort(std::list<Tuple>& array);
            void mergeLRU(std::list<Tuple> &array, std::list<Tuple> left, std::list<Tuple> right);
            void mergeLFU(std::list<Tuple> &array, std::list<Tuple> left, std::list<Tuple> right);
            bool acceptMessage(Message* wsm);
            bool inPath(std::list<long> path);
    };
}