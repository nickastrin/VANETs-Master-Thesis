#pragma once

#include "veins/modules/application/traci/Thesis/UnitHandler.h"
#include "veins/modules/application/traci/Thesis/Message_m.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

namespace veins
{
    class VEINS_API OriginHandler : public UnitHandler
    {
        public:
            void initialize(int stage) override;
        protected:
            void initializingMsg() override;
            void requestingMsg() override;
            void collectingMsg(Message *wsm) override;
            void extractingMsg() override;

            void onWSM(BaseFrame1609_4 *frame) override;
            void onRsuInitReq(Message *wsm) override;
            void onRsuInitReply(Message *wsm) override;

            void onOriginCentralityReply(Message *wsm) override;

            void onPullReq(Message *wsm) override;
            void onPullReply(Message *wsm) override;
            void onPushContent(Message *wsm) override;

            void createPushContent();
            void sendContent(Message *wsm, storageDict &storage, bool multimedia, float centralDelay);
            void initializeVariables();
        protected:
            CentralityType centrality;
            Scenario simulation;

            int rsuCount;
            long pushRsu;
            float highestCentrality;

            std::map<long, Coord> rsuPositions;
            std::vector<long> centralVec;
    };
}