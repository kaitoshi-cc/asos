#include "Server.hpp"
#include "Connection.hpp"
#include "Signal.hpp"
#include <string.h>
#include <pthread.h>
#include <errno.h>

Server *server = NULL;

void * http_accept_loop(void *arg){
  int accept_thread_id = (*(int *)arg);
  if(server != NULL) server->beginAcceptLoop(accept_thread_id);
}

#define EPOLL_WAIT_TIMEOUT_MSEC 5000

void * http_event_loop(void *arg){
  int event_thread_id = (*(int *)arg);
  int sock;
  
  ssize_t read_size;

  int request_counter=0;

  int event_num;
  int max_events = 10;
  int i;
  int ret;
  EpollManager *epoll_mng;

  Connection *conn = NULL;

  struct epoll_event events[max_events];
  for(i=0; i<max_events; i++){
    memset(&events[i], 0, sizeof(events[i]));    
  }

  epoll_mng = server->getEpoll(event_thread_id);
  if(epoll_mng == NULL){
    printf("Error: getEpoll returned -1\n");
    exit(-1);
  }

  while(1){

    event_num = epoll_wait(epoll_mng->epoll, events, max_events, EPOLL_WAIT_TIMEOUT_MSEC);

    if(event_num == -1){ perror("epoll_wait"); break; }
    if(IsStop()){ break; }

    //printf("##### epoll wait : event num = %d\n", event_num);

    for(i=0; i<event_num; i++){
      conn = (Connection *)events[i].data.ptr;
      if(conn == NULL){
	printf("Error: epoll event do not have connection\n");
	exit(-1);
      }
      sock = conn->sock;
      
      if(events[i].events & EPOLLIN){
	// Read
	ret = conn->Read();
	if(ret <= 0 ){ epoll_mng->CloseConnection(conn);  delete conn;  conn = NULL; continue; }

	// Process
	ret = conn->Process();
	if(ret <= 0 ){ epoll_mng->CloseConnection(conn);  delete conn;  conn = NULL; continue; }	
      }

      if(events[i].events & EPOLLOUT){
	// Send
	ret = conn->Send();
	if(ret <= 0 ){ epoll_mng->CloseConnection(conn);  delete conn;  conn = NULL; continue; }
      }

      if((events[i].events & EPOLLRDHUP) || 
	 (events[i].events & EPOLLERR)   || 
	 (events[i].events & EPOLLHUP) ){
	epoll_mng->CloseConnection(conn);  delete conn;  conn = NULL;
	continue;
      }
    }

    epoll_mng->CheckAndCloseConnections();

  }

  pthread_exit(NULL);
}

#define ACCEPT_THREAD_NUM 4
#define EVENT_THREAD_NUM 1

int main(int argc, char *argv[]){
  int ret;
  int i=0;
  pthread_t event_threads[EVENT_THREAD_NUM];
  pthread_t accept_threads[ACCEPT_THREAD_NUM];
  void *status;
  int event_thread_ids[EVENT_THREAD_NUM];
  int accept_thread_ids[ACCEPT_THREAD_NUM];

  ret = SignalInit();
  if(ret == -1){ printf("Error: SignalInit() returned -1\n"); fflush(stdout); exit(-1); }

  server = new Server(80, EVENT_THREAD_NUM);
  if(server == NULL){ printf("Error: Server constructor error\n"); fflush(stdout); exit(-1); }

  ret = server->Init();
  if(ret == -1){ printf("Error: Server->Init() returned -1\n"); fflush(stdout); exit(-1); }

  for(i=0; i<EVENT_THREAD_NUM; i++){ event_thread_ids[i] = i; }
  for(i=0; i<ACCEPT_THREAD_NUM; i++){ accept_thread_ids[i] = i; }

  for(i=0; i<ACCEPT_THREAD_NUM; i++){
    ret = pthread_create(&accept_threads[i], NULL, &http_accept_loop, &accept_thread_ids[i]);
    if(ret != 0){
      printf("Error: Can't create thread %d\n", i);
      perror("pthread");
      exit(-1);
    }
  }

  for(i=0; i<EVENT_THREAD_NUM; i++){
    ret = pthread_create(&event_threads[i], NULL, &http_event_loop, &event_thread_ids[i]);
    if(ret != 0){
      printf("Error: Can't create thread %d\n", i);
      perror("pthread");
      exit(-1);
    }
  }

  SignalWaitForTermination();

  for(i=0; i<ACCEPT_THREAD_NUM; i++){
    ret = -1;
    do{
      ret = pthread_kill(accept_threads[i], SIGUSR1);
      if(ret !=0 && ret != ESRCH){
	perror("pthread_kill");
      }
    }while(ret != 0 && ret != ESRCH);
  }

  for(i=0; i<EVENT_THREAD_NUM; i++){
    ret = -1;
    do{
      ret = pthread_kill(event_threads[i], SIGUSR1);
      if(ret !=0 && ret != ESRCH){
	perror("pthread_kill");
      }
    }while(ret != 0 && ret != ESRCH);
  }

  for(i=0; i<ACCEPT_THREAD_NUM; i++){
    ret = pthread_join(accept_threads[i], &status);
  }
  for(i=0; i<EVENT_THREAD_NUM; i++){
    ret = pthread_join(event_threads[i], &status);
  }
  
  server->Stop();
  delete server;
}
