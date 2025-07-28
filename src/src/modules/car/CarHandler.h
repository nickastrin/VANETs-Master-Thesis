#pragma once

#include "../unit/UnitHandler.h"
#include "../message/Message_m.h"
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
            void onRequest(Message *wsm) override;

            void onDegreeReply(Message *wsm) override;
            void onBetweennessReply(Message *wsm) override;
        
            void createRequest(std::string contentId, bool multimedia, simtime_t time);
        protected:
            int currentSubscribedServiceId;
            bool sentMessage; 
            simtime_t lastDroveAt;
            simtime_t tstamp;
    };
}