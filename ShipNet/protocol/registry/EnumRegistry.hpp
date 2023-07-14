#pragma once

#include "VersionedRegistry.hpp"

namespace Ship {

  template<typename T>
  CreateInvalidArgumentErrorable(InvalidEnumByIdErrorable, T, "Invalid (unregistered) enum by id");

  template<typename T>
  class EnumRegistry : public VersionedRegistry {
   public:
    explicit EnumRegistry(const ProtocolVersions& versions, const std::set<ProtocolVersion>& versionMap) : VersionedRegistry(versions, versionMap) {}

    Errorable<T> GetValue(const ProtocolVersion* version, uint32_t id) const {
      ProceedErrorable(ordinal, uint32_t, GetOrdinalByID(version, id), InvalidEnumByIdErrorable<T>(id))
      return SuccessErrorable<T>((T) ordinal);
    }

    Errorable<uint32_t> GetID(const ProtocolVersion* version, const T& value) const {
      return GetIDByOrdinal(version, (uint32_t) value);
    }
  };
}