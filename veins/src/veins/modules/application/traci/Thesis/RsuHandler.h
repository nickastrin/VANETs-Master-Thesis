#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include <string.h>

namespace veins 
{
    class VEINS_API RsuHandler : public DemoBaseApplLayer 
    {
        public:
            void initialize(int stage) override;

        protected:
            bool sentMessage;

            int currentSubscribedServiceId;
            double radioRange;

            std::string roadInfo;

        protected:
            void onWSM(BaseFrame1609_4* wsm) override;
            void onWSA(DemoServiceAdvertisment* wsa) override;

            void handleSelfMsg(cMessage* msg) override;
    };
}
