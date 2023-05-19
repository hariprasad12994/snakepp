#include "widgets_prime.hpp"


/*
 * end goal
* auto ui = 
* Screen::chunk(1) <<= {
*   Box::chunk(2) <<= {
*     text("hello"),
*     text("world")
*   }
* }
*/


auto main(void) -> int {
  auto ui =
    LHS::chunks(2) <<= { 
      _LHS::chunks(2) <<= {
        text("hello"),
        text("world")
      },
      _LHS::chunks(2) <<= {
        text("hello"),
        text("world")
      } 
    };

  while(1) {
    ui->render();
  }

  return 0;
}
