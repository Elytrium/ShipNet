#pragma once

#include "../../../utils/exception/Errorable.hpp"
#include <string>

namespace Ship {
  class UUID {
   private:
    uint64_t mostSignificant;
    uint64_t leastSignificant;

   public:
    UUID() = default;

    static Errorable<UUID> Instantiate(const std::string& uuid);

    UUID(uint64_t mostSignificant, uint64_t leastSignificant);

    [[nodiscard]] uint64_t GetMostSignificant() const;

    [[nodiscard]] uint64_t GetLeastSignificant() const;

    [[nodiscard]] std::string ToUndashedString() const;

    [[nodiscard]] std::string ToString() const;
  };

  CreateInvalidArgumentErrorable(InvalidUUIDSizeErrorable, UUID, "Invalid UUID size");
}
