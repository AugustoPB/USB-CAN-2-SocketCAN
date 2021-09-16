#pragma once

#include <socketcan.h>
#include <usbcan.h>

#include <cstring>

namespace utils {

inline socketcan::SocketCanFrame UsbCanFrame2SocketCanFrame(usbcan::UsbCanFrame &frame) {
  socketcan::SocketCanFrame new_frame;
  new_frame.can_dlc = frame.DataLen;
  new_frame.can_id = frame.ID;
  ::memcpy(new_frame.data, frame.Data, frame.DataLen * sizeof(frame.Data[0]));
  return new_frame;
}

inline usbcan::UsbCanFrame SocketCanFrame2UsbCanFrame(socketcan::SocketCanFrame &frame) {
  usbcan::UsbCanFrame new_frame;
  new_frame.SendType = 0;
  new_frame.ID = frame.can_id;
  new_frame.DataLen = frame.can_dlc;
  memcpy(new_frame.Data, frame.data, frame.can_dlc * sizeof(frame.data[0]));
  new_frame.RemoteFlag = (frame.can_id & usbcan::CAN_RTR_FLAG_MASK) > 0;
  new_frame.ExternFlag = (frame.can_id & usbcan::CAN_EFF_FLAG_MASK) > 0;
  return new_frame;
}

}  // namespace utils
