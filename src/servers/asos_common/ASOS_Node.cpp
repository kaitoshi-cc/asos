#include "ASOS_Node.hpp"   // ASyncronus Object Service
#include "Connection.hpp"
#include "ProcesserCommon.hpp"
#include "ProcesserWebsocket.hpp"

ASOS_Node::ASOS_Node(){
}
ASOS_Node::~ASOS_Node(){
}

void ASOS_Node::Leave(){
  asos_core->CleanUpByNodeLeaving(this);
}

int ASOS_Node::ProcessMessage(const unsigned char *buff, int buff_size, int ws_opcode){
  int i;
  int ret = 1;
  ASOS_message msg;
  
  if(ws_opcode == 0x1 ){     // text
    ret = -1;                // disconnect.  Because the asos protocol has only binary message. 
    printf("[websocket opcode %x(=text)] ", ws_opcode);
    for(i=0; i<buff_size; i++){
      printf("%c", buff[i]);
      if(i > 70){ printf(" ... (size  %d)", buff_size); break; }
    }
    printf("\n");  
  }

  if(ws_opcode == 0x2 ){     // BLOB
    printf("[websocket opcode %x(=blob)] ", ws_opcode);
    for(i=0; i<buff_size; i++){
      printf("%02x", buff[i]);      
      if(i > 70){ printf(" ... (size  %d)", buff_size); break; }
    }
    printf("\n");  

    if(buff_size < 8){ printf("Worning: asos message too short\n"); return -1; }

    msg.message_type            = buff[0];
    msg.wait_time_for_response  = buff[1];
    msg.registration_lifetime   = buff[2];
    msg.lifetime_overwrite_flag = ( buff[3] & 0x80 ) >> 7;

    msg.message_identification[0] = buff[4];
    msg.message_identification[1] = buff[5];

    msg.object_field_identification_length = buff[6];
    msg.object_identification_length       = buff[7];

    msg.payload_size = buff_size - 8 - msg.object_field_identification_length - msg.object_identification_length;
    
    if(msg.payload_size < 0){ printf("Worning: asos payload size is wrong (minus value)\n"); return -1; }

    msg.object_field_identification = buff + 8;
    msg.object_identification       = msg.object_field_identification + msg.object_field_identification_length;
    msg.payload                     = msg.object_identification + msg.object_identification_length;

    switch(msg.message_type){
    case 0x00:  // "object heart beat";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.object_state = msg.payload[0];
      ret = ProcessObjectHeartbeat(&msg);
      break;

    case 0x01:  // "model publish";
      if(msg.payload_size <  8) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      msg.model_data_size = msg.payload_size - 8;
      msg.model_data      = msg.payload + 8;
      ret = ProcessModelPublish(&msg);
      break;

    case 0x02:  // "ping object command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      ret = ProcessPingObjectCommand(&msg);
      break;

    case 0x82:  // "ping object response";
      if(msg.payload_size != 2) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state = msg.payload[0];
      msg.object_state   = msg.payload[1];
      ret = ProcessPingObjectResponse(&msg);
      break;

    case 0x03:  // "browse model command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      ret = ProcessBrowseModelCommand(&msg);
      break;

    case 0x83:  // "browse model response";
      if(msg.payload_size <  9) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state  = msg.payload[0];
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 1);
      msg.model_data_size = msg.payload_size - 9;
      msg.model_data      = msg.payload + 9;
      ret = ProcessBrowseModelResponse(&msg);
      break;

    case 0x04:  // "update model command";
      if(msg.payload_size <  8) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      msg.model_data_size = msg.payload_size - 8;
      msg.model_data      = msg.payload + 8;
      ret = ProcessUpdateModelCommand(&msg);
      break;

    case 0x84:  // "update model response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state  = msg.payload[0];
      ret = ProcessUpdateModelResponse(&msg);
      break;

    case 0x05:  // "pop message command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      ret = ProcessPopMessageCommand(&msg);
      break;

    case 0x85:  // "pop message response";
      if(msg.payload_size <  9) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state  = msg.payload[0];
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 1);
      msg.message_size    = msg.payload_size - 9;
      msg.message         = msg.payload + 9;
      ret = ProcessPopMessageResponse(&msg);
      break;

    case 0x06:  // "push message command";
      if(msg.payload_size <  8) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      msg.message_size    = msg.payload_size - 8;
      msg.message         = msg.payload + 8;
      ret = ProcessPushMessageCommand(&msg);
      break;

    case 0x86:  // "push message response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state  = msg.payload[0];
      ret = ProcessPushMessageResponse(&msg);
      break;

    case 0x07:  // "create object command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      ret = ProcessCreateObjectCommand(&msg);
      break;

    case 0x87:  // "create object response";
      if(msg.payload_size != 9) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.response_state  = msg.payload[0];
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 1);
      ret = ProcessCreateObjectResponse(&msg);
      break;

    case 0x08:  // "register object heartbeat command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      ret = ProcessRegisterObjectHeartbeatCommand(&msg);
      break;

    case 0x88:  // "register object heartbeat response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      ret = ProcessRegisterObjectHeartbeatResponse(&msg);
      break;

    case 0x09:  // "register model subscription command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      ret = ProcessRegisterModelSubscriptionCommand(&msg);
      break;

    case 0x89:  // "register model subscription response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      ret = ProcessRegisterModelSubscriptionResponse(&msg);
      break;

    case 0x0a:  // "delete object command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      ret = ProcessDeleteObjectCommand(&msg);
      break;

    case 0x8a:  // "delete object response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      ret = ProcessDeleteObjectResponse(&msg);
      break;

    case 0x0b:  // "cancel object heartbeat command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      ret = ProcessCancelObjectHeartbeatCommand(&msg);
      break;

    case 0x8b:  // "cancel object heartbeat response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      ret = ProcessCancelObjectHeartbeatResponse(&msg);
      break;

    case 0x0c:  // "cancel model subscription command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      ret = ProcessCancelModelSubscriptionCommand(&msg);
      break;

    case 0x8c:  // "cancel model subscription response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      ret = ProcessCancelModelSubscriptionResponse(&msg);
      break;


    case 0x0d:  // "captured message";
      if(msg.payload_size < 8) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      msg.message_size    = msg.payload_size - 8;
      msg.message         = msg.payload + 8;
      ret = ProcessCapturedMessage(&msg);
      break;

    case 0x0e:  // "register message capture command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      ret = ProcessRegisterMessageCaptureCommand(&msg);
      break;

    case 0x8e:  // "register message capture response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      ret = ProcessRegisterMessageCaptureResponse(&msg);
      break;

    case 0x0f:  // "cancel message capture command";
      if(msg.payload_size != 0) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      ret = ProcessCancelMessageCaptureCommand(&msg);
      break;

    case 0x8f:  // "cancel message capture response";
      if(msg.payload_size != 1) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
      msg.model_revision  = msg.get_revision_from_net(msg.payload + 0);
      ret = ProcessCancelMessageCaptureResponse(&msg);
      break;

    default:    printf("Error: asos message type is unknown\n"); return -1;
    }
    //    msg.print();
  }
  
  return ret;
}

