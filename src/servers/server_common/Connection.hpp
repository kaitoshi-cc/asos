#ifndef CONNECTION_HPP
#define CONNECTION_HPP

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
#include <time.h>

#include "Processer.hpp"

#define BUFF_SIZE 4000
#define CLOSE_TIME_SEC 60

class Connection{
public:
  int epoll;
  int sock;
  
  unsigned char buff[BUFF_SIZE];
  int buff_size;
  int data_start;
  int data_last;
  int is_write_mode;
  
  Processer processer;
  
  Connection(int in_sock, int in_epoll);
  ~Connection();
  
  void Close();
  ssize_t Read();
  int Process();
  
  int Send();
  
  int PushSendMessage(Message *msg);
  int SendMessage();
  
  Message *send_queue_head;
  Message *send_queue_tail;
  
  struct timespec time_latest_receive;
  int time_close_grace;

  Connection *next;
  
  /*
    buff[]
    (1) read で buffに入れる。
    
    (2) stateに応じて、buffからfieldを切り出す。
        切り出しには３種類がある。
	(a)固定サイズ
	(b)メッセージからサイズの抽出
	(c)区切り記号までの抽出

    (3) field切り出しをして処理をして、まだbuffに残っていたら、
        繰り返しfield切り出しを行う。
	field切り出しに十分な情報がbuffになければ、buff内の
	データを先頭にずらす。
        buff_sizeいっぱいまで埋まっても切り出せない場合、エラーとする。
   */

};

#endif

