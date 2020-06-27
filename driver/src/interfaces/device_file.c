#include "device_file.h"

void device_file_free(device_file_ref *ref){
   ref->interface->free(ref->obj);
   ref->interface = NULL;
   ref->obj = NULL;
}

void clear_device_file_ref(device_file_ref *ref) { 
  ref->interface = NULL;
  ref->obj = NULL;
}