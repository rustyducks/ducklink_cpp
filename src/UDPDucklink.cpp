#include "Ducklink/UDPDucklink.h"

#include <unistd.h>

#include <cstring>

namespace rd {

UDPDucklinkBase::UDPDucklinkBase(const std::string& addr, const int port) : fPort_(port), fAddr_(addr) {
    char decimal_port[16];
    snprintf(decimal_port, sizeof(decimal_port), "%d", fPort_);
    decimal_port[sizeof(decimal_port) / sizeof(decimal_port[0]) - 1] = '\0';
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    int r(getaddrinfo(addr.c_str(), decimal_port, &hints, &fAddrinfo_));
    if (r != 0 || fAddrinfo_ == NULL) {
        throw std::runtime_error(("invalid address or port: \"" + addr + ":" + decimal_port + "\"").c_str());
    }
    fSocket_ = socket(fAddrinfo_->ai_family, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
    if (fSocket_ == -1) {
        freeaddrinfo(fAddrinfo_);
        throw std::runtime_error(("could not create socket for: \"" + addr + ":" + decimal_port + "\"").c_str());
    }
}

UDPDucklinkBase::~UDPDucklinkBase() {
    freeaddrinfo(fAddrinfo_);
    close(fSocket_);
}

UDPDucklinkSender::UDPDucklinkSender(const std::string& addr, const int port) : UDPDucklinkBase(addr, port) {}

template <>
int UDPDucklinkSender::send(const uint8_t* msg, size_t size) {
    return sendto(fSocket_, msg, size, 0, fAddrinfo_->ai_addr, fAddrinfo_->ai_addrlen);
}

template <>
int UDPDucklinkSender::send(const char* msg, size_t size) {
    return sendto(fSocket_, msg, size, 0, fAddrinfo_->ai_addr, fAddrinfo_->ai_addrlen);
}

int UDPDucklinkSender::sendMessage(const protoduck::Message& message) {
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

UDPDucklinkReceiver::UDPDucklinkReceiver(const std::string& addr, const int port) : UDPDucklinkBase(addr, port) {
    bind(fSocket_, fAddrinfo_->ai_addr, fAddrinfo_->ai_addrlen);
}

int UDPDucklinkReceiver::getMessages(std::vector<protoduck::Message>& messagesOut) {
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
