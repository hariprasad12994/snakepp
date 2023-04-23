project(snake)

cmake_minimum_required(VERSION 3.20)

find_package(Boost COMPONENTS program_options)

add_executable(snake snake.cpp)
target_link_libraries(snake ncurses)

add_executable(test_widgets test_widgets.cpp)
target_include_directories(test_widgets PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(test_widgets ncurses ${Boost_LIBRARIES})

