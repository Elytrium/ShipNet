#pragma once

#include "VersionRegistry.hpp"
#include <set>

namespace Ship {
  class VersionedRegistry {
   private:
    const ProtocolVersions& versions;
    uint32_t* versionToOrdinalMap;
    VersionRegistry** versionRegistry;

   public:
    explicit VersionedRegistry(const ProtocolVersions& versions, const std::set<ProtocolVersion>& versionMap);

    virtual ~VersionedRegistry() {
      if (versionRegistry) {
        for (uint32_t i = 0; i < VersionToOrdinal(&versions.GetMaximumVersion()) + 1; ++i) {
          delete versionRegistry[i];
        }
      }

      delete[] versionRegistry;
    }

    uint32_t VersionToOrdinal(const ProtocolVersion* version) const;
    uint32_t GetOrdinalByID(const ProtocolVersion* version, uint32_t id) const;
    uint32_t GetIDByOrdinal(const ProtocolVersion* version, uint32_t ordinal) const;
    void RegisterVersion(const ProtocolVersion* version, VersionRegistry* registry);
    void FillVersionRegistry(VersionRegistry* registry);
  };
}
