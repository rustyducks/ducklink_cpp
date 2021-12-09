#ifndef DUCKLINKBASE_H
#define DUCKLINKBASE_H

#include "generated/messages.pb.h"

namespace rd {
class DucklinkReceiverInterface {
   public:
    virtual int getMessages(std::vector<protoduck::Message>& outMessages) = 0;
};
class DucklinkSenderInterface {
   public:
    virtual int sendMessage(const protoduck::Message& message) = 0;
};
}  // namespace rd

#endif /* DUCKLINKBASE_H */
