#pragma once

#include "VersionRegistry.hpp"
#include "VersionedRegistry.hpp"
#include <functional>

namespace Ship {

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
  class InvalidConstructorErrorable : public Errorable<T> {
   public:
    static inline const uint32_t TYPE_ORDINAL = OrdinalRegistry::ErrorableTypeRegistry.RegisterOrdinal();

    explicit InvalidConstructorErrorable() : Errorable<T>(TYPE_ORDINAL, {}, 0) {
    }

    void Print(std::ostream o) {
      o << "Invalid constructor for " << typeid(T).name();
    }
  };

  template<typename T, typename R>
  inline std::function<Errorable<T*>(const ProtocolVersion* version, ByteBuffer* buffer)> CreateConstructor() {
    return [=](const ProtocolVersion* version, ByteBuffer* buffer) {
      return SuccessErrorable<T*>((T*) new R(version, buffer));
    };
  }

  template<typename T, typename R>
  inline std::function<Errorable<T*>(const ProtocolVersion* version, ByteBuffer* buffer)> WrapConstructor() {
    return [=](const ProtocolVersion* version, ByteBuffer* buffer) {
      ProceedErrorable(object, R, R::Instantiate(version, buffer), (Errorable<T*>) InvalidConstructorErrorable<T*>())
      return (Errorable<T*>) SuccessErrorable<T*>((T*) &object);
    };
  }

  template<typename T>
  class ConstructorRegistry : public VersionedRegistry {
   private:
    std::vector<std::function<Errorable<T*>(const ProtocolVersion* version, ByteBuffer* buffer)>> ordinalToObjectMap;

   public:
    explicit ConstructorRegistry(const ProtocolVersions& versions, const std::set<ProtocolVersion>& versionMap) : VersionedRegistry(versions, versionMap) {}

    void RegisterConstructor(uint32_t ordinal, const std::function<Errorable<T*>(const ProtocolVersion* version, ByteBuffer* buffer)>& constructor) {
      OrdinalVector::ResizeVectorAndSet(ordinalToObjectMap, ordinal, constructor);
    }

    Errorable<T*> GetObjectByID(const ProtocolVersion* version, uint32_t id, ByteBuffer* buffer) const {
      ProceedErrorable(ordinal, uint32_t, GetOrdinalByID(version, id), NoConstructorOrdinalExistErrorable<T*>(id))
      if (ordinal >= ordinalToObjectMap.size()) {
        return NoConstructorExistErrorable<T*>(ordinal);
      }

      std::function<Errorable<T*>(const ProtocolVersion* version, ByteBuffer* buffer)> constructor = ordinalToObjectMap[ordinal];
      if (constructor) {
        return constructor(version, buffer);
      } else {
        return NoConstructorExistErrorable<T*>(ordinal);
      }
    }

    [[nodiscard]] size_t GetConstructorCount() const {
      return ordinalToObjectMap.size();
    }
  };
}