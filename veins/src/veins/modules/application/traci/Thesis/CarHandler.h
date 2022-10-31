#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include <string.h>

namespace veins 
{
    class VEINS_API CarHandler : public DemoBaseApplLayer 
    {
        public:
            void initialize(int stage) override;

        //TODO: This might be useless. Remove if it is.
        private:
            struct VehiclePositions
            {
                int vehicleId;
                double distance;

                VehiclePositions(int id, double dist)
                {
                    vehicleId = id;
                    distance = dist;
                }
            };

        protected:
            bool sentMessage;
            //bool hasInfo;

            int currentSubscribedServiceId;
            double radioRange;

            std::string roadInfo;

            //std::list<VehiclePositions> distanceList;
            simtime_t lastDroveAt;

        protected:
            void onWSM(BaseFrame1609_4* wsm) override;
            void onWSA(DemoServiceAdvertisment* wsa) override;

            void handleSelfMsg(cMessage* msg) override;
            void handlePositionUpdate(cObject* obj) override;

            void requestInfo();
    };
}
