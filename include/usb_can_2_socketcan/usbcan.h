#pragma once

#include <controlcan.h>

namespace usbcan {

using UsbCanFrame = VCI_CAN_OBJ;

enum { CAN_EFF_FLAG_MASK = 0x80000000 };
enum { CAN_RTR_FLAG_MASK = 0x40000000 };
enum { MAX_CHANNELS = 2 };

enum UsbCanError {
  OK,
  OPEN_ERROR,
  CLOSE_ERROR,
  INIT_ERROR,
  START_ERROR,
  WRITE_ERROR,
  READ_ERROR,
};

enum UsbCanChannelMask {
  CAN1 = 0x1,
  CAN2 = 0x2,
  CAN1_CAN2 = 0x3
};

enum UsbCanBaund {
  CAN10K = 0xFFBF,
  CAN20K = 0x1C18,
  CAN40K = 0xFF87,
  CAN50K = 0x1C09,
  CAN80K = 0xFF04,
  CAN100K = 0x1C04,
  CAN125K = 0x1C03,
  CAN200K = 0xFA81,
  CAN250K = 0x1C01,
  CAN400K = 0xFA80,
  CAN500K = 0x1C00,
  CAN666K = 0xB680,
  CAN800K = 0x1600,
  CAN1000K = 0x1400
};

class UsbCan {
  unsigned int device_idx_;
  UsbCanChannelMask channel_mask_;
  UsbCanBaund *baund_rate_;
  unsigned int read_timeout_;

public:
  UsbCan();
  UsbCan(const UsbCan &) = delete;
  UsbCan &operator=(const UsbCan &) = delete;
  ~UsbCan();
  UsbCanError Open(unsigned int device_idx, UsbCanChannelMask channel_mask, UsbCanBaund baund_rate[MAX_CHANNELS],
                   unsigned int read_timeout);
  UsbCanError Close();
  UsbCanError Init();
  UsbCanError Write(UsbCanFrame &frame, unsigned int channel, unsigned size);
  int Read(UsbCanFrame &frame, unsigned int channel, unsigned size);
};

}  // namespace usbcan