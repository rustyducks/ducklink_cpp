#ifndef UDPDUCKLINK_H
#define UDPDUCKLINK_H

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "Ducklink/DucklinkBase.h"
#include "Ducklink/DucklinkBuffer.h"

namespace rd {

class UDPDucklinkBase {
   public:
    ~UDPDucklinkBase();

    int getSocket() const { return fSocket_; }
    int getPort() const { return fPort_; }
    std::string getAddr() const { return fAddr_; }

   protected:
    UDPDucklinkBase(const std::string &addr, const int port);
    int fSocket_;
    int fPort_;
    std::string fAddr_;
    struct sockaddr_in servaddr;
};

class UDPDucklinkClient : public UDPDucklinkBase, public DucklinkSenderInterface, public DucklinkReceiverInterface {
   public:
    UDPDucklinkClient(const std::string &addr, const int port);
    virtual int sendMessage(const protoduck::Message &message) override;
    virtual int getMessages(std::vector<protoduck::Message> &messagesOut) override;

   protected:
    template <typename T>
    int send(const T *msg, size_t size);
    DucklinkBuffer ducklinkBuffer_;
};

class UDPDucklinkServer: public UDPDucklinkBase, public DucklinkReceiverInterface {
   public:
    UDPDucklinkServer(const std::string &addr, const int port);
    virtual int getMessages(std::vector<protoduck::Message> &messagesOut) override;

   protected:
    DucklinkBuffer ducklinkBuffer_;
};
}  // namespace rd

#endif /* UDPDUCKLINK_H */
