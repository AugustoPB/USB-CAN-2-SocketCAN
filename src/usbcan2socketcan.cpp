#include <socketcan.h>
#include <usbcan.h>
#include <utils.h>

#include <iostream>
#include <thread>

using namespace usbcan;
using namespace socketcan;
using namespace utils;

enum { BUFFER_SIZE = 1 };

void TxThead(bool &stop, SocketCan &socket_can, UsbCan &usb_can) {
  SocketCanFrame recv_frame;
  UsbCanFrame send_frame;

  while (!stop) {
    int recv_cnt = socket_can.Read(recv_frame, 1);
    if (!recv_cnt) continue;

    send_frame = SocketCanFrame2UsbCanFrame(recv_frame);

    if (usb_can.Write(send_frame, 0, 1) != UsbCanError::OK) {
      std::cerr << __FILE__ << " UsbCan write error!" << std::endl;
      continue;
    }
  }
}

void RxThead(bool &stop, SocketCan &socket_can, UsbCan &usb_can) {
  UsbCanFrame recv_frame;
  SocketCanFrame send_frame;

  while (!stop) {
    int recv_cnt = usb_can.Read(recv_frame, 0, 1);
    if (!recv_cnt) continue;

    memset(&send_frame, 0, sizeof(send_frame));
    send_frame = UsbCanFrame2SocketCanFrame(recv_frame);

    if (socket_can.Write(send_frame, 1) != SocketCanError::OK) {
      std::cerr << __FILE__ << " SocketCan write error!" << std::endl;
      continue;
    }
  }
}

int main(int , char *[]) {
  UsbCan usb_can;
  UsbCanBaund baund_rate[MAX_CHANNELS] = {(UsbCanBaund)USB_CAN_CHANNEL_1_BAUND_RATE, (UsbCanBaund)USB_CAN_CHANNEL_2_BAUND_RATE};
  if (usb_can.Open(0, (UsbCanChannelMask)USB_CAN_CHANNEL_MASK, baund_rate, RESPONSE_TIMEOUT) != UsbCanError::OK) return 0;
  if (usb_can.Init() != UsbCanError::OK) return 0;

  SocketCan socket_can;
  if (socket_can.Open(SOCKETCAN_INTERFACE, RESPONSE_TIMEOUT) != SocketCanError::OK) return 0;

  bool stop_all;

  std::thread tx_thread(TxThead, std::ref(stop_all), std::ref(socket_can), std::ref(usb_can));
  std::thread rx_thread(RxThead, std::ref(stop_all), std::ref(socket_can), std::ref(usb_can));

  tx_thread.join();
  rx_thread.join();
}