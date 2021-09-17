cmake_minimum_required(VERSION 3.0.2)
project(usbcan2socketcan)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra)
add_compile_options(-DCMAKE_BUILD_TYPE=Debug)

add_library(libusbcan SHARED IMPORTED)
set_property(TARGET libusbcan PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/libusbcan.so)

link_directories(${CMAKE_CURRENT_LIST_DIR}/lib/)

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src CACHE INTERNAL "")
set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include/usb_can_2_socketcan CACHE INTERNAL "")

set(HEADER_FILES ${INCLUDE_DIR}/controlcan.h ${INCLUDE_DIR}/socketcan.h ${INCLUDE_DIR}/usbcan.h)
add_library(socketcan ${SRC_DIR}/socketcan.cpp ${HEADER_FILES})
target_include_directories(socketcan PRIVATE ${INCLUDE_DIR})

add_library(usbcan ${SRC_DIR}/usbcan.cpp ${HEADER_FILES})
target_include_directories(usbcan PRIVATE ${INCLUDE_DIR})

add_executable(usbcan2socketcan ${SRC_DIR}/usbcan2socketcan.cpp ${HEADER_FILES})
target_include_directories(usbcan2socketcan PRIVATE ${INCLUDE_DIR})
target_link_libraries(usbcan2socketcan socketcan usbcan libusbcan pthread)