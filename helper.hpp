#include <chrono>

class TimerBlock {
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> point_1;
  std::chrono::time_point<std::chrono::high_resolution_clock> point_2;
  std::chrono::duration<double, std::milli> time_in_ms;

public:
  TimerBlock(): point_1(std::chrono::high_resolution_clock::now()) {}
  ~TimerBlock() {
    point_2 = std::chrono::high_resolution_clock::now();
    time_in_ms = point_2 - point_1;
  }
};
