#include <unistd.h>
#include <socketcan.h>


#include <cstring>
#include <iostream>

namespace socketcan {

SocketCan::SocketCan() {}

SocketCan::~SocketCan() { Close(); }

SocketCanError SocketCan::Open(const std::string& socket_interface, unsigned int read_timeout) {
  read_timeout_ = read_timeout;

  if ((socket_ = ::socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    ::perror((std::string(__FILE__) + " Open").c_str());
    return SocketCanError::OPEN_ERROR;
  }

  ::strcpy(ifr_.ifr_name, socket_interface.c_str());
  ::ioctl(socket_, SIOCGIFINDEX, &ifr_);

  ::memset(&addr_, 0, sizeof(addr_));
  addr_.can_family = AF_CAN;
  addr_.can_ifindex = ifr_.ifr_ifindex;

  // struct timeval tv;
  // tv.tv_sec = 0;
  // tv.tv_usec = read_timeout_ * 1000;
  // ::setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));

  if (::bind(socket_, (struct sockaddr*)&addr_, sizeof(addr_)) < 0) {
    ::perror((std::string(__FILE__) + " Bind").c_str());
    return SocketCanError::BIND_ERROR;
  }

  return SocketCanError::OK;
}

SocketCanError SocketCan::Close() {
  if (::close(socket_) < 0) {
    ::perror((std::string(__FILE__) + " Close").c_str());
    return SocketCanError::CLOSE_ERROR;
  }
  return SocketCanError::OK;
}

SocketCanError SocketCan::Write(const SocketCanFrame &frame, unsigned size) {
  if (::write(socket_, &frame, sizeof(can_frame)*size) != sizeof(can_frame)*size) {
    ::perror((std::string(__FILE__) + " Write").c_str());
    return SocketCanError::WRITE_ERROR;
  }
  return SocketCanError::OK;
}

int SocketCan::Read(SocketCanFrame &frame, unsigned size) {
  auto recv_cnt = ::read(socket_, &frame, sizeof(SocketCanFrame)*size);
  if (recv_cnt < 0) {
    ::perror((std::string(__FILE__) + " Read").c_str());
  }
  return recv_cnt;
}

}  // namespace socketcan
