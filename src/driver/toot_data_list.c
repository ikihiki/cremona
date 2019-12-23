#include "header.h"

void init_toot_data_root(struct toot_data_root *list) {
  init_rwsem(&list->sem);
  list->data = NULL;
}
void add_toot_data(struct toot_data_root *list, struct toot_data *data) {
  down_write(&list->sem);
  if (list->data != NULL) {
    data->next = list->data;
  }
  list->data = data;
  data->prev = NULL;
  up_write(&list->sem);
}
void delete_toot_data(struct toot_data_root *list, struct toot_data *data) {
  down_write(&list->sem);
  if (list->data == data) {
    list->data = data->next;
    list->data->prev = NULL;
  } else {
    data->prev->next = data->next;
    data->next->prev = data->prev;
  }
  up_write(&list->sem);
}

int compare_uuid(const unsigned char a[16], const unsigned char b[16]) {
  for (int i = 0; i < 16; i++) {
    if (a[i] != b[i]) {
      return 1;
    }
  }
  return 0;
}

struct toot_data *serch_toot_data(const struct toot_data_root *list,
                                  const unsigned char uuid[16]) {
  down_read(&list->sem);
  struct toot_data *data = list->data;
  while (data != NULL) {
    if (compare_uuid(data->uuid, uuid) == 0) {
      break;
    }
    data = data->next;
  }
  up_read(&list->sem);
  return data;
}
