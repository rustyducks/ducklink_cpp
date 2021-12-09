#ifndef SERIALDUCKLINK_H
#define SERIALDUCKLINK_H

#include "Ducklink/DucklinkBase.h"
#include "Ducklink/DucklinkBuffer.h"
#include "serialib.h"

namespace rd {
class SerialDucklink : public DucklinkReceiverInterface, public DucklinkSenderInterface {
   public:
    SerialDucklink(const std::string& serialPath, const unsigned int baudrate);

    virtual int getMessages(std::vector<protoduck::Message>& messagesOut) override;
    virtual int sendMessage(const protoduck::Message& message) override;

   protected:
    serialib serial_;
    DucklinkBuffer ducklinkBuffer_;
};
}  // namespace rd

#endif /* SERIALDUCKLINK_H */
