#include "ASOS_Object.hpp"
#include "ASOS_message.hpp"
#include "ASOS_Node.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASOS_registered_node::ASOS_registered_node(ASOS_Node *in_node){
  node = in_node;
}

ASOS_registered_node::~ASOS_registered_node(){
}

ASOS_ApplicationMessage::ASOS_ApplicationMessage(){
  message = NULL;
}

ASOS_ApplicationMessage::~ASOS_ApplicationMessage(){
  if(message != NULL){
    free(message);
    message = NULL;
  }
}

ASOS_Object::ASOS_Object(char *in_object_id, int object_id_length){
  if(object_id_length < 256){
    strncpy(object_id, in_object_id, object_id_length);
    object_id[object_id_length] = '\0';
  }else{
    strncpy(object_id, in_object_id, 256);
    object_id[256] = '\0';
    printf("Error: object id length too long\n");
  }

  revision = 1;  // *** TODO: set revision based on current time (see design document) ***

  queue_head =0;
  queue_tail =0;
  temp_app_message.message = NULL;
}

ASOS_Object::~ASOS_Object(){
  std::list<ASOS_registered_node *>::iterator iter;
  ASOS_registered_node *reg;
  
  for(iter = model_subscription_registrants.begin(); iter != model_subscription_registrants.end(); iter++ ){
    reg = *iter; 
    if(reg != NULL){ delete reg; }
  }

  for(iter = message_capture_registrants.begin(); iter != message_capture_registrants.end(); iter++ ){
    reg = *iter; 
    if(reg != NULL){ delete reg; }
  }

  for(iter = message_pop_requestors.begin(); iter != message_pop_requestors.end(); iter++ ){
    reg = *iter; 
    if(reg != NULL){ 
      if(reg->res_msg != NULL){ delete reg->res_msg; }
      delete reg; 
    }
  }
}

