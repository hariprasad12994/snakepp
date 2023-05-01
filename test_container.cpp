#include "widgets.hpp"


auto main(void) -> int {
  auto container = Container::chunk(3) <<= {};
  auto another_container =
    NScreen::chunk(3) <<= { 
      Container::chunk(3) <<= {} 
    };
  return 0;
}
