#include "Connection.hpp"
#include "pipe/FramedPipe.hpp"

namespace Ship {
  thread_local ByteBuffer* connectionWriteBuffer = new ByteBufferImpl(MAX_PACKET_SIZE);

  Connection::Connection(BytePacketPipe* byte_packet_pipe, PacketHandler* main_packet_handler, size_t reader_buffer_length, size_t writer_buffer_length,
    ReadWriteCloser* read_write_closer, EventLoop* event_loop)
    : bytePacketPipe(byte_packet_pipe), mainPacketHandler(main_packet_handler), readerBuffer(new ByteBufferImpl(reader_buffer_length)),
      writerBuffer(new ByteBufferImpl(writer_buffer_length)), readWriteCloser(read_write_closer), eventLoop(event_loop) {
  }

  Connection::~Connection() {
    onClose();
    delete bytePacketPipe;
    delete readerBuffer;
    delete writerBuffer;
    delete mainPacketHandler;
    for (const auto& item : packetHandlers) {
      delete item;
    }
    delete readWriteCloser;
  }

  void Connection::SetBytePacketPipe(BytePacketPipe* newBytePacketPipe) {
    delete bytePacketPipe;
    bytePacketPipe = newBytePacketPipe;
  }

  BytePacketPipe* Connection::GetBytePacketPipe() {
    return bytePacketPipe;
  }

  void Connection::AppendByteBytePipe(ByteBytePipe* byte_byte_pipe) {
    pipeline.push_back(byte_byte_pipe);
  }

  void Connection::PrependByteBytePipe(ByteBytePipe* byte_byte_pipe) {
    pipeline.push_front(byte_byte_pipe);
  }

  void Connection::AppendByteBytePipe(ByteBytePipe* byte_byte_pipe, uint32_t after_ordinal) {
    auto afterIterator = pipeline.begin();
    while (afterIterator != pipeline.end()) {
      if ((*afterIterator)->GetOrdinal() == after_ordinal) {
        pipeline.insert(++afterIterator, byte_byte_pipe);
        return;
      }
    }
  }

  void Connection::PrependByteBytePipe(ByteBytePipe* byte_byte_pipe, uint32_t before_ordinal) {
    auto beforeIterator = pipeline.begin();
    while (beforeIterator != pipeline.end()) {
      if ((*beforeIterator)->GetOrdinal() == before_ordinal) {
        pipeline.insert(beforeIterator, byte_byte_pipe);
        return;
      }
    }
  }

  void Connection::RemoveByteBytePipe(uint32_t byte_byte_pipe_ordinal) {
    pipeline.remove_if([byte_byte_pipe_ordinal](ByteBytePipe*& pipe) {
      return byte_byte_pipe_ordinal == pipe->GetOrdinal();
    });
  }

  void Connection::ReplaceMainPacketHandler(PacketHandler* packet_handler) {
    delete mainPacketHandler;
    mainPacketHandler = packet_handler;
  }

  void Connection::AppendPacketHandler(PacketHandler* byteBytePipe) {
    packetHandlers.push_back(byteBytePipe);
  }

  void Connection::PrependPacketHandler(PacketHandler* byteBytePipe) {
    packetHandlers.push_front(byteBytePipe);
  }

  void Connection::AppendPacketHandler(PacketHandler* byteBytePipe, uint32_t afterOrdinal) {
    auto afterIterator = packetHandlers.begin();
    while (afterIterator != packetHandlers.end()) {
      if ((*afterIterator)->GetOrdinal() == afterOrdinal) {
        packetHandlers.insert(++afterIterator, byteBytePipe);
        return;
      }
    }
  }

  void Connection::PrependPacketHandler(PacketHandler* byteBytePipe, uint32_t beforeOrdinal) {
    auto beforeIterator = packetHandlers.begin();
    while (beforeIterator != packetHandlers.end()) {
      if ((*beforeIterator)->GetOrdinal() == beforeOrdinal) {
        packetHandlers.insert(beforeIterator, byteBytePipe);
        return;
      }
    }
  }

  void Connection::RemovePacketHandler(uint32_t byteByteOrdinal) {
    pipeline.remove_if([byteByteOrdinal](ByteBytePipe*& pipe) {
      return byteByteOrdinal == pipe->GetOrdinal();
    });
  }

  void Connection::HandleNewBytes(uint8_t* page, size_t page_size) {
    ByteBuffer* currentBuffer = readerBuffer;
    currentBuffer->WriteBytes(page, page_size);

    for (const auto& byteBytePipe : pipeline) {
      byteBytePipe->Read(currentBuffer);
      currentBuffer = byteBytePipe->GetReaderBuffer();
    }

    Errorable<PacketHolder> packet = bytePacketPipe->Read(currentBuffer);
    if (packet.IsSuccess()) {
      Errorable<bool> wasHandled = mainPacketHandler->Handle(mainPacketHandler, this, packet.GetValue());
      if (!wasHandled.IsSuccess()) {
        // TODO: Log error
        readWriteCloser->Close();
        return;
      }

      if (!wasHandled.GetValue()) {
        for (const auto& item : packetHandlers) {
          wasHandled = item->Handle(item, this, packet.GetValue());
          if (!wasHandled.IsSuccess()) {
            // TODO: Log error
            readWriteCloser->Close();
          }

          if (wasHandled.GetValue()) {
            break;
          }
        }
      }
    } else if (packet.GetTypeOrdinal() != IncompleteFrameErrorable::TYPE_ORDINAL) {
      // TODO: Log error.
      readWriteCloser->Close();
      return;
    }
  }

  void Connection::Write(const Packet& packet) {
    connectionWriteBuffer->ResetReaderIndex();
    connectionWriteBuffer->ResetWriterIndex();
    Errorable<bool> shouldWriteErrorable = bytePacketPipe->Write(connectionWriteBuffer, packet);

    if (!shouldWriteErrorable.IsSuccess()) {
      // TODO: Log error.
      readWriteCloser->Close();
      return;
    }

    if (shouldWriteErrorable.GetValue()) {
      ByteBuffer* buffer = connectionWriteBuffer;

      for (auto byteBytePipeIterator = pipeline.rbegin(); byteBytePipeIterator != pipeline.rend(); ++byteBytePipeIterator) {
        ByteBytePipe* pipe = *byteBytePipeIterator;
        Errorable<size_t> pipeShouldWriteErrorable = pipe->Write(buffer);
        if (!pipeShouldWriteErrorable.IsSuccess()) {
          // TODO: Log error.
          readWriteCloser->Close();
          return;
        }

        if (!pipeShouldWriteErrorable.GetValue()) {
          return;
        }

        buffer = pipe->GetWriterBuffer();
      }

      WriteDirect(buffer);
    }
  }

  void Connection::WriteAndFlush(const Packet& packet) {
    Write(packet);
    Flush();
  }

  void Connection::WriteDirect(ByteBuffer* buffer) {
    writerBuffer->WriteBytes(buffer, buffer->GetReadableBytes());
  }

  ReadWriteCloser* Connection::GetReadWriteCloser() {
    return readWriteCloser;
  }

  EventLoop* Connection::GetEventLoop() {
    return eventLoop;
  }

  void Connection::Flush() {
    readWriteCloser->Write(writerBuffer);
  }

  void Connection::SetOnClose(const std::function<void()>& on_close) {
    onClose = on_close;
  }
}
