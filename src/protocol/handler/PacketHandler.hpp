#pragma once

#include "../Protocol.hpp"
#include "../packet/Packet.hpp"
#include <functional>

#define SetPacketCallback(handlerClass, packetClass, callback)                                                                                   \
  SetPointerCallback(                                                                                                                            \
    handlerClass::HANDLER_ORDINAL, packetClass::PACKET_ORDINAL, [](PacketHandler* handlerPtr, void* connectionPtr, const PacketHolder& holder) { \
      ByteBuffer* buffer = holder.GetCurrentBuffer();                                                                                            \
      uint32_t oldReadableBytes = buffer->GetReadableBytes();                                                                                    \
      Errorable<packetClass> pkt = packetClass::Instantiate(holder);                                                                             \
                                                                                                                                                 \
      if (!pkt.IsSuccess()) {                                                                                                                    \
        /* TODO: Log Error */                                                                                                                    \
        return (Errorable<bool>) CanNotReadPacketErrorable(holder.GetExpectedSize());                                                            \
      }                                                                                                                                          \
                                                                                                                                                 \
      if (oldReadableBytes - buffer->GetReadableBytes() != holder.GetExpectedSize()) {                                                           \
        return (Errorable<bool>) InvalidPacketSizeErrorable(holder.GetExpectedSize());                                                           \
      }                                                                                                                                          \
                                                                                                                                                 \
      Connection* connection = (Connection*) connectionPtr;                                                                                      \
      handlerClass* handler = (handlerClass*) handlerPtr;                                                                                        \
      return handler->callback(connection, pkt.GetValue());                                                                                      \
    })

#define SetUnsafePacketCallback(handlerClass, packetClass, callback)                                                                             \
  SetPointerCallback(                                                                                                                            \
    handlerClass::HANDLER_ORDINAL, packetClass::PACKET_ORDINAL, [](PacketHandler* handlerPtr, void* connectionPtr, const PacketHolder& holder) { \
      ByteBuffer* buffer = holder.GetCurrentBuffer();                                                                                            \
      uint32_t oldReadableBytes = buffer->GetReadableBytes();                                                                                    \
      packetClass pkt = packetClass(holder);                                                                                                     \
                                                                                                                                                 \
      if (oldReadableBytes - buffer->GetReadableBytes() != holder.GetExpectedSize()) {                                                           \
        return (Errorable<bool>) InvalidPacketSizeErrorable(holder.GetExpectedSize());                                                           \
      }                                                                                                                                          \
                                                                                                                                                 \
      Connection* connection = (Connection*) connectionPtr;                                                                                      \
      handlerClass* handler = (handlerClass*) handlerPtr;                                                                                        \
      return handler->callback(connection, pkt);                                                                                                 \
    })

namespace Ship {
  CreateInvalidArgumentErrorable(InvalidPacketSizeErrorable, bool, "Invalid packet size");
  CreateInvalidArgumentErrorable(CanNotReadPacketErrorable, bool, "Can not read packet");

  class PacketHandler {
   private:
    static std::vector<std::vector<std::function<Errorable<bool>(PacketHandler*, void*, const PacketHolder&)>>> callbacks;

   public:
    virtual ~PacketHandler() = default;

    Errorable<bool> Handle(PacketHandler* handler_ptr, void* connection, const PacketHolder& packet);
    bool HasCallback(uint32_t ordinal) const;

    [[nodiscard]] virtual uint32_t GetOrdinal() const = 0;

    static void SetPointerCallback(
      uint32_t handler_ordinal, uint32_t packet_ordinal, std::function<Errorable<bool>(PacketHandler*, void*, const PacketHolder&)> callback);
  };
}
