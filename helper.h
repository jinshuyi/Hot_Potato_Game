#ifndef __HELPER_H__
#define __HELPER_H__
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "potato.h"
#include "server.h"

#define MinNumPlayer 1
#define MaxNumHops 512
#define MinNumHops 0
#define NextIPSize 100
#define NumFDs 3

// Sends a message to a socket and checks for errors
int send_message(int socket_fd, const void * message, size_t length, int flags) {
  int total_sent = 0;
  while (total_sent < length) {
    int sent = send(socket_fd, (char *)message + total_sent, length - total_sent, 0);
    if (sent == -1) {
      // handle error
      std::cerr << "Error: Failed to send message to socket" << std::endl;
      break;
    }
    total_sent += sent;
  }
  return total_sent;
}

// Receives a message from a socket and checks for errors
int receive_message(int socket_fd, void * buffer, size_t length, int flags) {
  int receive_result = recv(socket_fd, buffer, length, flags);
  if (receive_result == -1) {
    std::cerr << "Error: Failed to receive message from socket" << std::endl;
  }
  return receive_result;
}

#endif