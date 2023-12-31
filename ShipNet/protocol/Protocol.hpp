#pragma once

#include "../utils/exception/Errorable.hpp"
#include "data/uuid/UUID.hpp"
#include <deque>
#include <list>
#include <string>
#include <utility>
#include <vector>

namespace Ship {

  class ProtocolVersion {
   private:
    uint32_t ordinal;
    uint32_t protocolID;
    std::string displayVersion;

   public:
    static const ProtocolVersion UNKNOWN;

    ProtocolVersion();

    ProtocolVersion(uint32_t ordinal, uint32_t protocol_id, std::string display_version);

    [[nodiscard]] uint32_t GetOrdinal() const;

    [[nodiscard]] uint32_t GetProtocolID() const;

    [[nodiscard]] const std::string& GetDisplayVersion() const;

    bool operator<(ProtocolVersion* other) const {
      return this->protocolID < other->protocolID;
    };

    bool operator>(const ProtocolVersion* other) const {
      return this->protocolID > other->protocolID;
    };

    bool operator<=(const ProtocolVersion* other) const {
      return this->protocolID <= other->protocolID;
    };

    bool operator>=(const ProtocolVersion* other) const {
      return this->protocolID >= other->protocolID;
    };

    bool operator==(const ProtocolVersion* other) const {
      return this->protocolID == other->protocolID;
    };

    bool operator<(const ProtocolVersion& other) const {
      return this->protocolID < other.protocolID;
    };

    bool operator>(const ProtocolVersion& other) const {
      return this->protocolID > other.protocolID;
    };

    bool operator<=(const ProtocolVersion& other) const {
      return this->protocolID <= other.protocolID;
    };

    bool operator>=(const ProtocolVersion& other) const {
      return this->protocolID >= other.protocolID;
    };

    bool operator==(const ProtocolVersion& other) const {
      return this->protocolID == other.protocolID;
    };
  };

  class ProtocolVersions {
   private:
    const ProtocolVersion minimumVersion;
    const ProtocolVersion maximumVersion;
    const uint32_t minimumProtocolVersion;
    const uint32_t maximumProtocolVersion;
    const uint32_t minimumOrdinal;
    const uint32_t maximumOrdinal;

    ProtocolVersion* idToVersionMap;
    ProtocolVersion* ordinalToVersionMap;

   public:
    explicit ProtocolVersions(const std::list<ProtocolVersion>& versions);

    const ProtocolVersion* FromProtocolID(uint32_t protocol_id) const;

    const ProtocolVersion* FromOrdinal(uint32_t ordinal) const;

    [[nodiscard]] const ProtocolVersion& GetMinimumVersion() const;
    [[nodiscard]] const ProtocolVersion& GetMaximumVersion() const;
  };

  class ByteBuffer {
   public:
    virtual ~ByteBuffer();

    virtual void WriteBoolean(bool input);
    virtual void WriteByte(uint8_t input) = 0;
    virtual void WriteShort(uint16_t input);
    virtual void WriteMedium(uint32_t input);
    virtual void WriteInt(uint32_t input);
    virtual void WriteVarInt(uint32_t input);
    virtual void WriteLong(uint64_t input);
    virtual void WriteVarLong(uint64_t input);
    virtual void WriteBytes(const uint8_t* input, size_t size) = 0;
    virtual void WriteBytesAndDelete(const uint8_t* input, size_t size) = 0;
    virtual void WriteBytes(ByteBuffer* input, size_t size) = 0;
    virtual void WriteUUID(UUID input);
    virtual void WriteUUIDIntArray(UUID input);
    virtual void WriteDouble(double input);
    virtual void WriteFloat(float input);
    virtual void WriteString(const std::string& input);
    virtual void WriteByteArray(ByteBuffer* input);
    virtual void WriteAngle(float input);

    virtual Errorable<bool> ReadBoolean();
    virtual Errorable<uint8_t> ReadByte();
    virtual uint8_t ReadByteUnsafe() = 0;
    virtual Errorable<uint16_t> ReadShort();
    virtual Errorable<uint32_t> ReadMedium();
    virtual Errorable<uint32_t> ReadInt();
    virtual Errorable<uint32_t> ReadVarInt();
    virtual Errorable<uint64_t> ReadLong();
    virtual Errorable<uint64_t> ReadVarLong();
    virtual Errorable<uint8_t*> ReadBytes(size_t size);
    virtual Errorable<uint8_t*> ReadBytes(uint8_t* output, size_t size) = 0;
    virtual Errorable<double> ReadDouble();
    virtual Errorable<float> ReadFloat();
    virtual Errorable<UUID> ReadUUID();
    virtual Errorable<UUID> ReadUUIDIntArray();
    virtual Errorable<std::string> ReadString();
    virtual Errorable<std::string> ReadString(uint32_t max_size);
    virtual Errorable<ByteBuffer*> ReadByteArray();
    virtual Errorable<ByteBuffer*> ReadByteArray(uint32_t max_size);
    virtual Errorable<float> ReadAngle();

