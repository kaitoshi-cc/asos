#include "Server.hpp"
#include "Connection.hpp"
#include "Signal.hpp"

Server::Server(unsigned short port, unsigned int in_event_thread_num){
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sin_port = htons(port);
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_sock = -1;

  event_thread_num = in_event_thread_num;
  epolls = NULL;
}

Server::~Server(){
  struct epoll_event event;
  int ret;
  int i;

  if(server_sock != -1){
    close(server_sock);
    server_sock = -1;
  }

  if(epolls != NULL){
    delete [] epolls;
    epolls = NULL;
  }
}

void Server::Stop(){
  struct epoll_event event;
  int ret;
  int i;

  if(server_sock != -1){
    close(server_sock);
    server_sock = -1;
    printf("server stoped.\n");
  }

  if(epolls != NULL){
    delete [] epolls;
    epolls = NULL;
  }
}

int Server::Init(ASOS_Core *in_asos_core){
  int ret;
  int on = 1;
  int i;

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if(server_sock == -1){ perror("socket"); return -1; }
  ret = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  if(ret == -1){ 
    perror("setsockopt"); close(server_sock); server_sock = -1; return -1; 
  }  
  ret = bind(server_sock, (struct sockaddr *) &sockaddr, sizeof(sockaddr));
  if(ret == -1){ 
    perror("bind"); close(server_sock); server_sock = -1; return -1; 
  }
  ret = listen(server_sock, 4000);
  if(ret == -1){ 
    perror("listen"); close(server_sock); server_sock = -1; return -1; 
  }
  
  epolls = new EpollManager[event_thread_num];
  if(epolls == NULL){
    perror("malloc"); close(server_sock); server_sock = -1; return -1; 
  }
  
  for(i=0; i<event_thread_num; i++){
    epolls[i].asos_core = in_asos_core;
    epolls[i].id = i;
    epolls[i].epoll = epoll_create(3000);
    if(epolls[i].epoll == -1){
      perror("epoll_create"); close(server_sock); server_sock = -1; return -1; 
    }
  }

  return 0;
}

int Server::Accept(){
  struct sockaddr_in client_addr;
  unsigned int client_addr_size = sizeof(client_addr);
  int accepted_sock;

  accepted_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_addr_size);
  if(accepted_sock == -1){
    perror("accept");
    return -1;
  }

  return accepted_sock;
}

void Server::beginAcceptLoop(int in_accept_thread_id){
  int sock;
  int event_thread_id;
  int ret;
  Connection *conn = NULL;
  
  while(IsStop() == 0){
    sock = Accept();
    if(sock >= 0){

      if(in_accept_thread_id >= 0 && in_accept_thread_id < event_thread_num){
	event_thread_id = in_accept_thread_id;
      }else{
	event_thread_id = sock % event_thread_num;
      }

      conn = new Connection(sock, epolls[event_thread_id].epoll);

      if(conn == NULL){
	printf("Error: new Connection()\n"); 
	close(sock);
	continue;
      }
      
      ret = epolls[event_thread_id].AddConnection(conn);

      if(ret == -1){ 
	delete conn;
	conn = NULL;
      }
    }
  }
}

EpollManager *Server::getEpoll(unsigned int id){
  int ret;
  if(id < event_thread_num){
    return &epolls[id];
  }

  return NULL;
}

EpollManager::EpollManager(){
  int ret;

  connection_list = NULL;
  epoll = -1; 

  ret = clock_gettime(CLOCK_BOOTTIME, &time_latest_check);
  if(ret == -1){   perror("clock_gettime()"); }
}

EpollManager::~EpollManager(){
  if(epoll != -1){
    close(epoll); 
    epoll = -1; 
  }

  Connection *temp_conn;
  Connection *next_conn;

  for(temp_conn = connection_list; temp_conn!=NULL; temp_conn = next_conn){
    next_conn = temp_conn->next;
    temp_conn->Close();
    delete temp_conn;
    temp_conn = NULL;
  }
}

int EpollManager::AddConnection(Connection *conn){
  int ret;
  struct epoll_event event;

  if(conn == NULL) return -1;

  conn->processer.websock.asos_node.asos_core = asos_core;

  memset(&event, 0, sizeof(event));
  event.events = EPOLLIN | EPOLLRDHUP;

  ret = pthread_mutex_lock(&conn_mutex);   if(ret != 0) {perror("pthread_mutex_lock");   return -1;}

  event.data.ptr = conn;
  ret = epoll_ctl(epoll, EPOLL_CTL_ADD, conn->sock, &event);
  if(ret == -1){
    perror("epoll_ctl(EPOLL_CTL_ADD)");
    ret = pthread_mutex_unlock(&conn_mutex); if(ret != 0) {perror("pthread_mutex_unlock"); return -2;}  
    return -1;
  }

  conn->next = connection_list;
  connection_list = conn;

  ret = pthread_mutex_unlock(&conn_mutex); if(ret != 0) {perror("pthread_mutex_unlock"); return -2;}
  return 1;
}

int EpollManager::CloseConnection(Connection *conn){
  int ret;
  if(conn == NULL) return -1;
  Connection *temp_conn;
  Connection *prev_conn = NULL;
  Connection *next_conn;

  ret = pthread_mutex_lock(&conn_mutex);    if(ret != 0) {perror("pthread_mutex_lock");   return -1;}

  for(temp_conn = connection_list; temp_conn!=NULL; temp_conn = next_conn){
    next_conn = temp_conn->next;

    if(temp_conn == conn){
      if(prev_conn == NULL){
	connection_list = temp_conn->next;
      }else{
	prev_conn->next = temp_conn->next;
      }
      temp_conn->Close();
      break;
    }else{
      prev_conn = temp_conn;
    }
  }

  ret = pthread_mutex_unlock(&conn_mutex);  if(ret != 0) {perror("pthread_mutex_unlock"); return -1;}  
  return 1;
}

int EpollManager::CheckAndCloseConnections(){
  int ret;
  Connection *temp_conn;
  Connection *next_conn;
  Connection *prev_conn = NULL;
  struct timespec current_time;
  int no_recieveing_time;

  ret = clock_gettime(CLOCK_BOOTTIME, &current_time);
  if(ret == -1){   perror("clock_gettime()"); return -1; }

  if((int)current_time.tv_sec - (int)time_latest_check.tv_sec < CHECK_SPAN_SEC) return 1;
  time_latest_check = current_time;

  ret = pthread_mutex_lock(&conn_mutex);    if(ret != 0) {perror("pthread_mutex_lock");   return -1;}

  for(temp_conn = connection_list; temp_conn!=NULL; temp_conn = next_conn){
    next_conn = temp_conn->next;

    no_recieveing_time = (int)current_time.tv_sec - (int)temp_conn->time_latest_receive.tv_sec;
    //    printf("##%p %d %d\n", temp_conn, no_recieveing_time, temp_conn->time_close_grace);

    if( no_recieveing_time > temp_conn->time_close_grace){
      if(prev_conn == NULL){
	connection_list = temp_conn->next;
      }else{
	prev_conn->next = temp_conn->next;
      }
      temp_conn->Close();
      delete temp_conn;
      printf("Info: connection close by exceed no receiving time\n");
      
    }else{
      prev_conn = temp_conn;
    }
  }

  ret = pthread_mutex_unlock(&conn_mutex);  if(ret != 0) {perror("pthread_mutex_unlock"); return -1;}  
  return 1;
}

