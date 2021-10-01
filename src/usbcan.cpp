#include <usbcan.h>

#include <iostream>

namespace usbcan {

enum { USBCAN_TYPE = 4 };

UsbCan::UsbCan() {}

UsbCan::~UsbCan() { Close(); }

UsbCanError UsbCan::Open(unsigned int device_idx, UsbCanChannelMask channel_mask,
                         UsbCanBaund baund_rate[MAX_CHANNELS], unsigned int read_timeout) {
  device_idx_ = device_idx;
  channel_mask_ = channel_mask;
  baund_rate_ = baund_rate;
  read_timeout_ = read_timeout;

  if (!::VCI_OpenDevice(USBCAN_TYPE, device_idx_, 0)) {
    std::cerr << __FILE__ << " " << __LINE__ << " Open error" << std::endl;
    return UsbCanError::OPEN_ERROR;
  }
  return UsbCanError::OK;
}

UsbCanError UsbCan::Close() {
  if (!::VCI_CloseDevice(USBCAN_TYPE, device_idx_)) {
    std::cerr << __FILE__ << " " << __LINE__ << " Close error" << std::endl;
    return UsbCanError::CLOSE_ERROR;
  }
  return UsbCanError::OK;
}

UsbCanError UsbCan::Init() {
  VCI_INIT_CONFIG configs[MAX_CHANNELS];
  for (size_t i = 0; i < MAX_CHANNELS; i++) {
    configs[i].AccCode = 0x00000000;
    configs[i].AccMask = 0xffffffff;
    configs[i].Filter = 1;
    configs[i].Mode = 0;
    configs[i].Timing0 = baund_rate_[i] & 0xff;
    configs[i].Timing1 = baund_rate_[i] >> 8;
  }

  for (int i = 0; i < MAX_CHANNELS; i++) {
    if ((channel_mask_ & (1 << i)) == 0) continue;

    if (!::VCI_InitCAN(USBCAN_TYPE, device_idx_, i, &configs[i])) {
      std::cerr << __FILE__ << " " << __LINE__ << " Init error" << std::endl;
      return UsbCanError::INIT_ERROR;
    }

    if (!::VCI_StartCAN(USBCAN_TYPE, device_idx_, i)) {
      std::cerr << __FILE__ << " " << __LINE__ << " Start error" << std::endl;
      return UsbCanError::START_ERROR;
    }
  }
  return UsbCanError::OK;
}

UsbCanError UsbCan::Write(UsbCanFrame &frame, unsigned int channel, unsigned size) {
  if (size != ::VCI_Transmit(USBCAN_TYPE, device_idx_, channel, &frame, size)) {
    VCI_ERR_INFO errinfo;
    VCI_ReadErrInfo(USBCAN_TYPE, device_idx_, channel, &errinfo);
    std::cerr << __FILE__ << " " << __LINE__ << " VCI_Write error, ErrCode(" << errinfo.ErrCode
              << ") Passive_ErrData(" << errinfo.Passive_ErrData[0] << " "
              << errinfo.Passive_ErrData[1] << " " << errinfo.Passive_ErrData[2]
              << ") ArLost_ErrData(" << errinfo.ArLost_ErrData << ")" << std::endl;
    return UsbCanError::WRITE_ERROR;
  }

  return UsbCanError::OK;
}

int UsbCan::Read(UsbCanFrame &frame, unsigned int channel, unsigned size) {
  int recv_cnt = ::VCI_Receive(USBCAN_TYPE, device_idx_, channel, &frame, size, read_timeout_);
  if (recv_cnt < 0) {
    std::cerr << __FILE__ << " " << __LINE__ << " Read error" << std::endl;
  }
  return recv_cnt;
}

}  // namespace usbcan
