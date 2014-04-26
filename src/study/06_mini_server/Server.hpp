#ifndef SERVER_HPP
#define SERVER_HPP

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

class Connection;

#define CHECK_SPAN_SEC 5

class EpollManager{
public:
  EpollManager();
  ~EpollManager();

  int id;
  int epoll;

  Connection *connection_list;
  pthread_mutex_t conn_mutex = PTHREAD_MUTEX_INITIALIZER;  
  int AddConnection(Connection *conn);
  int CloseConnection(Connection *conn);
  int CheckAndCloseConnections();

  struct timespec time_latest_check;
  
};

class Server{
public:
  Server(unsigned short port, unsigned int in_event_thread_num);
  ~Server();
  int  Init();
  void Stop();
  void beginAcceptLoop(int in_event_thread_id);
  EpollManager *getEpoll(unsigned int id);

private:
  struct sockaddr_in sockaddr;
  int server_sock;

  EpollManager *epolls;
  int event_thread_num;

  int  Accept();

};


#endif
