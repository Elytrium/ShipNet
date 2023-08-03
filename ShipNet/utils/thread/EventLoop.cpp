#include "EventLoop.hpp"
#include "../ShipUtils.hpp"

namespace Ship {
  void EventLoop::Execute(const std::function<void()>& function) {
    immediateTasks.push(function);
  }

  void EventLoop::Delay(const std::function<void()>& function, time_t millis) {
    delayedTasks.emplace_back(ShipUtils::GetCurrentMillis() + millis, function);
  }

  void EventLoop::ProceedTasks() {
    while (!immediateTasks.empty()) {
      immediateTasks.front()();
      immediateTasks.pop();
    }

    if (!delayedTasks.empty()) {
      uint32_t currentTime = ShipUtils::GetCurrentMillis();

      delayedTasks.remove_if([&currentTime](auto& it) {
        if (it.first <= currentTime) {
          it.second();
          return true;
        }

        return false;
      });
    }
  }
}