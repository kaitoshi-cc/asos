#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <list>

class Link;
class Connection;
class Object;

typedef std::list<Link *> LinkList;
typedef std::list<Link *>::iterator LinkIterator;

typedef std::list<Object *> ObjectList;
typedef std::list<Object *>::iterator ObjectIterator;
typedef std::list<Connection *> ConnectionList;
typedef std::list<Connection *>::iterator ConnectionIterator;

class ObjectField;
class ConnectionManager;

// ================================================================================

#define CONNECTION_MANAGER_NUM 10
#define OBJECT_FIELD_NUM       10

#define INITIAL_OBJECT_NUM     10
#define INITIAL_CONNECTION_NUM 10

ObjectField *fields[OBJECT_FIELD_NUM];
ConnectionManager *managers[CONNECTION_MANAGER_NUM];

// ================================================================================
class Link{
public:
  Link();
  ~Link();
  
  Object *object;
  Connection *connection;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  

  void open(Object *in_object);
  void open(Connection *in_connection);
  Connection *close_object_side();
  Object *close_connection_side();
  Object *getObject();
  Connection *getConnection();
  void lock();
  void unlock();
};

Link::Link(){ object = NULL; connection = NULL; }
Link::~Link(){}

void Link::open(Object *in_object){ object = in_object; }
void Link::open(Connection *in_connection){ connection = in_connection; }
Connection *Link::close_object_side(){ lock(); object = NULL; unlock(); return connection; }
Object *Link::close_connection_side(){ lock(); connection = NULL; unlock(); return object; }
Object *Link::getObject(){ return object; }
Connection *Link::getConnection(){ return connection; }
void Link::lock(){ pthread_mutex_lock(&mutex); }
void Link::unlock(){ pthread_mutex_unlock(&mutex); }


// ================================================================================
class Connection{
public:
  int connection_id;

  Connection(int in_connection_id);  
  ~Connection();  
  void Destroy();

  LinkList object_list_for_heartbeat;
  pthread_mutex_t mutex_for_heartbeat_list = PTHREAD_MUTEX_INITIALIZER;  

  LinkList object_list_for_subscribe;
  pthread_mutex_t mutex_for_subscribe_list = PTHREAD_MUTEX_INITIALIZER;  

  LinkList object_list_for_delivery;
  pthread_mutex_t mutex_for_delivery_list = PTHREAD_MUTEX_INITIALIZER;  

  int register_heartbeat(int field_id, int object_id);
  int register_subscribe(int field_id, int object_id);
  int register_delivery(int field_id, int object_id);

  int dummy_count;

  void heartbeat_listener(int object_id){
    printf("INFO: heartbeat_listener called on Connection(%04d) by Object(%04d) \n", connection_id, object_id);
    dummy_count++;
  }
  
};

// ================================================================================
class ConnectionManager{
public:
  int manager_id;
  pthread_t thread;
  int connection_count;

  ConnectionManager(int in_manager_id);
  ~ConnectionManager();

  Connection *addConnection();
  void removeConnection(int in_connection_id);
  
  ConnectionList list;
  pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;  

};

ConnectionManager::ConnectionManager(int in_manager_id){
  manager_id = in_manager_id;
  connection_count = 0;
}

ConnectionManager::~ConnectionManager(){}

Connection *ConnectionManager::addConnection(){
  int id = manager_id * 1000 + connection_count++;
  Connection *connection;
  connection = new Connection(id);

  pthread_mutex_lock(&list_mutex);
  {
    list.push_back(connection);
  }
  pthread_mutex_unlock(&list_mutex);

  return connection;
}

void ConnectionManager::removeConnection(int in_connection_id){
  ConnectionIterator iter;
  Connection *target = NULL;

  pthread_mutex_lock(&list_mutex);
  {
    for(iter = list.begin(); iter != list.end(); ++iter){ if((*iter)->connection_id == in_connection_id){ target = *iter; break;} }
    if(target != NULL){ 
      list.remove(target); 
    }
  }
  pthread_mutex_unlock(&list_mutex);

  if(target != NULL){
    target->Destroy();
    delete target;
  }
}


