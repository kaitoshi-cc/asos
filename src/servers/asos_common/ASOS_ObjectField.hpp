#ifndef ASOS_OBJECTFIELD_HPP
#define ASOS_OBJECTFIELD_HPP

#include "ASOS_Object.hpp"
class ASOS_Node;

class ASOS_ObjectField{
public:
  ASOS_ObjectField();
  ~ASOS_ObjectField();

  char field_id[256];
  int field_id_length;

  ASOS_Object *object_list;

  int AddObject(ASOS_Object *in_object, ASOS_Node *in_node);
  int RemoveObject(ASOS_Object *in_object);
  ASOS_Object *FindObject(char object_id, int object_id_length);
  
  void CleanUpByNodeLeaving(ASOS_Node *in_node);

  
};

#endif
