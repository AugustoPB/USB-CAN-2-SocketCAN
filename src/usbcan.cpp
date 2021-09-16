#include <usbcan.h>

#include <iostream>

namespace usbcan {

enum { USBCAN_TYPE = 4 };
enum { MAX_CHANNELS = 2 };

UsbCan::UsbCan() {}

UsbCan::~UsbCan() { Close(); }

UsbCanError UsbCan::Open(unsigned int device_idx, UsbCanChannelMask channel_mask,
                         UsbCanBaund baund_rate, unsigned int read_timeout) {
  device_idx_ = device_idx;
  channel_mask_ = channel_mask;
  baund_rate_ = baund_rate;
  read_timeout_ = read_timeout;

  if (!::VCI_OpenDevice(USBCAN_TYPE, device_idx_, 0)) {
    std::cerr << __FILE__ << " Open error" << std::endl;
    return UsbCanError::OPEN_ERROR;
  }
  return UsbCanError::OK;
}

UsbCanError UsbCan::Close() {
  if (!::VCI_CloseDevice(USBCAN_TYPE, device_idx_)) {
    std::cerr << __FILE__ << " Close error" << std::endl;
    return UsbCanError::CLOSE_ERROR;
  }
  return UsbCanError::OK;
}

UsbCanError UsbCan::Init() {
  VCI_INIT_CONFIG config;
  config.AccCode = 0x00000000;
  config.AccMask = 0xffffffff;
  config.Filter = 1;
  config.Mode = 0;
  config.Timing0 = baund_rate_ & 0xff;
  config.Timing1 = baund_rate_ >> 8;

  for (int i = 0; i < MAX_CHANNELS; i++) {
    if ((channel_mask_ & (1 << i)) == 0) continue;

    if (!::VCI_InitCAN(USBCAN_TYPE, device_idx_, i, &config)) {
      std::cerr << __FILE__ << " Init error" << std::endl;
      return UsbCanError::INIT_ERROR;
    }

    if (!::VCI_StartCAN(USBCAN_TYPE, device_idx_, i)) {
      std::cerr << __FILE__ << " Start error" << std::endl;
      return UsbCanError::START_ERROR;
    }
  }
  return UsbCanError::OK;
}

UsbCanError UsbCan::Write(UsbCanFrame *frame, unsigned int channel, unsigned size) {
  if (size != ::VCI_Transmit(USBCAN_TYPE, device_idx_, channel, frame, size)) {
    std::cerr << __FILE__ << " Write error" << std::endl;
    return UsbCanError::WRITE_ERROR;
  }
  return UsbCanError::OK;
}

UsbCanError UsbCan::Read(UsbCanFrame *frame, unsigned int channel, unsigned size) {
  int cnt = ::VCI_Receive(USBCAN_TYPE, device_idx_, channel, frame, size, read_timeout_);
  if (!cnt) {
    std::cerr << __FILE__ << " Read error" << std::endl;
    return UsbCanError::READ_ERROR;
  }
  return UsbCanError::OK;
}

}  // namespace usbcan
