#ifndef PROCESSER_HTTP_HPP
#define PROCESSER_HTTP_HPP

#include "ProcesserCommon.hpp"

#define WEBSOCK_KEY_MAX 256
#define MAX_URL_SIZE 256
#define MAX_URL_ARG_NUM 5
#define MAX_URL_ARG_NAME_SIZE 16
#define MAX_URL_ARG_VALUE_SIZE 64

class Connection;

class HTTP_processer_url_args{
public:
  char arg_name[MAX_URL_ARG_NAME_SIZE];
  char arg_value[MAX_URL_ARG_VALUE_SIZE];
};

class HTTP_processer{
public:
  HTTP_processer();
  ~HTTP_processer();
  void Process(int sock, unsigned char *buff, int index, int data_start, ProcessResult &ret);
  void ClearState();

  Connection *conn;

  int line_num;

  int method; // 1: GET, 2: POST, 3: ... 
  char url[MAX_URL_SIZE];
  HTTP_processer_url_args url_args[MAX_URL_ARG_NUM];
  int arg_num;
  int version;  // majar_var * 100 + miner_ver
  
  int is_keepalive;

  unsigned char websock_key[WEBSOCK_KEY_MAX+1];
  unsigned char websock_responce_key[WEBSOCK_KEY_MAX+1];
  int websock_key_size;
 
  int is_error;

};

#endif