    friend ByteBuffer& operator<<(ByteBuffer& buffer, bool input);
    friend ByteBuffer& operator<<(ByteBuffer& buffer, uint8_t input);
    friend ByteBuffer& operator<<(ByteBuffer& buffer, uint16_t input);
    friend ByteBuffer& operator<<(ByteBuffer& buffer, uint32_t input);
    friend ByteBuffer& operator<<(ByteBuffer& buffer, uint64_t input);
    friend ByteBuffer& operator<<(ByteBuffer& buffer, double input);
    friend ByteBuffer& operator<<(ByteBuffer& buffer, float input);

    friend ByteBuffer& operator>>(ByteBuffer& buffer, bool& output);
    friend ByteBuffer& operator>>(ByteBuffer& buffer, uint8_t& output);
    friend ByteBuffer& operator>>(ByteBuffer& buffer, uint16_t& output);
    friend ByteBuffer& operator>>(ByteBuffer& buffer, uint32_t& output);
    friend ByteBuffer& operator>>(ByteBuffer& buffer, uint64_t& output);
    friend ByteBuffer& operator>>(ByteBuffer& buffer, double& input);
    friend ByteBuffer& operator>>(ByteBuffer& buffer, float& input);

    virtual void Release() = 0;
    virtual void ResetReaderIndex() = 0;
    [[nodiscard]] virtual size_t GetReaderIndex() const = 0;
    virtual void ResetWriterIndex() = 0;
    [[nodiscard]] virtual size_t GetWriterIndex() const = 0;
    [[nodiscard]] virtual size_t GetReadableBytes() const = 0;
    [[nodiscard]] virtual size_t GetSingleCapacity() const = 0;
    [[nodiscard]] virtual std::deque<const uint8_t*> GetDirectBuffers() const = 0;
    virtual void TryRefreshReaderBuffer() = 0;
    virtual void TryRefreshWriterBuffer() = 0;
    virtual void AppendBuffer() = 0;
    virtual void PopBuffer() = 0;
    virtual Errorable<size_t> SkipReadBytes(size_t count) = 0;
    virtual size_t SkipWriteBytes(size_t count) = 0;

    [[nodiscard]] virtual bool CanReadDirect(size_t read_size) const = 0;
    virtual uint8_t* GetDirectReadAddress() = 0;

    [[nodiscard]] virtual bool CanWriteDirect(size_t write_size) const = 0;
    virtual uint8_t* GetDirectWriteAddress() = 0;

    static uint32_t VarIntBytes(uint32_t varInt);
    static uint32_t VarLongBytes(uint64_t varLong);
    static uint32_t StringBytes(const std::string& string);
    static uint32_t ArrayBytes(uint32_t arrayLength);

    static const uint32_t BYTE_SIZE;
    static const uint32_t SHORT_SIZE;
    static const uint32_t MEDIUM_SIZE;
    static const uint32_t INT_SIZE;
    static const uint32_t LONG_SIZE;
    static const uint32_t FLOAT_SIZE;
    static const uint32_t DOUBLE_SIZE;
    static const uint32_t BOOLEAN_SIZE;
    static const uint32_t ANGLE_SIZE;
    static const uint32_t UUID_SIZE;
  };

  CreateInvalidArgumentErrorable(InvalidUUIDErrorable, UUID, "Invalid UUID read");
  CreateInvalidArgumentErrorable(InvalidVarIntErrorable, uint32_t, "Invalid VarInt read");
  CreateInvalidArgumentErrorable(IncompleteVarIntErrorable, uint32_t, "ByteBuffer doesn't contain enough data to read VarInt correctly");
  CreateInvalidArgumentErrorable(InvalidVarLongErrorable, uint64_t, "Invalid VarLong read");
  CreateInvalidArgumentErrorable(IncompleteVarLongErrorable, uint64_t, "ByteBuffer doesn't contain enough data to read VarLong correctly");
  CreateInvalidArgumentErrorable(IncompleteBooleanErrorable, bool, "ByteBuffer doesn't contain enough data to read boolean correctly");
  CreateInvalidArgumentErrorable(IncompleteByteErrorable, uint8_t, "ByteBuffer doesn't contain enough data to read byte correctly");
  CreateInvalidArgumentErrorable(IncompleteShortErrorable, uint16_t, "ByteBuffer doesn't contain enough data to read short correctly");
  CreateInvalidArgumentErrorable(IncompleteMediumErrorable, uint32_t, "ByteBuffer doesn't contain enough data to read medium correctly");
  CreateInvalidArgumentErrorable(IncompleteIntErrorable, uint32_t, "ByteBuffer doesn't contain enough data to read int correctly");
  CreateInvalidArgumentErrorable(IncompleteLongErrorable, uint64_t, "ByteBuffer doesn't contain enough data to read long correctly");
  CreateInvalidArgumentErrorable(IncompleteFloatErrorable, float, "ByteBuffer doesn't contain enough data to read float correctly");
  CreateInvalidArgumentErrorable(IncompleteDoubleErrorable, double, "ByteBuffer doesn't contain enough data to read double correctly");
  CreateInvalidArgumentErrorable(IncompleteByteArrayErrorable, uint8_t*, "ByteBuffer doesn't contain enough data to read byte array correctly");
  CreateInvalidArgumentErrorable(InvalidStringSizeErrorable, std::string, "Invalid received string size");
  CreateInvalidArgumentErrorable(InvalidByteArraySizeErrorable, ByteBuffer*, "Invalid received byte array size");
  CreateInvalidArgumentErrorable(IncompleteAngleErrorable, float, "ByteBuffer doesn't contain enough data to read angle correctly");
  CreateInvalidArgumentErrorable(InvalidReadSkipRequest, size_t, "Not enough readable bytes to skip them");

