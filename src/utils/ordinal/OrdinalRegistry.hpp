#pragma once

#include <cstdint>
#include <mutex>

namespace Ship {

  class OrdinalRegistry {
   private:
    uint32_t counter = 0;
    std::mutex mtx;

   public:
    static OrdinalRegistry PacketRegistry;
    static OrdinalRegistry ByteBytePipeRegistry;
    static OrdinalRegistry PacketHandlerRegistry;
    static OrdinalRegistry ErrorableTypeRegistry;

    uint32_t RegisterOrdinal();

    [[nodiscard]] uint32_t GetLastOrdinal() const;
  };

}
