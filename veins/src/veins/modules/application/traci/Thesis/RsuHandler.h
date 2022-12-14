#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/Thesis/Message_m.h"

#include <iterator>
#include <list>

namespace veins
{    
    class VEINS_API RsuHandler : public DemoBaseApplLayer 
    {
        public:
            void initialize(int stage) override;

        private:
            int degree;
            int closeness;
            int betweenness;

            int capacity;
            int flushed;
            simtime_t threshold;

            double signalRange;
            int maxHops;

            cachingPolicy policy;
            scenario testCase;

            std::list<Tuple> messageList;
            std::list<Tuple> candidateList;
            std::list<Tuple> distanceList;
            std::list<Tuple> ackList;

        private:
            void testScenario(scenario test);

            void handleSelfMsg(cMessage * msg) override;
            void initializingMessage(Message * wsm);
            void sendingMessage(Message * wsm);
            void collectingMessage(Message * wsm, std::list<Tuple> & array, std::list<Tuple> & ackArray);
            void repeatingMessage(Message * wsm, std::list<Tuple> & array);

            void onWSA(DemoServiceAdvertisment* wsa) override;
            void onWSM(BaseFrame1609_4* frame) override;

            void handleBroadcast(Message * wsm);
            void handleRequest(Message * wsm, std::list<Tuple> & msgArray, std::list<Tuple> & cndArray, std::list<Tuple> & ackArray);
            void auxRequestHandler(Message * msg, std::list<Tuple> & array, std::list<Tuple> & ackArray);
            void handleReply(Message * wsm);

            void handleRsuRequest(Message * wsm, std::list<Tuple> & ackArray);
            void degreeRequest(Message * msg, std::list<Tuple> & ackArray);
            void closenessRequest(Message * msg, std::list<Tuple> & ackArray);
            void betweennessRequest(Message * msg);

            void handleCentralityRequest(Message * wsm, std::list<Tuple> & ackArray);
            void handleCentralityReply(Message * wsm, std::list<Tuple> & array);

            void handleRsuReply(Message* wsm, std::list<Tuple> & ackArray);
            void degreeReply(Message* wsm);
            void closenessReply(Message* wsm, std::list<Tuple> & array);
            void betweennessReply(Message* wsm);

            void handleAcknowledgement(Message * wsm, std::list<Tuple> & ackArray);

            bool inPath(std::list<long> path);

            bool receiveMessage(Message * msg, std::list<Tuple> & msgArray, std::list<Tuple> & candArray);
            void flushList(std::list<Tuple> & msgArray, std::list<Tuple> & candArray);
            bool insertMessage(Message * msg, std::list<Tuple> & msgArray, std::list<Tuple> & candArray);

            void mergeSort(std::list<Tuple> & array, bool restore = false);
            void mergeRestore(std::list<Tuple> & array, std::list<Tuple> left, std::list<Tuple> right);
            void mergeFIFO(std::list<Tuple> & array, std::list<Tuple> left, std::list<Tuple> right);
            void mergeLRU(std::list<Tuple> & array, std::list<Tuple> left, std::list<Tuple> right);
            void mergeLFU(std::list<Tuple> & array, std::list<Tuple> left, std::list<Tuple> right);
            
            void thresholdRequest();
            void thresholdControl(simtime_t time, std::list<Tuple> & msgArray, std::list<Tuple> & candArray);
    };
}