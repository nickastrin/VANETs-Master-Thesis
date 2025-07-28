#pragma once

#include "../unit/UnitHandler.h"
#include "../message/Message_m.h"

namespace veins
{
    class VEINS_API RsuHandler : public UnitHandler 
    {
        public:
            void initialize(int stage) override;
        
        protected:
            void collectingMsg(Message *wsm) override;
            /**
             * @brief On WSA handler
             * 
             * @param wsa Service advertisement message
             */
            void onWSA(DemoServiceAdvertisment *wsa) override;
            void onWSM(BaseFrame1609_4 *frame) override;

            void onOriginInitReq(Message *wsm) override;
            void onOriginInitReply(Message *wsm) override;
            void onRsuInitReq(Message *wsm) override;
            void onRsuInitReply(Message *wsm) override;

            void onOriginCentralityReq(Message *wsm) override;
            void onOriginCentralityReply(Message *wsm) override;

            void onPullReq(Message *wsm) override;
            void onPullReply(Message *wsm) override;

            void onPushML(Message * wsm) override;
            void onPushCentrality(Message *wsm) override;
            
            void onBroadcast(Message *wsm) override;
            void onRequest(Message *wsm) override;

            void onDegreeReply(Message *wsm) override;
            void onBetweennessReply(Message *wsm) override;

            void createPullReq(std::string contentId, long source, 
                long dest, bool multimedia);
            void initializeVariables();
        
        protected:
            CentralityType centrality;

            int rsuCount;
            long originIp;
            long pushRsu;
            float highestCentrality;

            dataDeque pendingReply;
            bool temp = false;
            int idtmp = 0;
            std::vector<long> centralVec;
            simtime_t lastUpdateRsu = 0;
    };
}