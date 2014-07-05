#include "ASOS_Core.hpp"
#include "ASOS_Node.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASOS_Core::ASOS_Core(){
  field_num = 0;
}

ASOS_Core::~ASOS_Core(){
}

int ASOS_Core::CreateField(const char *field_id){
  int size = strlen(field_id);
  if(size >= 256 || size < 1) return -1;
  if(field_num + 1 >= MAX_OBJECT_FIELD_SIZE) return -1;
  
  strncpy( fields[field_num].field_id, field_id, 256);
  fields[field_num].field_id_length = size; 

  printf("ASOS_Core::CreateField %s %s\n", fields[field_num].field_id, field_id);

  field_num++;

  return 1;
}

int ASOS_Core::GetFieldIndex(const char *field_id, int field_id_length){
  int i;
  if(field_id == NULL) return -1;
  for(i=0; i<field_num; i++){
    if(fields[i].field_id_length == field_id_length ){
      if( strncmp(fields[i].field_id, field_id, field_id_length) == 0 ){
	return i;
      }
    }
  }
  return -1;
}

int ASOS_Core::Process(ASOS_message *in_msg, ASOS_Node *in_node, ASOS_Protocolv1_info *pinfo){
  int ret=0;
  int index;
  ASOS_message res_msg;  res_msg.copy(in_msg);    res_msg.ModifyToResponse();
  ASOS_Object *object=NULL;

  index = GetFieldIndex((char *)in_msg->object_field_identification, in_msg->object_field_identification_length);
  printf("ASOS_Core::Process Field ID = %d\n", index);

  if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){

    if(pinfo->flag_permit_owner_only == 1 && in_msg->is_own != 1) {
      printf("Error: This command is only permitted to a owner.\n");
      res_msg.response_state = 0x03;
    }else{
      switch(pinfo->target){
      case ASOSP_FIELD: 
	Process_for_ObjectField(&(fields[index]), in_msg, &res_msg, in_node);
	break;
      case ASOSP_OBJECT: 
	object = fields[index].FindObject((char *)in_msg->object_field_extension, (char *)in_msg->object_identification, 
					  in_msg->object_identification_length);
	if(object != NULL){
	  Process_for_Object(object, in_msg, &res_msg, in_node);
	}else{
	  res_msg.response_state = 0x01;
	}
	break;
      default:
	printf("ASOS_Core::Process [ERROR] wrong taget\n");
	ret = -1;
	break;
      }    
    }
  }else{
    res_msg.response_state = 0x01;
  }

  //********************************************
  //  Send response
  //********************************************
  if(in_node != NULL && res_msg.long_polling_flag != 1 ){
    in_node->SendMessage(&res_msg);
  }
  
  //********************************************
  //  After processing
  //********************************************
  if(ret != -1 && res_msg.response_state == 0x00){
    if(index >= 0 && MAX_OBJECT_FIELD_SIZE > index){
      switch(pinfo->target){
      case ASOSP_FIELD: 
	AfterProcess_for_ObjectField(&(fields[index]), in_msg, &res_msg, in_node);
	break;
      case ASOSP_OBJECT: 
	if(object != NULL){
	  AfterProcess_for_Object(object, in_msg, &res_msg, in_node);
	}
      }
    }
  }

  //********************************************
  //  Clear temporary message buffer 
  //********************************************
  if(object != NULL){
    if(object->temp_app_message.message != NULL){
      free(object->temp_app_message.message);
      object->temp_app_message.message = NULL;
    }
  }
  
  return ret;
}

int ASOS_Core::Process_for_ObjectField(ASOS_ObjectField *in_field, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  int ret=0;
  
  if(in_field == NULL){printf("ASOS_Core::Process_for_ObjectField: [Error] field is NULL \n");return -1;}

  switch(in_msg->message_type){
  case 0x07:  // 0x07: "create object command"
    onCreateObject(in_field, in_msg, in_res_msg, in_node);
    break;
  case 0x0a:  // 0x0a: "delete object command"
    onDeleteObject(in_field, in_msg, in_res_msg, in_node);
    break;
    
  default:
    printf("Error: ASOS_Core::Process_for_ObjectField: Invalid message type\n");
    ret=-1;
  }

  return ret;
}

