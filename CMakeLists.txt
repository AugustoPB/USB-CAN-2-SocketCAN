cmake_minimum_required(VERSION 3.0.2)
project(usbcan2socketcan)

include(CMakeSettings.txt)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra)
add_compile_options(-DCMAKE_BUILD_TYPE=Debug)

add_definitions("-DUSB_CAN_CHANNEL_MASK=${USB_CAN_CHANNEL_MASK}")
message(STATUS "Mask for channel selection is set to ${USB_CAN_CHANNEL_MASK}")

add_definitions("-DUSB_CAN_CHANNEL_1_BAUND_RATE=${USB_CAN_CHANNEL_1_BAUND_RATE}")
message(STATUS "Baund rate for channel 1 is set to ${USB_CAN_CHANNEL_1_BAUND_RATE}")

add_definitions("-DUSB_CAN_CHANNEL_2_BAUND_RATE=${USB_CAN_CHANNEL_2_BAUND_RATE}")
message(STATUS "Baund rate for channel 2 is set to ${USB_CAN_CHANNEL_2_BAUND_RATE}")

add_definitions("-DSOCKETCAN_INTERFACE=\"${SOCKETCAN_INTERFACE}\"")
message(STATUS "Socketcan interface is set to ${SOCKETCAN_INTERFACE}")

add_definitions("-DRESPONSE_TIMEOUT=${RESPONSE_TIMEOUT}")
message(STATUS "Timeout value is set to ${RESPONSE_TIMEOUT} ms")

if (${ROS})
    find_package(catkin REQUIRED)

    catkin_package(
        INCLUDE_DIRS include
        LIBRARIES usb-can-2-socketcan
        #  CATKIN_DEPENDS other_catkin_pkg
        #  DEPENDS system_lib
    ) 
endif()

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

if (${ROS})
  install(TARGETS ${PROJECT_NAME}
      RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  )
  
  install(TARGETS ${PROJECT_NAME}
      ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
      LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
      RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
  )
endif()