int ASOS_Object::onPingObject(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->object_state = 0x01;
  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onRegisterObjectHeartbeat(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onCancelObjectHeartbeat(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onBrowseModel(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  in_res_msg->model_revision = revision;
  in_res_msg->model_data_size = model_size;
  in_res_msg->model_data = model_data;
  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onUpdateModel(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  if(in_msg->model_revision == -1 || in_msg->model_revision == revision){
    revision++;
    if(in_msg->model_data_size < ASOS_MAX_MODEL_DATA_SIZE){
      model_size = in_msg->model_data_size;
    }else{
      model_size = ASOS_MAX_MODEL_DATA_SIZE;
    }
    memcpy(model_data, in_msg->model_data, model_size);
    in_res_msg->model_revision = revision;
    in_res_msg->response_state = 0x00;
  }else{
    if(in_msg->model_revision > revision){
      in_res_msg->response_state = 0x81;
    }else{
      in_res_msg->response_state = 0x80;
    }
    in_res_msg->model_revision = revision;
  }
}

int ASOS_Object::onRegisterModelSubscription(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  std::list<ASOS_registered_node *>::iterator iter;
  int exist_flag = 0;
  for(iter = model_subscription_registrants.begin(); iter != model_subscription_registrants.end(); iter++ ){
    if( (*iter)->node == in_node ){ exist_flag == 1; break; }
  }
  
  if(exist_flag == 0){
    ASOS_registered_node *reg = new ASOS_registered_node(in_node);
    model_subscription_registrants.push_back(reg);
    in_res_msg->response_state = 0x00;

    in_node->model_subscription_list.push_back(this);

  }else{
    in_res_msg->response_state = 0x00;
  }

}

int ASOS_Object::onCancelModelSubscription(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  std::list<ASOS_registered_node *>::iterator iter;
  ASOS_registered_node *reg;
  for(iter = model_subscription_registrants.begin(); iter != model_subscription_registrants.end(); iter++ ){
    if( (*iter)->node == in_node ){ 
      reg = *iter; 
      model_subscription_registrants.erase(iter); 
      if(reg != NULL){
	delete reg;
      }

      in_node->model_subscription_list.remove(this);

      break; 
    }
  }

  in_res_msg->response_state = 0x00;
}

int ASOS_Object::onPushMessage(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  ASOS_ApplicationMessage *app_msg = new ASOS_ApplicationMessage();  
  if(app_msg != NULL){
    app_msg->target_revision = in_msg->model_revision;
    app_msg->message_size = in_msg->message_size;
    app_msg->message = (unsigned char *)malloc(app_msg->message_size+1);
    if(app_msg->message != NULL){
      memcpy(app_msg->message, in_msg->message, app_msg->message_size);
      app_msg->message[app_msg->message_size] = '\0';

      if( !message_pop_requestors.empty() ){
	// If there are long polling pop command, send message to the node.
	ASOS_registered_node *reg = message_pop_requestors.front();
	message_pop_requestors.pop_front();
	if(reg != NULL){
	  if(reg->res_msg != NULL){
	    if(reg->node != NULL){

	      temp_app_message.target_revision = app_msg->target_revision;
	      temp_app_message.message_size = app_msg->message_size;
	      temp_app_message.message = app_msg->message;
	      app_msg->message = NULL;
	      delete app_msg;

	      reg->res_msg->object_field_identification = in_res_msg->object_field_identification;
	      reg->res_msg->object_identification = in_res_msg->object_identification; 

	      reg->res_msg->model_revision = temp_app_message.target_revision;
	      reg->res_msg->message_size   = temp_app_message.message_size;
	      reg->res_msg->message        = temp_app_message.message;
	      reg->res_msg->response_state = 0x00;
	      
	      reg->node->SendMessage(reg->res_msg);
	    }else{
	      printf("ERROR: ASOS_Object::onPushMessage: registered node pointer is NULL\n");
	    }
	    delete reg->res_msg;
	  }else{
	    printf("ERROR: ASOS_Object::onPushMessage: registered message is NULL\n");
	  }
	  delete reg;
	}else{
	  printf("ERROR: ASOS_Object::onPushMessage: registered node is NULL\n");
	}

      }else{
	// Push message to ring queue
	message_queue[queue_tail] = app_msg;
	queue_tail = (queue_tail + 1) % ASOS_MAX_MESSAGE_QUEUE_SIZE;
	if( queue_tail == queue_head ){
	  if(message_queue[queue_tail] != NULL ) delete message_queue[queue_tail];	
	}
      }
      
      in_res_msg->response_state = 0x00;
    }else{
      delete app_msg;
      in_res_msg->response_state = 0x89;
    }
  }else{
    in_res_msg->response_state = 0x89;
  }

}

int ASOS_Object::onPopMessage(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  if(queue_tail != queue_head){
    ASOS_ApplicationMessage *app_msg = message_queue[queue_head];
    message_queue[queue_head] = NULL;
    queue_head = (queue_head + 1) % ASOS_MAX_MESSAGE_QUEUE_SIZE;

    if(app_msg != NULL){
      temp_app_message.target_revision = app_msg->target_revision;
      temp_app_message.message_size = app_msg->message_size;
      temp_app_message.message = app_msg->message;
      app_msg->message = NULL;
      delete app_msg;

      in_res_msg->model_revision = temp_app_message.target_revision;
      in_res_msg->message_size   = temp_app_message.message_size;
      in_res_msg->message        = temp_app_message.message;
      in_res_msg->response_state = 0x00;

    }else{
      in_res_msg->response_state = 0x87;
    }
    
  }else{
    // for long polling 
    in_res_msg->long_polling_flag = 1;
    in_res_msg->response_state = 0x87;

    int already_registered_flag = 0;

    std::list<ASOS_registered_node *>::iterator iter;

    // If there are already registered node that has same node and meesage-id, 
    // it should updated for timeout value without new registration.
    for(iter = message_pop_requestors.begin(); iter != message_pop_requestors.end(); iter++ ){
      if( ( (*iter)->node == in_node )
	  && ( (*iter)->res_msg->message_identification[0] == in_res_msg->message_identification[0] )
	  && ( (*iter)->res_msg->message_identification[1] == in_res_msg->message_identification[1] ) ){ 

	(*iter)->res_msg->wait_time_for_response = in_res_msg->wait_time_for_response;
	(*iter)->res_msg->registration_lifetime = in_res_msg->registration_lifetime;

	already_registered_flag = 1; 
	break; 
      }
    }

    if(already_registered_flag == 0){
      ASOS_registered_node *reg = new ASOS_registered_node(in_node);
      ASOS_message *saved_res_msg = new ASOS_message();
      saved_res_msg->copy(in_res_msg);
      saved_res_msg->object_field_identification = NULL;
      saved_res_msg->object_identification = NULL; 
      saved_res_msg->payload    = NULL;
      saved_res_msg->model_data = NULL;
      saved_res_msg->message    = NULL;

      reg->res_msg = saved_res_msg;
      message_pop_requestors.push_back(reg);
    }
  }

}

  
int ASOS_Object::onRegisterMessageCapture(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  std::list<ASOS_registered_node *>::iterator iter;
  int exist_flag = 0;
  for(iter = message_capture_registrants.begin(); iter != message_capture_registrants.end(); iter++ ){
    if( (*iter)->node == in_node ){ exist_flag == 1; break; }
  }
  
  if(exist_flag == 0){
    ASOS_registered_node *reg = new ASOS_registered_node(in_node);
    message_capture_registrants.push_back(reg);
    in_res_msg->response_state = 0x00;

    in_node->message_capture_list.push_back(this);

  }else{
    in_res_msg->response_state = 0x00;
  }
}

int ASOS_Object::onCancelMessageCapture(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  std::list<ASOS_registered_node *>::iterator iter;
  ASOS_registered_node *reg;
  for(iter = message_capture_registrants.begin(); iter != message_capture_registrants.end(); iter++ ){
    if( (*iter)->node == in_node ){ 
      reg = *iter; 
      message_capture_registrants.erase(iter); 
      if(reg != NULL){
	delete reg;
      }

      in_node->message_capture_list.remove(this);

      break; 
    }
  }

  in_res_msg->response_state = 0x00;
}

int ASOS_Object::notifyObjectHeartbeat(ASOS_message *in_msg){
}

int ASOS_Object::notifyModelPublish(ASOS_message *in_msg){
  if(in_msg != NULL){
    std::list<ASOS_registered_node *>::iterator iter;
    
    in_msg->message_type = 0x01; 
    in_msg->wait_time_for_response = 0x00;
    in_msg->registration_lifetime = 0x00;
    in_msg->lifetime_overwrite_flag = 0x00;
    
    in_msg->message_identification[0] = 0;
    in_msg->message_identification[1] = 0;
    
    in_msg->payload_size = 0;
    in_msg->object_state = 0;
    in_msg->model_revision = revision;
    in_msg->response_state = 0x00;
    in_msg->model_data_size = model_size;
    in_msg->model_data = model_data;
    in_msg->message_size = 0;
    in_msg->message = NULL;
    
    for(iter = model_subscription_registrants.begin(); iter != model_subscription_registrants.end(); iter++ ){
      if( (*iter)->node != NULL ){ 
	(*iter)->node->SendMessage(in_msg);
      }
    }
  }else{
    printf("ERROR: ASOS_Object::notifyModelPublish(): parameters are NULL\n");
    return -1;
  }

}

int ASOS_Object::notifyModelPublish_one_node(ASOS_message *in_msg, ASOS_Node *in_node){
  if(in_msg != NULL && in_node != NULL){

    in_msg->message_type = 0x01; 
    in_msg->wait_time_for_response = 0x00;
    in_msg->registration_lifetime = 0x00;
    in_msg->lifetime_overwrite_flag = 0x00;
    
    in_msg->message_identification[0] = 0;
    in_msg->message_identification[1] = 0;
    
    in_msg->payload_size = 0;
    in_msg->object_state = 0;
    in_msg->model_revision = revision;
    in_msg->response_state = 0x00;
    in_msg->model_data_size = model_size;
    in_msg->model_data = model_data;
    in_msg->message_size = 0;
    in_msg->message = NULL;

    in_node->SendMessage(in_msg);
  }else{
    printf("ERROR: ASOS_Object::notifyModelPublish_one_node(): parameters are NULL\n");
    return -1;
  }
}



int ASOS_Object::notifyPushedMessage(ASOS_message *in_msg){
  if(in_msg != NULL){
    std::list<ASOS_registered_node *>::iterator iter;

    in_msg->message_type = 0x0d; 
    in_msg->wait_time_for_response = 0x00;
    in_msg->registration_lifetime = 0x00;
    in_msg->lifetime_overwrite_flag = 0x00;
    
    in_msg->message_identification[0] = 0;
    in_msg->message_identification[1] = 0;
    
    in_msg->payload_size = 0;
    in_msg->object_state = 0;
    in_msg->model_revision = revision;
    in_msg->response_state = 0x00;
    in_msg->model_data_size = 0;
    in_msg->model_data = NULL;
    
    for(iter = message_capture_registrants.begin(); iter != message_capture_registrants.end(); iter++ ){
      if( (*iter)->node != NULL ){ 
	(*iter)->node->SendMessage(in_msg);
      }
    }
  }else{
    printf("ERROR: ASOS_Object::notifyPushedMessage(): parameters are NULL\n");
    return -1;
  }

}


void ASOS_Object::CleanUpByNodeLeaving(ASOS_Node *in_node){
  std::list<ASOS_registered_node *>::iterator iter;
  ASOS_registered_node *reg;

  while(iter != model_subscription_registrants.end()){
    for(iter = model_subscription_registrants.begin(); iter != model_subscription_registrants.end(); iter++ ){
      if( (*iter)->node == in_node ){ 
	reg = *iter; 
	model_subscription_registrants.erase(iter); 
	if(reg != NULL){
	  delete reg;
	}
	printf("#### delete subscription\n");
	break;
      }
    }
  }

  while(iter != message_capture_registrants.end()){
    for(iter = message_capture_registrants.begin(); iter != message_capture_registrants.end(); iter++ ){
      if( (*iter)->node == in_node ){ 
	reg = *iter; 
	message_capture_registrants.erase(iter); 
	if(reg != NULL){
	  delete reg;
	}
	printf("#### delete capture\n");
	break;
      }
    }
  }

  while(iter != message_pop_requestors.end()){

    for(iter = message_pop_requestors.begin(); iter != message_pop_requestors.end(); iter++ ){
      if( (*iter)->node == in_node ){ 
	reg = *iter; 
	printf("#### delete pop request %p 0x%02x%02x %s\n", *iter,
	       (*iter)->res_msg->message_identification[0], 
	       (*iter)->res_msg->message_identification[1], 
	       object_id
	       );

	message_pop_requestors.erase(iter); 
	if(reg != NULL){
	  if(reg->res_msg != NULL){
	    delete reg->res_msg;
	  }
	  delete reg;
	}
	break;
      }
    }
  }
}

