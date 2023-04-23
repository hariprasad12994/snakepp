#include "widgets.hpp"
#include <exception>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>


/*
* Tests to be  explored
* Window resizing 
* Window bordering
* Window out of bounds
* Widget level keystroke event
* Mouse event
* Overlapping windows
* Async keystroke handling
*/


template <typename Widget>
class WidgetTest {
private:
  Screen screen;
  Widget w;

public:
  template<typename ... Args>
  WidgetTest(Args ... args): w(std::forward<Args...>(args...)), screen(Screen()) {}
  virtual void test() = 0;
};


const static std::map<std::string, std::function<void(void)>> test_fn ={
};


auto main(int argc, char* argv[]) -> int {
  try {
    namespace opt = boost::program_options;

    opt::options_description desc{"options"};
    desc.add_options()
      ("help,h", "display program usage help")
      ("basic_window_test", "create window");
    opt::variables_map program_options;
    opt::store(opt::parse_command_line(argc, argv, desc), program_options);
    opt::notify(program_options);

    if(program_options.count("help")) {
      std::cout << desc << '\n';
      return 0;
    }

  } catch(const std::exception& exception) {
    std::cerr << exception.what() << '\n';
  }
  return 0;
}
