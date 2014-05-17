#include "ASOS_ObjectField.hpp"
#include <stdio.h>

ASOS_ObjectField::ASOS_ObjectField(){
}

ASOS_ObjectField::~ASOS_ObjectField(){
}

void ASOS_ObjectField::CleanUpByNodeLeaving(ASOS_Node *in_node){
  printf("Leave node %p on %s\n", in_node, field_id);
}
