#include "VersionRegistry.hpp"
#include "../../utils/ordinal/OrdinalVector.hpp"

namespace Ship {

  VersionRegistry::VersionRegistry(const std::vector<uint32_t>& ordinals) {
    idToOrdinalMap = ordinals;

    uint32_t index = 0;
    for (uint32_t ordinal : ordinals) {
      OrdinalVector::ResizeVectorAndSet(ordinalToIDMap, (uint32_t) ordinal, index++);
    }
  }

  Errorable<uint32_t> VersionRegistry::GetOrdinalByID(uint32_t id) const {
    if (id >= idToOrdinalMap.size()) {
      return InvalidPacketById(id);
    }

    return SuccessErrorable<uint32_t>(idToOrdinalMap[id]);
  }

  Errorable<uint32_t> VersionRegistry::GetIDByOrdinal(uint32_t ordinal) const {
    if (ordinal >= ordinalToIDMap.size()) {
      return InvalidPacketByOrdinal(ordinal);
    }

    return SuccessErrorable<uint32_t>(ordinalToIDMap[ordinal]);
  }

  void VersionRegistry::Register(uint32_t ordinal) {
    Register(ordinal, ++latestRegisteredID);
  }

  void VersionRegistry::Register(uint32_t ordinal, uint32_t id) {
    OrdinalVector::ResizeVectorAndSet(ordinalToIDMap, ordinal, id);
    OrdinalVector::ResizeVectorAndSet(idToOrdinalMap, id, ordinal);
  }
}