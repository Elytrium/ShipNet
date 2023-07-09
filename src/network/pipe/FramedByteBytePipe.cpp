#include "FramedPipe.hpp"

namespace Ship {
  Errorable<size_t> FramedByteBytePipe::Write(ByteBuffer* in) {
    size_t readableBytes = in->GetReadableBytes();
    if (nextWriteFrameLength == 0) {
      if (readableBytes == 0) {
        return IncompleteByteFrameErrorable(readableBytes);
      }

      Errorable<uint32_t> nextWriteFrameLengthErrorable = in->ReadVarInt();
      uint32_t frameLength = nextWriteFrameLengthErrorable.GetValue();
      if (nextWriteFrameLengthErrorable.IsSuccess()) {
        nextWriteFrameLength = frameLength;
      } else if (nextWriteFrameLengthErrorable.GetTypeOrdinal() == IncompleteVarIntErrorable::TYPE_ORDINAL) {
        return IncompleteByteFrameErrorable(frameLength);
      } else {
        return InvalidByteFrameErrorable(frameLength);
      }
    }

    size_t frameLength = nextWriteFrameLength;
    if (readableBytes >= frameLength) {
      Errorable<size_t> frameErrorable = EncodeFrame(in, frameLength);
      nextWriteFrameLength = 0;
      return frameErrorable;
    }

    return IncompleteByteFrameErrorable(frameLength);
  }

  Errorable<size_t> FramedByteBytePipe::Read(ByteBuffer* in) {
    if (nextReadFrameLength == 0) {
      Errorable<uint32_t> nextReadFrameLengthErrorable = in->ReadVarInt();
      uint32_t frameLength = nextReadFrameLengthErrorable.GetValue();
      if (nextReadFrameLengthErrorable.IsSuccess()) {
        if (frameLength > maxReadSize) {
          return InvalidByteFrameErrorable(frameLength);
        }

        nextReadFrameLength = frameLength;
      } else if (nextReadFrameLengthErrorable.GetTypeOrdinal() == IncompleteVarIntErrorable::TYPE_ORDINAL) {
        return IncompleteByteFrameErrorable(frameLength);
      } else {
        return InvalidByteFrameErrorable(frameLength);
      }
    }

    size_t frameLength = nextReadFrameLength;
    if (in->GetReadableBytes() >= frameLength) {
      Errorable<size_t> frameErrorable = DecodeFrame(in, frameLength);
      nextReadFrameLength = 0;
      return frameErrorable;
    }

    return IncompleteByteFrameErrorable(frameLength);
  }
}