int ASOS_Node::ProcessCreateObjectCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x07){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onCreateObject(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n");  ret = -1; }
  return ret;
}

int ASOS_Node::ProcessCreateObjectResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x87){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessDeleteObjectCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x0a){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onDeleteObject(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessDeleteObjectResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x8a){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessPingObjectCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x02){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onPingObject(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessPingObjectResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x82){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessObjectHeartbeat(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x00){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessRegisterObjectHeartbeatCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x08){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onRegisterObjectHeartbeat(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessRegisterObjectHeartbeatResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x88){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessCancelObjectHeartbeatCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x0b){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onCancelObjectHeartbeat(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessCancelObjectHeartbeatResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x8b){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessBrowseModelCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x03){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onBrowseModel(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessBrowseModelResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x83){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessUpdateModelCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x04){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onUpdateModel(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessUpdateModelResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x84){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessModelPublish(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x01){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessRegisterModelSubscriptionCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x09){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onRegisterModelSubscription(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessRegisterModelSubscriptionResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x89){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessCancelModelSubscriptionCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x0c){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onCancelModelSubscription(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessCancelModelSubscriptionResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x8c){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessPopMessageCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x05){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onPopMessage(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessPopMessageResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x85){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessPushMessageCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x06){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onPushMessage(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessPushMessageResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x86){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessCapturedMessage(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x0d){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessRegisterMessageCaptureCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x0e){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onRegisterMessageCapture(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessRegisterMessageCaptureResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x8e){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessCancelMessageCaptureCommand(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x0f){
    switch(asos_core->server_type){
    case 1: // basic server
      ret = asos_core->onCancelMessageCapture(in_msg, this);
      break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}

int ASOS_Node::ProcessCancelMessageCaptureResponse(ASOS_message *in_msg){
  int ret = 0;
  if(in_msg->message_type == 0x8f){
    switch(asos_core->server_type){
    case 1: // basic server
      printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); ret = -1; break;
    default:  break;
    }
  }else{ printf("ASOS_Node Error: message type incorrect\n"); ret = -1; }
  return ret;
}


void ASOS_Node::SendMessage(ASOS_message *in_msg){
  Message *message;
  long long int websock_payload_size;
  int websock_header_size;
  int _index;
  int ret;

  message = new Message();
  message->index = 0;
  message->is_malloced = 1;
  message->disconnect = 0;
  message->next = NULL;

  // -------------------------------------------------
  // message size calcuration
  // -------------------------------------------------
  switch(in_msg->message_type){
  case 0x00:  // "object heart beat";
    in_msg->payload_size = 1; break;
  case 0x01:  // "model publish";
    in_msg->payload_size = 8 + in_msg->model_data_size; break;
  case 0x02:  // "ping object command";
    in_msg->payload_size = 0; break;
  case 0x82:  // "ping object response";
    in_msg->payload_size = 2; break;
  case 0x03:  // "browse model command";
    in_msg->payload_size = 0; break;
  case 0x83:  // "browse model response";
    in_msg->payload_size = 9 + in_msg->model_data_size; break;
  case 0x04:  // "update model command";
    in_msg->payload_size = 8 + in_msg->model_data_size; break;
  case 0x84:  // "update model response";
    in_msg->payload_size = 1; break;
  case 0x05:  // "pop message command";
    in_msg->payload_size = 0; break;
  case 0x85:  // "pop message response";
    in_msg->payload_size = 9 + in_msg->message_size; break;
  case 0x06:  // "push message command";
    in_msg->payload_size = 8 + in_msg->message_size; break;
  case 0x86:  // "push message response";
    in_msg->payload_size = 1; break;
  case 0x07:  // "create object command";
    in_msg->payload_size = 0; break;
  case 0x87:  // "create object response";
    in_msg->payload_size = 9; break;
  case 0x08:  // "register object heartbeat command";
    in_msg->payload_size = 0; break;
  case 0x88:  // "register object heartbeat response";
    in_msg->payload_size = 1; break;
  case 0x09:  // "register model subscription command";
    in_msg->payload_size = 0; break;
  case 0x89:  // "register model subscription response";
    in_msg->payload_size = 1; break;
  case 0x0a:  // "delete object command";
    in_msg->payload_size = 0; break;
  case 0x8a:  // "delete object response";
    in_msg->payload_size = 1; break;
  case 0x0b:  // "cancel object heartbeat command";
    in_msg->payload_size = 0; break;
  case 0x8b:  // "cancel object heartbeat response";
    in_msg->payload_size = 1; break;
  case 0x0c:  // "cancel model subscription command";
    in_msg->payload_size = 0; break;
  case 0x8c:  // "cancel model subscription response";
    in_msg->payload_size = 1; break;
  case 0x0d:  // "captured message";
    in_msg->payload_size = 8 + in_msg->message_size; break;
  case 0x0e:  // "register message capture command";
    in_msg->payload_size = 0; break;
  case 0x8e:  // "register message capture response";
    in_msg->payload_size = 1; break;
  case 0x0f:  // "cancel message capture command";
    in_msg->payload_size = 0; break;
  case 0x8f:  // "cancel message capture response";
    in_msg->payload_size = 1; break;
  default:    printf("Error: asos message type is unknown\n");
  }

  // -------------------------------------------------
  // websock message creation
  // -------------------------------------------------

  websock_payload_size = 8 + in_msg->object_identification_length + in_msg->object_field_identification_length + in_msg->payload_size; 
  if(websock_payload_size < 126){
    websock_header_size = 2;
  }else if(websock_payload_size < 65536){
    websock_header_size = 4;
  }else{
    websock_header_size = 10;    
  }

  message->size = websock_payload_size + websock_header_size;
  message->data = (unsigned char *)malloc(message->size);

  message->data[0] = 0x82;  // 0x80: fin flag + 0x02 denotes a binary frame
  if(websock_header_size == 2){
    message->data[1] = websock_payload_size;
    _index = 2;
  }else if(websock_header_size == 4){
    message->data[1] = 126;
    message->data[2] = (websock_payload_size / 0x0100) % 0x100;
    message->data[3] = (websock_payload_size /0x0001) % 0x100;
    _index = 4;
  }else{
    message->data[1] = 127;
    message->data[2] = (websock_payload_size / 0x0100000000000000) % 0x100;
    message->data[3] = (websock_payload_size / 0x0001000000000000) % 0x100;
    message->data[4] = (websock_payload_size / 0x0000010000000000) % 0x100;
    message->data[5] = (websock_payload_size / 0x0000000100000000) % 0x100;
    message->data[6] = (websock_payload_size / 0x0000000001000000) % 0x100;
    message->data[7] = (websock_payload_size / 0x0000000000010000) % 0x100;
    message->data[8] = (websock_payload_size / 0x0000000000000100) % 0x100;
    message->data[9] = (websock_payload_size / 0x0000000000000001) % 0x100;
    _index = 10;
  }

  // -------------------------------------------------
  // ASOS message header build
  // -------------------------------------------------
  message->data[_index] = in_msg->message_type;  _index++;
  message->data[_index] = in_msg->wait_time_for_response;  _index++;
  message->data[_index] = in_msg->registration_lifetime;  _index++;
  message->data[_index] = ( in_msg->lifetime_overwrite_flag == 1 )?0x80:0x00;  _index++;
  message->data[_index] = in_msg->message_identification[0];  _index++;
  message->data[_index] = in_msg->message_identification[1];  _index++;
  message->data[_index] = in_msg->object_field_identification_length;  _index++;
  message->data[_index] = in_msg->object_identification_length;  _index++;
  
  memcpy(message->data+_index, in_msg->object_field_identification, in_msg->object_field_identification_length);
  _index+= in_msg->object_field_identification_length;

  memcpy(message->data+_index, in_msg->object_identification, in_msg->object_identification_length);
  _index+= in_msg->object_identification_length;
  
  // -------------------------------------------------
  // ASOS message payload build
  // -------------------------------------------------
  switch(in_msg->message_type){
  case 0x00:  // "object heart beat";
    message->data[_index] = in_msg->object_state;  _index++;
    break;
  case 0x01:  // "model publish";
    in_msg->set_revision_to_net(message->data+_index, in_msg->model_revision);  _index+=8;
    memcpy(message->data+_index, in_msg->model_data, in_msg->model_data_size);
    _index+= in_msg->model_data_size;
    break;
  case 0x02:  // "ping object command";
    break;
  case 0x82:  // "ping object response";
    message->data[_index] = in_msg->response_state;  _index++;
    message->data[_index] = in_msg->object_state;  _index++;
  case 0x03:  // "browse model command";
    break;
  case 0x83:  // "browse model response";
    message->data[_index] = in_msg->response_state;  _index++;
    in_msg->set_revision_to_net(message->data+_index, in_msg->model_revision);  _index+=8;
    memcpy(message->data+_index, in_msg->model_data, in_msg->model_data_size);
    _index+= in_msg->model_data_size;
    break;
  case 0x04:  // "update model command";
    in_msg->set_revision_to_net(message->data+_index, in_msg->model_revision);  _index+=8;
    memcpy(message->data+_index, in_msg->model_data, in_msg->model_data_size);
    _index+= in_msg->model_data_size;
    break;
  case 0x84:  // "update model response";
    message->data[_index] = in_msg->response_state;  _index++;
    break;
  case 0x05:  // "pop message command";
    break;
  case 0x85:  // "pop message response";
    message->data[_index] = in_msg->response_state;  _index++;
    in_msg->set_revision_to_net(message->data+_index, in_msg->model_revision);  _index+=8;
    memcpy(message->data+_index, in_msg->message, in_msg->message_size);
    _index+= in_msg->message_size;
    break;
  case 0x06:  // "push message command";
    in_msg->set_revision_to_net(message->data+_index, in_msg->model_revision);  _index+=8;
    memcpy(message->data+_index, in_msg->message, in_msg->message_size);
    _index+= in_msg->message_size;
    break;
  case 0x86:  // "push message response";
    message->data[_index] = in_msg->response_state;  _index++;
    break;
  case 0x07:  // "create object command";
    break;
  case 0x87:  // "create object response";
    message->data[_index] = in_msg->response_state;  _index++;
    in_msg->set_revision_to_net(message->data+_index, in_msg->model_revision);  _index+=8;
    break;
  case 0x08:  // "register object heartbeat command";
    break;
  case 0x88:  // "register object heartbeat response";
    message->data[_index] = in_msg->response_state;  _index++;
    break;
  case 0x09:  // "register model subscription command";
    break;
  case 0x89:  // "register model subscription response";
    message->data[_index] = in_msg->response_state;  _index++;
    break;
  case 0x0a:  // "delete object command";
    break;
  case 0x8a:  // "delete object response";
    message->data[_index] = in_msg->response_state;  _index++;
    break;
  case 0x0b:  // "cancel object heartbeat command";
    break;
  case 0x8b:  // "cancel object heartbeat response";
    message->data[_index] = in_msg->response_state;  _index++;
    break;
  case 0x0c:  // "cancel model subscription command";
    break;
  case 0x8c:  // "cancel model subscription response";
    message->data[_index] = in_msg->response_state;  _index++;
    break;
  case 0x0d:  // "captured message";
    in_msg->set_revision_to_net(message->data+_index, in_msg->model_revision);  _index+=8;
    memcpy(message->data+_index, in_msg->message, in_msg->message_size);
    _index+= in_msg->message_size;
    break;
  case 0x0e:  // "register message capture command";
    break;
  case 0x8e:  // "register message capture response";
    message->data[_index] = in_msg->response_state;  _index++;
    break;
  case 0x0f:  // "cancel message capture command";
    break;
  case 0x8f:  // "cancel message capture response";
    message->data[_index] = in_msg->response_state;  _index++;
    break;
  default:    printf("Error: asos message type is unknown\n");
  }
  
  // -------------------------------------------------
  // websock message send
  // -------------------------------------------------
  ret = conn->PushSendMessage(message);
  
  if(ret < 0){
    printf("WORNING: Message can't send\n");
  }

}
