#include "Signal.hpp"

static volatile sig_atomic_t process_stop_flag = 0;
static pthread_mutex_t stop_flag_mutex = PTHREAD_MUTEX_INITIALIZER;

static void signal_action_handler_ignore(int signum){
}

static struct sigaction old_action;
static struct sigaction new_action_ignore;



int SignalInit(){
  int ret;
  sigset_t set;

  sigemptyset(&set);

  new_action_ignore.sa_handler   = signal_action_handler_ignore;
  new_action_ignore.sa_mask      = set;
  new_action_ignore.sa_flags     = 0;
  
  ret = sigaction(SIGPIPE, &new_action_ignore ,&old_action);
  if(ret == -1){ perror("sigaction/SIGPIPE"); return -1; }

  ret = sigaction(SIGUSR1, &new_action_ignore ,&old_action);
  if(ret == -1){ perror("sigaction/SIGUSR1"); return -1; }
 
  return 0;
}

void SetStopFlag(){
  pthread_mutex_lock(&stop_flag_mutex);
  process_stop_flag = 1;
  pthread_mutex_unlock(&stop_flag_mutex);
}

int IsStop(){
  int ret;
  pthread_mutex_lock(&stop_flag_mutex);
  if(process_stop_flag){
    ret = 1;
  }else{
    ret = 0;
  }
  pthread_mutex_unlock(&stop_flag_mutex);
  return ret;
}

int SignalWaitForTermination(){
  sigset_t ss;
  int ret;
  int signo;

  sigemptyset(&ss);
  ret = sigaddset(&ss, SIGHUP);  if(ret != 0){ perror("sigaddset/SIGHUP"); return -1; }
  ret = sigaddset(&ss, SIGINT);  if(ret != 0){ perror("sigaddset/SIGINT"); return -1; }
  ret = sigaddset(&ss, SIGTERM); if(ret != 0){ perror("sigaddset/SIGTERM"); return -1; }
 
  ret = pthread_sigmask(SIG_BLOCK, &ss, NULL); if(ret != 0){ perror("sigprocmask"); return -1; }

  while(1){
    ret = sigwait(&ss, &signo);
    if(signo == SIGHUP || signo == SIGINT || signo == SIGTERM ){
      break;
    }
  }

  SetStopFlag();
}

