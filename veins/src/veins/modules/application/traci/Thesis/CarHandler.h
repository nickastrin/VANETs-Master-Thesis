#pragma once

#include "veins/modules/application/traci/Thesis/UnitHandler.h"
#include "veins/modules/application/traci/Thesis/Message_m.h"

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

            /**
             * @brief On WSA handler
             * 
             * @param wsa Service advertisement message
             */
            void onWSA(DemoServiceAdvertisment* wsa) override;
            void onBroadcast(Message *wsm, roadsDeque &roads) override;
            void onDegreeReply(Message *wsm) override;
            void onBetweennessReply(Message *wsm) override;
        
        protected:
            int currentSubscribedServiceId;
            bool sentMessage; 
            simtime_t lastDroveAt;  
    };
}