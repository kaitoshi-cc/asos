#include "ASOS_Object.hpp"
#include <stdio.h>
#include <string.h>

ASOS_Object::ASOS_Object(char *in_object_id, int object_id_length){
  if(object_id_length < 256){
    strncpy(object_id, in_object_id, object_id_length);
    object_id[object_id_length] = '\0';
  }else{
    strncpy(object_id, in_object_id, 256);
    object_id[256] = '\0';
    printf("Error: object id length too long\n");
  }
}

ASOS_Object::~ASOS_Object(){
}


