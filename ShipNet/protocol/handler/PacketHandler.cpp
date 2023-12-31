#include "PacketHandler.hpp"
#include "../../utils/ordinal/OrdinalVector.hpp"
#include <utility>

namespace Ship {
  auto PacketHandler::callbacks = std::vector<std::vector<std::function<Errorable<bool>(PacketHandler*, void*, const PacketHolder&)>>>();

  Errorable<bool> PacketHandler::Handle(PacketHandler* handler, void* connection, const PacketHolder& packet) {
    if (GetOrdinal() >= callbacks.size()) {
      return SuccessErrorable<bool>(false);
    }

    const auto& localCallbacks = callbacks[GetOrdinal()];
    uint32_t ordinal = packet.GetOrdinal();
    if (ordinal >= localCallbacks.size()) {
      return SuccessErrorable<bool>(false);
    }

    std::function<Errorable<bool>(PacketHandler*, void*, const PacketHolder&)> callback = localCallbacks[ordinal];
    if (callback) {
      return callback(handler, connection, packet);
    } else {
      return SuccessErrorable<bool>(false);
    }
  }

  bool PacketHandler::HasCallback(uint32_t ordinal) const {
    if (callbacks.size() >= GetOrdinal()) {
      return false;
    }

    const auto& localCallbacks = callbacks[GetOrdinal()];
    return ordinal < localCallbacks.size() && localCallbacks[ordinal];
  }

  void PacketHandler::SetPointerCallback(
    uint32_t handler_ordinal, uint32_t packet_ordinal, std::function<Errorable<bool>(PacketHandler*, void*, const PacketHolder&)> callback) {
    if (callbacks.size() >= handler_ordinal) {
      callbacks.resize(handler_ordinal + 8);
    }

    OrdinalVector::ResizeVectorAndSet(callbacks[handler_ordinal], packet_ordinal, std::move(callback));
  }
}