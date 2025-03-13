#ifndef __POTATO_H__
#define __POTATO_H__
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

class Potato {
 public:
  int num_hops;
  int cnt;
  int path[512];
  Potato() : num_hops(0), cnt(0) { memset(path, 0, sizeof(path)); }
};
#endif