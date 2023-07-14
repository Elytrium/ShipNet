#include "Protocol.hpp"
#include <utility>

namespace Ship {
  ProtocolVersion::ProtocolVersion() : ordinal(UINT32_MAX), protocolID(UINT32_MAX), displayVersion("Unknown") {
  }

  const ProtocolVersion ProtocolVersion::UNKNOWN = ProtocolVersion();

  ProtocolVersions::ProtocolVersions(const std::list<ProtocolVersion>& versions)
    : minimumVersion(versions.front()), maximumVersion(versions.back()), minimumProtocolVersion(minimumVersion.GetProtocolID()),
      maximumProtocolVersion(maximumVersion.GetProtocolID()), minimumOrdinal(minimumVersion.GetOrdinal()), maximumOrdinal(maximumVersion.GetOrdinal()),
      idToVersionMap(new ProtocolVersion[maximumProtocolVersion + 1]), ordinalToVersionMap(new ProtocolVersion[maximumOrdinal + 1]) {
    for (const auto& version : versions) {
      idToVersionMap[version.GetProtocolID()] = version;
      ordinalToVersionMap[version.GetOrdinal()] = version;
    }
  }

  ProtocolVersion::ProtocolVersion(uint32_t ordinal, uint32_t protocol_id, std::string display_version)
    : ordinal(ordinal), protocolID(protocol_id), displayVersion(std::move(display_version)) {
  }

  const ProtocolVersion* ProtocolVersions::FromProtocolID(uint32_t protocol_id) const {
    if (protocol_id < minimumProtocolVersion || protocol_id > maximumProtocolVersion) {
      return &ProtocolVersion::UNKNOWN;
    } else {
      return idToVersionMap + protocol_id;
    }
  }

  const ProtocolVersion* ProtocolVersions::FromOrdinal(uint32_t ordinal) const {
    if (ordinal < minimumOrdinal || ordinal > maximumOrdinal) {
      return &ProtocolVersion::UNKNOWN;
    } else {
      return ordinalToVersionMap + ordinal;
    }
  }

  const ProtocolVersion& ProtocolVersions::GetMinimumVersion() const {
    return minimumVersion;
  }

  const ProtocolVersion& ProtocolVersions::GetMaximumVersion() const {
    return maximumVersion;
  }

  uint32_t ProtocolVersion::GetOrdinal() const {
    return ordinal;
  }

  uint32_t ProtocolVersion::GetProtocolID() const {
    return protocolID;
  }

  const std::string& ProtocolVersion::GetDisplayVersion() const {
    return displayVersion;
  }
}
