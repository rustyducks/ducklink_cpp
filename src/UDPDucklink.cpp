#include "Ducklink/UDPDucklink.h"

#include <unistd.h>

#include <cstring>

namespace rd {

UDPDucklinkBase::UDPDucklinkBase(const std::string& addr, const int port) : fPort_(port), fAddr_(addr) {
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(addr.c_str());
    fSocket_ = socket(servaddr.sin_family, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
    if (fSocket_ == -1) {
        throw std::runtime_error(("could not create socket for: \"" + addr + "\"").c_str());
    }
}

UDPDucklinkBase::~UDPDucklinkBase() {
    close(fSocket_);
}

UDPDucklinkClient::UDPDucklinkClient(const std::string& addr, const int port) : UDPDucklinkBase(addr, port) {}

template <>
int UDPDucklinkClient::send(const uint8_t* msg, size_t size) {
    return sendto(fSocket_, msg, size, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
}

template <>
int UDPDucklinkClient::send(const char* msg, size_t size) {
    return sendto(fSocket_, msg, size, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
}

int UDPDucklinkClient::sendMessage(const protoduck::Message& message) {
    std::vector<uint8_t> toSend(message.ByteSize() + 3 + 1);
    toSend.at(0) = 0xFF;
    toSend.at(1) = 0xFF;
    toSend.at(2) = message.ByteSize();
    uint8_t checksum = 0;
    message.SerializeToArray(toSend.data() + 3, message.ByteSize());
    for (size_t i = 3; i < toSend.size() - 1; i++) {
        checksum ^= toSend[i];
    }
    toSend.back() = checksum;
    return send(toSend.data(), toSend.size());
}

int UDPDucklinkClient::getMessages(std::vector<protoduck::Message>& messagesOut) {
    uint8_t in[256];
    protoduck::Message msg;
    messagesOut.clear();
    int nbRead = 0;
    while ((nbRead = recv(fSocket_, in, 256, 0)) > 0) {
        for (int i = 0; i < nbRead; i++) {
            if (ducklinkBuffer_.pushNewOct(in[i], msg)) {
                messagesOut.push_back(msg);
            }
        }
    }
    return messagesOut.size();
}

UDPDucklinkServer::UDPDucklinkServer(const std::string& addr, const int port) : UDPDucklinkBase(addr, port) {
    bind(fSocket_, (struct sockaddr *)&servaddr, sizeof(servaddr));
}

int UDPDucklinkServer::getMessages(std::vector<protoduck::Message>& messagesOut) {
    uint8_t in[256];
    protoduck::Message msg;
    messagesOut.clear();
    int nbRead = 0;
    while ((nbRead = recv(fSocket_, in, 256, 0)) > 0) {
        for (int i = 0; i < nbRead; i++) {
            if (ducklinkBuffer_.pushNewOct(in[i], msg)) {
                messagesOut.push_back(msg);
            }
        }
    }
    return messagesOut.size();
}
}  // namespace rd
