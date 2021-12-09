#include "Ducklink/SerialDucklink.h"

namespace rd {
SerialDucklink::SerialDucklink(const std::string& serialPath, const unsigned int baudrate) {
    if (serial_.openDevice(serialPath.c_str(), baudrate) != 1) {
        throw std::runtime_error("Cannot open serial");
    }
}

int SerialDucklink::getMessages(std::vector<protoduck::Message>& messagesOut) {
    messagesOut.clear();
    protoduck::Message msg;
    char rcvChar;
    uint8_t rcv;
    while (serial_.available()) {
        if (serial_.readChar(&rcvChar) == 1) {
            rcv = static_cast<uint8_t>(rcvChar);
            if (ducklinkBuffer_.pushNewOct(rcv, msg)) {
                // A message is complete
                messagesOut.push_back(msg);
            }
        }
    }
    return messagesOut.size();
}

int SerialDucklink::sendMessage(const protoduck::Message& message) {
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
    return serial_.writeBytes(toSend.data(), toSend.size());
}
}  // namespace rd
