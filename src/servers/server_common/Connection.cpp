#include "Connection.hpp"

#define MAX_SEND_SIZE 8192

Connection::Connection(int in_sock, int in_epoll){
  int ret;
  sock = in_sock;
  epoll = in_epoll;
  buff_size = BUFF_SIZE;
  memset(buff, 0, buff_size);
  data_start = 0;
  data_last = 0;
  is_write_mode = 0;

  send_queue_head = NULL;
  send_queue_tail = NULL;

  processer.conn = this;
  processer.http.conn = this;
  processer.websock.conn = this;
  processer.websock.asos_node.conn = this;

  ret = clock_gettime(CLOCK_BOOTTIME, &time_latest_receive);
  if(ret == -1){ 
    perror("clock_gettime()"); 
  }
  time_close_grace = CLOSE_TIME_SEC;

  next = NULL;

  id="";
  key="";
}

void Connection::Close(){
  int ret;
  struct epoll_event event;
  memset(&event, 0, sizeof(event));
  event.events = EPOLLIN | EPOLLRDHUP;
  ret = epoll_ctl(epoll, EPOLL_CTL_DEL, sock, &event);
  if(ret == -1){ 
    perror("epoll_ctl(event a )"); 
  }
  processer.Close();

}

int Connection::PushSendMessage(Message *msg){
  int ret;
  int disconnect = 0;
  int written_size;
  int send_size;

  if(is_write_mode == 0){

    send_size = msg->size - msg->index;
    if(send_size > MAX_SEND_SIZE)  send_size = MAX_SEND_SIZE; 
    written_size = write(sock, msg->data, send_size);
    //    printf("%p write %d %d\n", this , msg->size, written_size);
    if(written_size > 0){
      msg->index += written_size;
      if(msg->index == msg->size){
	if(msg->disconnect == 1)  disconnect = 1;

	if(msg->is_malloced == 1){
	  free(msg->data);
	}
	delete msg;
	if(disconnect == 1){
	  return -2;
	}else{
	  return written_size;
	}
      }else{
	//printf("send size:%d index:%d\n", msg->size, msg->index);
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
	event.data.ptr = this;
	ret = epoll_ctl(epoll, EPOLL_CTL_MOD, sock, &event);
	if(ret == -1){ 
	  perror("epoll_ctl(event a )"); 
	  return ret;
	}
	is_write_mode = 1;
      }
    }else{
      perror("write");
      return written_size;
    }
  }

  if(send_queue_tail == NULL){
    send_queue_head = msg;
  }else{
    send_queue_tail->next = msg;
  }
  send_queue_tail = msg;
  msg->next = NULL;

  return 1;
}

int Connection::SendMessage(){
  int written_size;
  int ret;
  Message *msg;
  int disconnect = 0;
  int send_size;
  
  if(send_queue_head != NULL){
    msg = send_queue_head;

    send_size = msg->size - msg->index;
    if(send_size > MAX_SEND_SIZE)  send_size = MAX_SEND_SIZE; 
    written_size = write(sock, msg->data, send_size);
    if(written_size > 0){
      msg->index += written_size;
      if(msg->index == msg->size){
	if(msg->disconnect == 1)  disconnect = 1;

	send_queue_head = msg->next;
	if(msg->is_malloced == 1){
	  free(msg->data);
	}
	delete msg;
      }else{
	//printf("send size:%d index:%d\n", msg->size, msg->index);
      }
    }else{
      return written_size;
    }
  }

  if(send_queue_head == NULL){
    send_queue_tail = NULL;
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN | EPOLLRDHUP;
    event.data.ptr = this;
    ret = epoll_ctl(epoll, EPOLL_CTL_MOD, sock, &event);
    if(ret == -1){ 
      perror("epoll_ctl(event a )"); 
      return ret;
    }
    is_write_mode = 0;
  }

  if(disconnect == 1){
    printf("v %d\n", msg->disconnect);
    return -2;
  }else{
    return 1;
  }
}

Connection::~Connection(){
  if(sock >= 0)  close(sock);
  sock = -1;
}

ssize_t Connection::Read(){
  ssize_t read_size;
  read_size = read(sock, buff + data_last, buff_size - data_last);
  int ret;

  if(read_size >= 0){
    data_last = data_last + read_size;
    ret = clock_gettime(CLOCK_BOOTTIME, &time_latest_receive);
    if(ret == -1){ 
      perror("clock_gettime()"); 
    }
    //    printf("### recv: %d %d\n", (int)time_latest_receive.tv_sec, (int)time_latest_receive.tv_nsec);
  }

  if(read_size == -1) 	  perror("Connection::Read() read");

  return read_size;
}

int Connection::Process(){
  int ret = 1;
  int index;
  ssize_t written_size;
  ProcessResult process_ret;

  // process data
  //  for(index = data_start; index < data_last; index+=process_ret.next_expect_size){

  for(index = data_start; index < data_last; index+=1){
    
    processer.Process(sock, buff, index, data_start, process_ret);
    
    if(process_ret.is_written == true && process_ret.write_size <= 0){
      ret = process_ret.write_size;
      return ret;
    }
    if(process_ret.disconnect == 1){
      return -2;
    }

    data_start = process_ret.data_start;

    if(process_ret.next_expect_size != 1){
      index = data_start + process_ret.next_expect_size - 2;
    }

  }

  // move data
  if(data_start != 0){
    for(index = data_start; index < data_last; index++){
      buff[ index - data_start ] = buff[index];
    }
    data_last = data_last - data_start;
    data_start = 0;
  }
  return ret;
}

int Connection::Send(){
  return SendMessage();
}