// ================================================================================
class Object{
public:
  int object_id;

  Object(int in_object_id);
  ~Object();
  void Destroy();

  LinkList connection_list_for_heartbeat;
  pthread_mutex_t mutex_for_heartbeat_list = PTHREAD_MUTEX_INITIALIZER;  

  LinkList connection_list_for_subscribe;
  pthread_mutex_t mutex_for_subscribe_list = PTHREAD_MUTEX_INITIALIZER;  

  LinkList connection_list_for_delivery;
  pthread_mutex_t mutex_for_delivery_list = PTHREAD_MUTEX_INITIALIZER;  

  Object *link_for_heartbeat(Link *in_link);
  Object *link_for_subscribe(Link *in_link);
  Object *link_for_delivery(Link *in_link);

  void send_heartbeat(){
    LinkIterator iter;
    Connection *connection;

    pthread_mutex_lock(&mutex_for_heartbeat_list);
    {
      for(iter = connection_list_for_heartbeat.begin(); iter != connection_list_for_heartbeat.end(); ++iter){
	(*iter)->lock();
	{
	  connection = (*iter)->getConnection();
	  if(connection != NULL){
	    connection->heartbeat_listener(object_id);
	  }else{
	    // ** TODO **
	    // need to delete "link" and remove the "link" from connection_list_for_heartbeat
	    //   For safty remove, the "link" add to "temporally list for delete".
	  }
	}
	(*iter)->unlock();
      }
      // ** TODO **
      // Remove links that are included in "temporally list for delete" from connection_list_for_heartbeat.
    }
    pthread_mutex_unlock(&mutex_for_heartbeat_list);
  }
};

Object::Object(int in_object_id){
  object_id = in_object_id;
}
Object::~Object(){}

void Object::Destroy(){
  LinkIterator iter;
  Connection *connection;

  pthread_mutex_lock(&mutex_for_heartbeat_list);
  {
    for(iter = connection_list_for_heartbeat.begin(); iter != connection_list_for_heartbeat.end(); ++iter){
      connection = (*iter)->close_object_side();
      if(connection == NULL){
	delete (*iter);
      }
    }
  }
  connection_list_for_heartbeat.clear();
  pthread_mutex_unlock(&mutex_for_heartbeat_list);
}

Object *Object::link_for_heartbeat(Link *in_link){
  in_link->open(this);

  pthread_mutex_lock(&mutex_for_heartbeat_list);
  {
    connection_list_for_heartbeat.push_back(in_link);
  }
  pthread_mutex_unlock(&mutex_for_heartbeat_list);

  return this;
}

Object *Object::link_for_subscribe(Link *in_link){
}

Object *Object::link_for_delivery(Link *in_link){
}


// ================================================================================

class ObjectField{
public:
  int field_id;
  pthread_t thread;
  int object_count;

  ObjectField(int in_field_id);
  ~ObjectField();

  ObjectList list;
  pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;  

  Object *addObject();
  void removeObject(int in_object_id);
  
  Object *register_heartbeat(int in_object_id, Link *link);

  void send_heartbeat(){
    ObjectIterator iter;
    Object *target = NULL;
    
    pthread_mutex_lock(&list_mutex);
    {
      for(iter = list.begin(); iter != list.end(); ++iter){ (*iter)->send_heartbeat(); }
      if(target != NULL){ list.remove(target); }
    }
    pthread_mutex_unlock(&list_mutex);
  
  }

};

ObjectField::ObjectField(int in_field_id){
  field_id = in_field_id;
  object_count = 0;
}

ObjectField::~ObjectField(){}

Object *ObjectField::addObject(){
  int id = field_id * 1000 + object_count++;
  Object *object;

  object = new Object(id);

  pthread_mutex_lock(&list_mutex);
  {
    list.push_back(object);
  }
  pthread_mutex_unlock(&list_mutex);

  return object;
}

