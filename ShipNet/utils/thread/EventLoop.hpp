#pragma once

#include <functional>
#include <list>
#include <queue>
#include <cstdint>

namespace Ship {
  class EventLoop {
   private:
    std::queue<std::function<void()>> immediateTasks;
    std::list<std::pair<uint32_t, std::function<void()>>> delayedTasks;

   public:
    virtual ~EventLoop() = default;

    void Execute(const std::function<void()>& function);
    void Delay(const std::function<void()>& function, time_t millis);
    void ProceedTasks();

    virtual void StartLoop() {
    }
  };
}
