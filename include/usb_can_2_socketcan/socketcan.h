#pragma once

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <string>

namespace socketcan {

using SocketCanFrame = can_frame;

enum SocketCanError {
  OK,
  OPEN_ERROR,
  CLOSE_ERROR,
  BIND_ERROR,
  WRITE_ERROR,
  READ_ERROR,
  READ_TIMEOUT
};

class SocketCan {
  int socket_;
  struct sockaddr_can addr_;
  struct ifreq ifr_;
  unsigned int read_timeout_;
  unsigned int message_delay_;

 public:
  SocketCan();
  SocketCan(const SocketCan&) = delete;
  SocketCan& operator=(const SocketCan&) = delete;
  ~SocketCan();
  SocketCanError Open(const std::string& socket_interface, unsigned int read_timeout);
  SocketCanError Close();
  SocketCanError Write(const SocketCanFrame& frame);
  SocketCanError Read(SocketCanFrame& frame);
};

}  // namespace socketcan