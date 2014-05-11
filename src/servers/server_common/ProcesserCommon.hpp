#ifndef PROCESSER_COMMON_HPP
#define PROCESSER_COMMON_HPP

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/epoll.h>

#define PROTO_HTTP 0
#define PROTO_WEBSOCK 1

class ProcessResult{
public:
  int is_written;
  int write_size;
  int data_start;  
  int protocol;
  int disconnect;
  int next_expect_size;
};

class Message{
public:
  unsigned char *data;
  int size;
  int index;
  int is_malloced;  
  int disconnect;
  Message *next;
};

#endif

