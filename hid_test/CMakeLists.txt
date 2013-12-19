cmake_minimum_required(VERSION 2.8)
project(hid_test)


set(CMAKE_CXX_FLAGS "--std=gnu++11")

add_executable(hid_test main.cpp)
target_link_libraries(hid_test usb-1.0)

install(TARGETS hid_test RUNTIME DESTINATION bin)
