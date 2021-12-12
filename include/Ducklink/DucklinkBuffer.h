#ifndef DUCKLINKRECEIVER_H
#define DUCKLINKRECEIVER_H
#include "messages.pb.h"

namespace rd {
class DucklinkBuffer {
   public:
    DucklinkBuffer() : receiveState_(eReceiveState::START_1FF), expectedLen_(0), payloadRead_(0), computedChecksum_(0) {}

    inline bool pushNewOct(const uint8_t newOct, protoduck::Message& msg) {
        if (receiveState_ == eReceiveState::START_1FF) {
            if (newOct == 0xFF) {
                receiveState_ = eReceiveState::START_2FF;
            }
        } else if (receiveState_ == eReceiveState::START_2FF) {
            if (newOct == 0xFF) {
                receiveState_ = eReceiveState::LEN;
            } else {
                receiveState_ = eReceiveState::START_1FF;
            }
        } else if (receiveState_ == eReceiveState::LEN) {
            expectedLen_ = newOct;
            payloadRead_ = 0;
            computedChecksum_ = 0;
            receiveState_ = eReceiveState::PAYLOAD;
        } else if (receiveState_ == eReceiveState::PAYLOAD) {
            expectedLen_--;
            constructingPayload_[payloadRead_++] = newOct;
            computedChecksum_ ^= newOct;
            if (expectedLen_ == 0) {
                receiveState_ = eReceiveState::CHECKSUM;
            }
        } else if (receiveState_ == eReceiveState::CHECKSUM) {
            receiveState_ = eReceiveState::START_1FF;
            if (computedChecksum_ == newOct) {
                msg.Clear();
                if (msg.ParseFromArray(constructingPayload_, payloadRead_)) {
                    return true;
                }
            }
        }
        return false;
    }

   protected:
    enum eReceiveState { START_1FF, START_2FF, LEN, PAYLOAD, CHECKSUM };
    eReceiveState receiveState_;
    uint8_t expectedLen_;
    uint8_t payloadRead_;
    uint8_t computedChecksum_;
    uint8_t constructingPayload_[256];
};
}  // namespace rd

#endif /* DUCKLINKRECEIVER_H */
