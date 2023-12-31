#pragma once

#include "Pipe.hpp"

namespace Ship {
  CreateInvalidArgumentErrorable(IncompleteByteFrameErrorable, size_t, "ByteBuffer doesn't contain enough data to read frame correctly");
  CreateInvalidArgumentErrorable(InvalidByteFrameErrorable, size_t, "An exception occurred while decoding frame");
  CreateInvalidArgumentErrorable(IncompleteFrameErrorable, PacketHolder, "ByteBuffer doesn't contain enough data to read frame correctly");
  CreateInvalidArgumentErrorable(InvalidFrameErrorable, PacketHolder, "An exception occurred while decoding frame");

  class FramedByteBytePipe : public ByteBytePipe {
   private:
    uint32_t nextReadFrameLength = 0;
    uint32_t nextWriteFrameLength = 0;
    uint32_t maxReadSize;

   public:
    FramedByteBytePipe(size_t reader_buffer_length, size_t writer_buffer_length, uint32_t max_read_size)
      : ByteBytePipe(reader_buffer_length, writer_buffer_length), maxReadSize(max_read_size) {
    }

    Errorable<size_t> Write(ByteBuffer* in) override;
    Errorable<size_t> Read(ByteBuffer* in) override;

    virtual Errorable<size_t> EncodeFrame(ByteBuffer* in, uint32_t frame_size) = 0;
    virtual Errorable<size_t> DecodeFrame(ByteBuffer* in, uint32_t frame_size) = 0;
  };

  class FramedBytePacketPipe : public BytePacketPipe {
   private:
    uint32_t nextReadFrameLength = 0;
    uint32_t maxReadSize;

   public:
    explicit FramedBytePacketPipe(uint32_t max_read_size) : BytePacketPipe(), maxReadSize(max_read_size) {
    }

    Errorable<PacketHolder> Read(ByteBuffer* in) override;

    virtual Errorable<PacketHolder> ReadPacket(ByteBuffer* in, uint32_t frame_size) = 0;
  };
}
