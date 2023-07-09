#pragma once

#include "VersionRegistry.hpp"
#include "VersionedRegistry.hpp"
#include <functional>

namespace Ship {

  template<typename T>
  inline std::function<T*(const ProtocolVersion* version, ByteBuffer* buffer)> CreateConstructor() {
    return [=](const ProtocolVersion* version, ByteBuffer* buffer) {
      return new T(version, buffer);
    };
  }

  template<typename T>
  class NoConstructorExistErrorable : public Errorable<T> {
   public:
    static inline const uint32_t TYPE_ORDINAL = OrdinalRegistry::ErrorableTypeRegistry.RegisterOrdinal();

    explicit NoConstructorExistErrorable(uint32_t ordinal) : Errorable<T>(TYPE_ORDINAL, {}, ordinal) {
    }

    void Print(std::ostream o) {
      o << "No constructor exist, packet ordinal: " << this->GetErrorCode();
    }
  };

  template<typename T>
  class NoConstructorOrdinalExistErrorable : public Errorable<T> {
   public:
    static inline const uint32_t TYPE_ORDINAL = OrdinalRegistry::ErrorableTypeRegistry.RegisterOrdinal();

    explicit NoConstructorOrdinalExistErrorable(uint32_t id) : Errorable<T>(TYPE_ORDINAL, {}, id) {
    }

    void Print(std::ostream o) {
      o << "No constructor exist, packet id: " << this->GetErrorCode();
    }
  };

  template<typename T>
  class ConstructorRegistry : public VersionedRegistry {
   private:
    std::vector<std::function<T*(const ProtocolVersion* version, ByteBuffer* buffer)>> ordinalToObjectMap;

   public:
    explicit ConstructorRegistry(const ProtocolVersions& versions, const std::set<ProtocolVersion>& versionMap) : VersionedRegistry(versions, versionMap) {}

    void RegisterConstructor(uint32_t ordinal, const std::function<T*(const ProtocolVersion* version, ByteBuffer* buffer)>& constructor) {
      OrdinalVector::ResizeVectorAndSet(ordinalToObjectMap, ordinal, constructor);
    }

    Errorable<T*> GetObjectByID(const ProtocolVersion* version, uint32_t id, ByteBuffer* buffer) const {
      ProceedErrorable(ordinal, uint32_t, GetOrdinalByID(version, id), NoConstructorOrdinalExistErrorable<T*>(id))
        if (ordinal >= ordinalToObjectMap.size()) {
        return NoConstructorExistErrorable<T*>(ordinal);
      }

      std::function<T*(const ProtocolVersion* version, ByteBuffer* buffer)> constructor = ordinalToObjectMap[ordinal];
      if (constructor) {
        return SuccessErrorable<T*>(constructor(version, buffer));
      } else {
        return NoConstructorExistErrorable<T*>(ordinal);
      }
    }

    [[nodiscard]] size_t GetConstructorCount() const {
      return ordinalToObjectMap.size();
    }
  };
}