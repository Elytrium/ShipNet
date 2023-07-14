#include "VersionedRegistry.hpp"

namespace Ship {
  VersionedRegistry::VersionedRegistry(const ProtocolVersions& versions, const std::set<ProtocolVersion>& versionMap)
    : versions(versions), versionToOrdinalMap(new uint32_t[versions.GetMaximumVersion().GetOrdinal() + 1]) {
    int ordinal = 0;

    auto previousIt = versionMap.begin();
    for (auto it = versionMap.begin()++; it != versionMap.end(); it++) {
      std::fill(versionToOrdinalMap + previousIt->GetOrdinal(), versionToOrdinalMap + it->GetOrdinal(), ordinal++);
      previousIt = it;
    }

    std::fill(versionToOrdinalMap + previousIt->GetOrdinal(), versionToOrdinalMap + versions.GetMaximumVersion().GetOrdinal() + 1, ordinal);
    versionRegistry = new VersionRegistry*[VersionToOrdinal(&versions.GetMaximumVersion()) + 1];
  }

  void VersionedRegistry::RegisterVersion(const ProtocolVersion* version, VersionRegistry* registry) {
    versionRegistry[VersionToOrdinal(version)] = registry;
  }

  void VersionedRegistry::FillVersionRegistry(VersionRegistry* registry) {
    for (uint32_t i = VersionToOrdinal(&versions.GetMinimumVersion()); i <= VersionToOrdinal(&versions.GetMaximumVersion()); ++i) {
      versionRegistry[i] = registry;
    }
  }

  Errorable<uint32_t> VersionedRegistry::GetOrdinalByID(const ProtocolVersion* version, uint32_t id) const {
    return versionRegistry[VersionToOrdinal(version)]->GetOrdinalByID(id);
  }

  Errorable<uint32_t> VersionedRegistry::GetIDByOrdinal(const ProtocolVersion* version, uint32_t ordinal) const {
    return versionRegistry[VersionToOrdinal(version)]->GetIDByOrdinal(ordinal);
  }

  uint32_t VersionedRegistry::VersionToOrdinal(const ProtocolVersion* version) const {
    if (version == &ProtocolVersion::UNKNOWN) {
      version = &versions.GetMaximumVersion();
    }

    return versionToOrdinalMap[version->GetOrdinal()];
  }
}