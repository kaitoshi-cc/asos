#ifndef ASOS_OBJECTFIELD_HPP
#define ASOS_OBJECTFIELD_HPP

#include <map>
#include <string>

#include "ASOS_Object.hpp"
class ASOS_Node;

class ASOS_ObjectField{
public:
  ASOS_ObjectField();
  ~ASOS_ObjectField();
  
  char field_id[256];
  int field_id_length;

  int Process(ASOS_message *in_msg, ASOS_message *in_res_msg, ASOS_Node *in_node);
  
  int AddObject(ASOS_Object *in_object, ASOS_Node *in_node);
  int RemoveObject(ASOS_Object *in_object);
  ASOS_Object *FindObject(char *object_id, int object_id_length);
  
  void CleanUpByNodeLeaving(ASOS_Node *in_node);

private:
  std::map<std::string, ASOS_Object*> object_map;
  
};

#endif
