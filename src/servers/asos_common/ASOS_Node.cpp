#include "ASOS_Node.hpp"   // ASyncronus Object Service
#include "Connection.hpp"
#include "ProcesserCommon.hpp"
#include "ProcesserWebsocket.hpp"

#include "ASOS_Protocolv1.hpp"

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
  int index=0;
  int size=0;

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

    // -------------------------------------------------
    // parsing for headers
    // -------------------------------------------------
    if(buff_size < 10){ printf("Worning: asos message too short\n"); return -1; }

    msg.protocol_type           = buff[0];
    if(msg.protocol_type != 0x01){ printf("ERROR: Protocol type invalid (%02x)\n", msg.protocol_type); return -1; }

    msg.message_type            = buff[2];
    msg.wait_time_for_response  = buff[3];
    msg.registration_lifetime   = buff[4];
    msg.lifetime_overwrite_flag = ( buff[5] & 0x80 ) >> 7;

    msg.message_identification[0] = buff[6];
    msg.message_identification[1] = buff[7];

    msg.object_field_identification_length = buff[8];
    msg.object_identification_length       = buff[9];

    msg.payload_size = buff_size - 10 - msg.object_field_identification_length - msg.object_identification_length;
    
    if(msg.payload_size < 0){ printf("Worning: asos payload size is wrong (minus value)\n"); return -1; }

    msg.object_field_identification = buff + 10;
    msg.object_identification       = msg.object_field_identification + msg.object_field_identification_length;
    msg.payload                     = msg.object_identification + msg.object_identification_length;


    // -------------------------------------------------
    // get protocol info
    // -------------------------------------------------
    ASOS_Protocolv1_info *pinfo = asos_core->asos_proto_v1.info[msg.message_type];
    if(pinfo == NULL) {printf("Error: asos message type is unknown\n"); return -1;}

    // -------------------------------------------------
    // buffered message size check
    // -------------------------------------------------
    if(pinfo->flag_response_state == 1) size += 1;
    if(pinfo->flag_object_state   == 1) size += 1;
    if(pinfo->flag_model_revision == 1) size += 8;

    if(pinfo->flag_model_data == 1 || pinfo->flag_message == 1){
      if(msg.payload_size < size) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
    }else{
      if(msg.payload_size != size) { printf("Worning: asos payload size is wrong [%s]\n", msg.message_type_string()); return -1;}
    }

    // -------------------------------------------------
    // parsing for payload
    // -------------------------------------------------
    if(pinfo->flag_response_state == 1){
      msg.response_state = msg.payload[index];
      index++;
    }
    if(pinfo->flag_object_state == 1){
      msg.object_state = msg.payload[index];  index++;
    }
    if(pinfo->flag_model_revision == 1){
      msg.model_revision  = msg.get_revision_from_net(msg.payload + index);
      index+=8;
    }
    if(pinfo->flag_model_data == 1){
      msg.model_data_size = msg.payload_size - index;
      msg.model_data      = msg.payload + index;
      index += msg.model_data_size;
    }
    if(pinfo->flag_message == 1){
      msg.message_size    = msg.payload_size - index;
      msg.message         = msg.payload + index;
      index += msg.message_size;
    }

    // -------------------------------------------------
    // process for server types
    // -------------------------------------------------
    switch(asos_core->server_type){
    case 1: // basic server
      if(pinfo->flag_accepted_by_basic_server == 0x01){
	ret = asos_core->Process(&msg, this, pinfo);
      }else{
	printf("Error: basic server should not receive Response/Notice message of ASOS.\n"); 
	ret = -1;
      }
      break;
    default:  break;
      printf("Error: This server has invalid server type.\n"); 
      ret = -1;
    }
  }
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
  // get protocol info
  // -------------------------------------------------
  ASOS_Protocolv1_info *pinfo = asos_core->asos_proto_v1.info[in_msg->message_type];
  if(pinfo == NULL) {printf("Error: asos message type is unknown\n"); return;}
  
  // -------------------------------------------------
  // message size calcuration
  // -------------------------------------------------
  in_msg->payload_size = 0;
  if(pinfo->flag_response_state == 1) in_msg->payload_size += 1;
  if(pinfo->flag_object_state   == 1) in_msg->payload_size += 1;
  if(pinfo->flag_model_revision == 1) in_msg->payload_size += 8;
  if(pinfo->flag_model_data     == 1) in_msg->payload_size += in_msg->model_data_size;
  if(pinfo->flag_message        == 1) in_msg->payload_size += in_msg->message_size;

  // -------------------------------------------------
  // websock message creation
  // -------------------------------------------------

  websock_payload_size = 10 + in_msg->object_identification_length + in_msg->object_field_identification_length + in_msg->payload_size; 
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
  message->data[_index] = 0x01;  _index++; // protocol type(0x01): ASOS protocol v1
  message->data[_index] = 0x00;  _index++; // reserved

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

  if(pinfo->flag_response_state == 1){
    message->data[_index] = in_msg->response_state;  _index++;
  }
  if(pinfo->flag_object_state == 1){
    message->data[_index] = in_msg->object_state;  _index++;
  }
  if(pinfo->flag_model_revision == 1){
    in_msg->set_revision_to_net(message->data+_index, in_msg->model_revision);  _index+=8;
  }
  if(pinfo->flag_model_data == 1){
    memcpy(message->data+_index, in_msg->model_data, in_msg->model_data_size);
    _index+= in_msg->model_data_size;
  }
  if(pinfo->flag_message == 1){
    memcpy(message->data+_index, in_msg->message, in_msg->message_size);
    _index+= in_msg->message_size;
  }
 
  // -------------------------------------------------
  // websock message send
  // -------------------------------------------------
  ret = conn->PushSendMessage(message);
  
  if(ret < 0){
    printf("WORNING: Message can't send\n");
  }

}