  class ByteBufferImpl : public ByteBuffer {
   private:
    std::deque<const uint8_t*> buffers;
    size_t singleCapacity;
    uint8_t* currentReadBuffer;
    uint8_t* currentWriteBuffer;
    size_t localReaderIndex = 0;
    size_t localWriterIndex = 0;
    size_t readableBytes = 0;

   public:
    explicit ByteBufferImpl(size_t singleCapacity);
    ByteBufferImpl(uint8_t* buffer, size_t singleCapacity);
    explicit ByteBufferImpl(ByteBuffer* buffer);

    ~ByteBufferImpl() override;

    void WriteByte(uint8_t input) override;
    void WriteBytes(const uint8_t* input, size_t size) override;
    void WriteBytes(ByteBuffer* buffer, size_t size) override;
    void WriteBytesAndDelete(const uint8_t* input, size_t size) override;

    uint8_t ReadByteUnsafe() override;
    Errorable<uint8_t*> ReadBytes(uint8_t* output, size_t size) override;

    void Release() override;
    void ResetReaderIndex() override;
    [[nodiscard]] size_t GetReaderIndex() const override;
    void ResetWriterIndex() override;
    [[nodiscard]] size_t GetWriterIndex() const override;
    [[nodiscard]] size_t GetReadableBytes() const override;
    [[nodiscard]] size_t GetSingleCapacity() const override;
    [[nodiscard]] std::deque<const uint8_t*> GetDirectBuffers() const override;
    void TryRefreshReaderBuffer() override;
    void TryRefreshWriterBuffer() override;
    void AppendBuffer() override;
    void PopBuffer() override;
    Errorable<size_t> SkipReadBytes(size_t count) override;
    size_t SkipWriteBytes(size_t count) override;

    [[nodiscard]] bool CanReadDirect(size_t read_size) const override;
    uint8_t* GetDirectReadAddress() override;

    [[nodiscard]] bool CanWriteDirect(size_t write_size) const override;
    uint8_t* GetDirectWriteAddress() override;
  };

  class ByteCounter : public ByteBuffer {
   private:
    size_t writerIndex = 0;

   public:
    ~ByteCounter() override = default;

    void WriteByte(uint8_t input) override;
    void WriteBytes(const uint8_t* input, size_t size) override;
    void WriteBytes(ByteBuffer* buffer, size_t size) override;
    void WriteBytesAndDelete(const uint8_t* input, size_t size) override;

    void WriteBoolean(bool input) override;
    void WriteShort(uint16_t input) override;
    void WriteMedium(uint32_t input) override;
    void WriteInt(uint32_t input) override;
    void WriteVarInt(uint32_t input) override;
    void WriteLong(uint64_t input) override;
    void WriteVarLong(uint64_t input) override;
    void WriteUUID(UUID input) override;
    void WriteUUIDIntArray(UUID input) override;
    void WriteDouble(double input) override;
    void WriteFloat(float input) override;
    void WriteString(const std::string& input) override;
    void WriteAngle(float input) override;

    uint8_t ReadByteUnsafe() override;
    Errorable<uint8_t*> ReadBytes(uint8_t* output, size_t size) override;

    void Release() override;
    void ResetReaderIndex() override;
    [[nodiscard]] size_t GetReaderIndex() const override;
    void ResetWriterIndex() override;
    [[nodiscard]] size_t GetWriterIndex() const override;
    [[nodiscard]] size_t GetReadableBytes() const override;
    [[nodiscard]] size_t GetSingleCapacity() const override;
    [[nodiscard]] std::deque<const uint8_t*> GetDirectBuffers() const override;
    void TryRefreshReaderBuffer() override;
    void TryRefreshWriterBuffer() override;
    void AppendBuffer() override;
    void PopBuffer() override;
    Errorable<size_t> SkipReadBytes(size_t count) override;
    size_t SkipWriteBytes(size_t count) override;

    [[nodiscard]] bool CanReadDirect(size_t read_size) const override;
    uint8_t* GetDirectReadAddress() override;

    [[nodiscard]] bool CanWriteDirect(size_t write_size) const override;
    uint8_t* GetDirectWriteAddress() override;
  };

  class Serializable {
   public:
    virtual ~Serializable() = default;

    virtual Errorable<bool> Write(const ProtocolVersion* version, ByteBuffer* buffer) const = 0;
    virtual uint32_t Size(const ProtocolVersion* version) const {
      ByteCounter counter;
      Write(version, &counter);
      return counter.GetWriterIndex();
    }
  };

  CreateInvalidArgumentErrorable(InvalidSerializableWriteErrorable, bool, "Tried to write invalid Serializable");
}
