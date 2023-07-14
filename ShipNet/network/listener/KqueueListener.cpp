#if defined(__APPLE__) || defined(__FreeBSD__)
#include "../../utils/exception/ErrnoException.hpp"
#include "Listener.hpp"
#include <arpa/inet.h>
#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace Ship {

  KqueueListener::KqueueListener(KqueueEventLoop* event_loop, int max_events, const timespec* timeout)
    : eventLoop(event_loop), maxEvents(max_events), timeout(timeout) {
  }

  KqueueListener::~KqueueListener() {
    close(kqueueFileDescriptor);
    close(socketFileDescriptor);
    delete eventLoop;
    delete[] errorBuffer;
  }

  Errorable<int> KqueueListener::Bind(SocketAddress address) {
    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor == -1) {
      return ErrnoErrorable<int>(socketFileDescriptor);
    }

    int fionbioValue = true;
    if (ioctl(socketFileDescriptor, FIONBIO, &fionbioValue) == -1) {
      return ErrnoErrorable<int>(socketFileDescriptor);
    }

    sockaddr_in bindAddress {};
    bindAddress.sin_family = AF_INET;
    bindAddress.sin_port = htons(address.GetPort());
    bindAddress.sin_addr.s_addr = inet_addr(address.GetHostname().c_str());

    if (bind(socketFileDescriptor, (sockaddr*) &bindAddress, sizeof(sockaddr_in)) == -1) {
      return ErrnoErrorable<int>(socketFileDescriptor);
    }

    if (listen(socketFileDescriptor, SOMAXCONN) == -1) {
      return ErrnoErrorable<int>(socketFileDescriptor);
    }

    kqueueFileDescriptor = kqueue();

    if (kqueueFileDescriptor == -1) {
      return ErrnoErrorable<int>(kqueueFileDescriptor);
    }

    struct kevent ctlEvent {};
    EV_SET(&ctlEvent, socketFileDescriptor, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, NULL);

    if (::kevent(kqueueFileDescriptor, &ctlEvent, 1, nullptr, 0, nullptr) == -1) {
      return ErrnoErrorable<int>(kqueueFileDescriptor);
    }

    return SuccessErrorable<int>(epollFileDescriptor);
  }

  Errorable<int> Accept(int socketFileDescriptor, sockaddr* connectionAddress, socklen_t* length, int flags) {
    int receivedFileDescriptor = accept(socketFileDescriptor, &connectionAddress, &length);
    if (receivedFileDescriptor == -1) {
      return ErrnoErrorable<int>(receivedFileDescriptor);
    }

    int receivedFlags = fcntl(receivedFileDescriptor, F_GETFL, 0);
    fcntl(receivedFileDescriptor, F_SETFL, receivedFlags | flags);

    return SuccessErrorable<int>(receivedFileDescriptor);
  }

  [[noreturn]] void KqueueListener::StartListening() {
    struct kevent events[maxEvents];
    struct kevent event; // NOLINT(cppcoreguidelines-pro-type-member-init)
    while (true) {
      int amount = kevent(kqueueFileDescriptor, nullptr, 0, events, maxEvents, timeout);

      for (int i = 0; i < amount; ++i) {
        event = events[i];
        while (true) {
          sockaddr connectionAddress {};
          socklen_t length = sizeof(sockaddr);

          Errorable<int> receivedFileDescriptor = Accept(socketFileDescriptor, &connectionAddress, &length, O_NONBLOCK | O_CLOEXEC);
          if (receivedFileDescriptor.GetTypeOrdinal() == ErrnoErrorable<int>::TYPE_ORDINAL && errno == EAGAIN) {
            break;
          } else if (!receivedFileDescriptor.IsSuccess()) {
            // Log Exception
            close((int) event.ident);
            close(kqueueFileDescriptor);
            break;
          } else {
            eventLoop->Accept(receivedFileDescriptor.GetValue());
          }
        }
      }
    }
  }
}
#endif