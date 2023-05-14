#include "widgets.hpp"
#include <curses.h>


/*
* auto ui = 
* Screen::chunk(1) <<= {
*   Box::chunk(2) <<= {
*     text("hello"),
*     text("world")
*   }
* }
*/


auto main(void) -> int {
  auto container = Container::chunk(3) <<= {};
  auto another_container =
    NScreen::chunk(3) <<= { 
      Container::chunk(3) <<= {} 
    };
  return 0;
}
