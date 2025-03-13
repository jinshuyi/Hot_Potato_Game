#include "client.h"

void Client::initStatus(const char * _hostname, const char * _port) {
  hostname = _hostname;
  port = _port;
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    std::cerr << "Error: cannot get address info for host" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }  //if
}

void Client::createSocket() {
  socket_fd = socket(host_info_list->ai_family,
                     host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    std::cerr << "Error: cannot create socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }  //if

  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    std::cerr << "Error: cannot connect to socket" << std::endl;
    std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
    exit(EXIT_FAILURE);
  }  //if
  freeaddrinfo(host_info_list);
}