int ASOS_Core::AfterProcess_for_ObjectField(ASOS_ObjectField *in_field, ASOS_message *in_msg, 
					    ASOS_message *in_res_msg, ASOS_Node *in_node){
  int ret=0; 
  if(in_field == NULL){printf("ASOS_Core::AfterProcess_for_ObjectField: [Error] field is NULL \n");return -1;}
  return ret;
}

int ASOS_Core::Process_for_Object(ASOS_Object *in_object, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  int ret=0;

  if(in_object == NULL){printf("ASOS_Core::Process_for_Object: [Error] object is NULL \n");return -1;}

  in_object->previous_object_state = in_object->object_state;

  switch(in_msg->message_type){
  case 0x03:  // 0x03: "browse model command"
    in_object->onBrowseModel(in_msg, in_res_msg, in_node);
    break;
  case 0x04:  // 0x04: "update model command"
    in_object->onUpdateModel(in_msg, in_res_msg, in_node);
    break;
  case 0x05:  // 0x05: "pop message command"
    in_object->onPopMessage(in_msg, in_res_msg, in_node);
    break;
  case 0x06:  // 0x06: "push message command"
    in_object->onPushMessage(in_msg, in_res_msg, in_node);
    break;
  case 0x09:  // 0x09: "register model subscription command"
    in_object->onRegisterModelSubscription(in_msg, in_res_msg, in_node);
    break;
  case 0x0c:  // 0x0c: "cancel model subscription command"
    in_object->onCancelModelSubscription(in_msg, in_res_msg, in_node);
    break;
  case 0x0e:  // 0x0e: "register message capture command"
    in_object->onRegisterMessageCapture(in_msg, in_res_msg, in_node);
    break;
  case 0x0f:  // 0x0f: "cancel message capture command"
    in_object->onCancelMessageCapture(in_msg, in_res_msg, in_node);
    break;
    
  default:
    printf("Error: ASOS_Core::Process_for_Object: Invalid message type\n");
    ret=-1;
  }
  return ret;
}

int ASOS_Core::AfterProcess_for_Object(ASOS_Object *in_object, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  int ret=0;
  if(in_object == NULL){printf("ASOS_Core::AfterProcess_for_Object: [Error] object is NULL \n");return -1;}
  switch(in_msg->message_type){

  case 0x04:  // 0x04: "update model command"
    in_object->notifyModelPublish(in_msg);
    break;

  case 0x06:  // 0x06: "push message command"
    in_object->notifyPushedMessage(in_msg);
    break;

  case 0x09:  // 0x09: "register model subscription command"
    in_object->notifyModelPublish_one_node(in_msg, in_node);
    break;

  default:
    break;
  }

  if(in_msg->message_type != 0x04 && in_object->previous_object_state != in_object->object_state){
    in_object->notifyModelPublish(in_msg);    
  }

  return ret;
}

int ASOS_Core::onCreateObject(ASOS_ObjectField *in_field, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  int ret=0;
  ASOS_Object *object;

  object = in_field->FindObject((char *)in_msg->object_field_extension, (char *)in_msg->object_identification, in_msg->object_identification_length);

  if(object == NULL){
    object = new ASOS_Object((char *)in_msg->object_field_extension, (char *)in_msg->object_identification, in_msg->object_identification_length, in_msg->private_flag);
    if(object != NULL){
      in_field->AddObject(object, in_node);
      object->field = in_field;
      in_res_msg->response_state = 0x00;
      in_res_msg->model_revision = object->revision;
    }else{
      printf("Error: ASOS_Core::onCreateObject: Can't create object\n");
      in_res_msg->response_state = 0x89;
    }
  }else{
    printf("Worning: ASOS_Core::onCreateObject: Object already exist\n");
    in_res_msg->model_revision = object->revision;
    in_res_msg->response_state = 0x88;
  }

  return ret;
}

int ASOS_Core::onDeleteObject(ASOS_ObjectField *in_field, ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node){
  int ret=0;
  ASOS_Object *object;

  object = in_field->FindObject((char *)in_msg->object_field_extension, (char *)in_msg->object_identification, in_msg->object_identification_length);
  if(object != NULL){
    in_field->RemoveObject(object);
    delete object;
  }else{
    printf("Worning: ASOS_Core::onDeleteObject: Object not found\n");
    in_res_msg->response_state = 0x01;
  }

  return ret;
}

void ASOS_Core::CleanUpByNodeLeaving(ASOS_Node *in_node){
  int i;
  for(i=0; i<field_num; i++){
    fields[i].CleanUpByNodeLeaving(in_node);
  }
}