void ObjectField::removeObject(int in_object_id){
  ObjectIterator iter;
  Object *target = NULL;

  pthread_mutex_lock(&list_mutex);
  {
    for(iter = list.begin(); iter != list.end(); ++iter){ if((*iter)->object_id == in_object_id){ target = *iter; } }
    if(target != NULL){ list.remove(target); }
  }
  pthread_mutex_unlock(&list_mutex);
}

Object *ObjectField::register_heartbeat(int in_object_id, Link *link){
  ObjectIterator iter;
  Object *target = NULL;

  pthread_mutex_lock(&list_mutex);
  {
    for(iter = list.begin(); iter != list.end(); ++iter){ if((*iter)->object_id == in_object_id){ target = *iter; } }
    if(target != NULL){ 
      target = target->link_for_heartbeat(link); 
    }
  }
  pthread_mutex_unlock(&list_mutex);

  return target;
}


// ================================================================================

Connection::Connection(int in_connection_id){
  connection_id = in_connection_id;
}
Connection::~Connection(){
}

void Connection::Destroy(){
  LinkIterator iter;
  Object *object;
  pthread_mutex_lock(&mutex_for_heartbeat_list);
  {
    for(iter = object_list_for_heartbeat.begin(); iter != object_list_for_heartbeat.end(); ++iter){
      object = (*iter)->close_connection_side();
      if(object == NULL){
	delete (*iter);
      }
    }
  }
  object_list_for_heartbeat.clear();
  pthread_mutex_unlock(&mutex_for_heartbeat_list);
}

int Connection::register_heartbeat(int field_id, int object_id){
  Object *object;
  Link *link = new Link();
  link->open(this);
  object = fields[field_id]->register_heartbeat(object_id, link);
  if(object == NULL){
    object = link->close_connection_side();
    if(object == NULL){
      delete link;
    }else{
      printf("Error: Connection::register_heartbeat - Strange state\n");
      exit(-1);
    }
  }else{
    pthread_mutex_lock(&mutex_for_heartbeat_list);
    {
      object_list_for_heartbeat.push_back(link);
    }
    pthread_mutex_unlock(&mutex_for_heartbeat_list);
  }
}

int Connection::register_subscribe(int field_id, int object_id){
}

int Connection::register_delivery(int field_id, int object_id){
}


// ================================================================================

void * connection_thread_loop(void *arg){
  ConnectionManager *manager = (ConnectionManager *)arg;
  int count = 0;
  int r;

  while(1){
    //    printf("manager %d\n" , manager->manager_id);

    count++;
    if(count % 10 == 0){
      r = rand() % INITIAL_CONNECTION_NUM;
      manager->removeConnection(manager->manager_id * 1000 + r);
    }
    
    usleep(10000);
  }
}

void * object_thread_loop(void *arg){
  ObjectField *field = (ObjectField *)arg;

  while(1){
    //    printf("field %d\n" , field->field_id);

    field->send_heartbeat();

    usleep(10000);
  }
}


int main(){
  int i, j, f, o;
  Connection *connection;

  // init
  for(f =0; f<OBJECT_FIELD_NUM; f++){
    fields[f] = new ObjectField(f);
    for(o=0; o<INITIAL_OBJECT_NUM; o++){ fields[f]->addObject(); }
  }

  for(i =0; i<CONNECTION_MANAGER_NUM; i++){
    managers[i] = new ConnectionManager(i);
    for(j=0; j<INITIAL_CONNECTION_NUM; j++){ 
      connection = managers[i]->addConnection(); 

      // register
      for(f =0; f<OBJECT_FIELD_NUM; f++){
	for(o=0; o<INITIAL_OBJECT_NUM; o++){
	  connection->register_heartbeat(f, o);
	}
      }
      
    }
  }
  
  // run (thread start)
  for(i =0; i<OBJECT_FIELD_NUM; i++){ pthread_create(&(fields[i]->thread), NULL, &object_thread_loop, fields[i]); }
  for(i =0; i<CONNECTION_MANAGER_NUM; i++){ pthread_create(&(managers[i]->thread), NULL, &connection_thread_loop, managers[i]); }

  while(1){ usleep(100000); }

}
