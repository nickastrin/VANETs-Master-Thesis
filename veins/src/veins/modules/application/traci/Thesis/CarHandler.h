#pragma once

#include "veins/modules/application/traci/Thesis/UnitHandler.h"
#include "veins/modules/application/traci/Thesis/Message_m.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace veins
{
    class VEINS_API CarHandler : public UnitHandler 
    {
        public:
            void initialize(int stage) override;

        protected:
            /**
             * @brief Position change handler
             * 
             * @param obj self object
             */
            void handlePositionUpdate(cObject* obj) override;

            void initializingMsg() override;
            void collectingMsg(Message *wsm) override;
            void writingMsg() override;

            void onWSA(DemoServiceAdvertisment* wsa) override;
            void onWSM(BaseFrame1609_4 *frame) override;

            void onBroadcast(Message *wsm) override;

            void onDegreeReply(Message *wsm) override;
            void onBetweennessReply(Message *wsm) override;
        
        protected:
            int currentSubscribedServiceId;
            bool sentMessage; 
            simtime_t lastDroveAt;
    };
}