#include "ASOS_ObjectField.hpp"
#include <stdio.h>

ASOS_ObjectField::ASOS_ObjectField(){
}

ASOS_ObjectField::~ASOS_ObjectField(){
}

int ASOS_ObjectField::AddObject(ASOS_Object *in_object, ASOS_Node *in_node){
  //  object_map.insert( std::map<std::string, ASOS_Object *>::value_type(in_object->object_id , in_object) );
}

int ASOS_ObjectField::RemoveObject(ASOS_Object *in_object){
}

ASOS_Object *ASOS_ObjectField::FindObject(char object_id, int object_id_length){
}


void ASOS_ObjectField::CleanUpByNodeLeaving(ASOS_Node *in_node){
  printf("Leave node %p on %s\n", in_node, field_id);
}
