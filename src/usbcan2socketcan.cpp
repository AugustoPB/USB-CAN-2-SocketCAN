#include <socketcan.h>
#include <usbcan.h>
#include <utils.h>

#include <iostream>
#include <thread>

using namespace usbcan;
using namespace socketcan;
using namespace utils;

void TxThead(bool &stop, SocketCan &socket_can, UsbCan &usb_can) {
  SocketCanFrame recv_frame;
  UsbCanFrame send_frame;

  while (!stop) {
    if (socket_can.Read(recv_frame) != SocketCanError::OK) {
      std::cout << "SocketCan receive error!";
      continue;
    }

    send_frame = SocketCanFrame2UsbCanFrame(recv_frame);

    if (usb_can.Write(&send_frame, 0, 1) != UsbCanError::OK) {
      std::cout << "UsbCan write error!";
      continue;
    }
  }
}

int main(int argc, char *argv[]) {
  UsbCan usb_can;
  if (usb_can.Open(0, CAN1, CAN500K, 100) != UsbCanError::OK) return 0;
  if (usb_can.Init() != UsbCanError::OK) return 0;

  SocketCan socket_can;
  if (socket_can.Open("vcan0", 100) != SocketCanError::OK) return 0;

  bool stop_all;

  std::thread tx_thread(TxThead, std::ref(stop_all), std::ref(socket_can), std::ref(usb_can));

  tx_thread.join();
}