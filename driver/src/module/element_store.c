#include "module.h"
#include "store_internal.h"

element_store_t *get_element_when_ready(store_t *store, uint32_t element_id,
                                        crmna_err_t *err) {
  element_store_t *element =
      (element_store_t *)idr_find(&store->elements, element_id);
  if (element == NULL) {
    ADD_ERROR(err, "canot find element id. element id: %d", element_id);
    return NULL;
  }

  spin_lock(&element->spinlock);
  if (!(element->state == ELEMENT_READY ||
        element->state == ELEMENT_SEND_WAIT)) {
    spin_unlock(&element->spinlock);
    ADD_ERROR(err, "element is not ready. element id: %d", element_id);
    return NULL;
  }
  uint32_t toot_id = element->toot_id;
  spin_unlock(&element->spinlock);

  if (!check_toot_ready(store, toot_id, err)) {
    ADD_ERROR(err, "toot is not ready. toot id: %d, element id: %d", toot_id, element_id);
    return NULL;
  }
  return element;
}

bool add_element(store_t *store, uint32_t toot_id, uint32_t *element_id, uint32_t *index,
                 crmna_err_t *err) {
  if (!check_toot_ready(store, toot_id, err)) {
    ADD_ERROR(err, "toot is not ready. toot id: %d", toot_id);
    return false;
  }

  idr_preload(GFP_KERNEL);
  spin_lock(&store->elements_lock);
  uint32_t id = 0;
  int allocate_result =
      idr_alloc_u32(&store->elements, NULL, &id, INT_MAX, GFP_KERNEL);
  spin_unlock(&store->elements_lock);
  idr_preload_end();
  switch (allocate_result) {
  case 0:
    break;
  case -ENOMEM:
    ADD_ERROR(err, "canot allocate element. because no memory. toot id: %d",
              toot_id);
    return false;
  case -ENOSPC:
    ADD_ERROR(err, "canot allocate element. because no id space. toot id: %d",
              toot_id);
    return false;
  default:
    ADD_ERROR(err,
              "canot allocate element. because something worng. toot id: %d",
              toot_id);
    return false;
  }

  element_store_t *element = kmalloc(sizeof(element_store_t), GFP_KERNEL);
  if (element == NULL) {
    spin_lock(&store->elements_lock);
    idr_remove(&store->elements, id);
    spin_unlock(&store->elements_lock);
    ADD_ERROR(err, "canot allocate element. fail malloc. toot id: %d", toot_id);
    return false;
  }

  toot_store_t *toot = (toot_store_t *)idr_find(&store->toots, toot_id);
  if (toot == NULL) {
    spin_lock(&store->elements_lock);
    idr_remove(&store->elements, id);
    spin_unlock(&store->elements_lock);
    kfree(element);
    ADD_ERROR(err, "canot find toot. toot id: %d", toot_id);
    return false;
  }

  spin_lock(&toot->spinlock);
  *index = toot->element_count = toot->element_count + 1;
  spin_unlock(&toot->spinlock);

  element->element_id = id;
  element->toot_id = toot_id;
  element->state = ELEMENT_READY;
  element->index = *index;
  init_waitqueue_head(&element->wait_head);
  spin_lock_init(&element->spinlock);
  idr_replace(&store->elements, element, id);
  *element_id = id;
  return true;
}

void remove_element(store_t *store, uint32_t element_id) {
  element_store_t *element =
      (element_store_t *)idr_find(&store->elements, element_id);
  if (element == NULL) {
    return;
  }
  idr_remove(&store->elements, element_id);
  spin_lock(&element->spinlock);
  element->state = ELEMENT_CREANUPED;
  spin_unlock(&element->spinlock);
  wake_up_interruptible(&element->wait_head);

  kfree(element);
}

bool wait_element_sent_or_failer(store_t *store, uint32_t element_id,
                                 crmna_err_t *err) {
  element_store_t *element = get_element_when_ready(store, element_id, err);
  if (element == NULL) {
    ADD_ERROR(err, "canot find element. element id: %d", element_id);
    return false;
  }

  wait_event_interruptible_timeout(element->wait_head,
                                   element->state == ELEMENT_SENT ||
                                       element->state == ELEMENT_CREANUPED ||
                                       element->state == ELEMENT_ERROR,
                                   1000 * HZ / 1000);

  if(element->state != ELEMENT_SENT){
    ADD_ERROR(err, "wake up with failure state. element id: %d state: %d", element_id,
              element->state);
    return false;
  }
  return true;
}

void set_element_sent(store_t *store, uint32_t element_id) {
  DEFINE_ERROR(err);
  element_store_t *element = get_element_when_ready(store, element_id, &err);
  if (element == NULL) {
    printk_err(&err);
    return;
  }

  spin_lock(&element->spinlock);
  element->state = ELEMENT_SENT;
  spin_unlock(&element->spinlock);

  wake_up_interruptible(&element->wait_head);
}

void set_element_failer(store_t *store, uint32_t element_id) {
  DEFINE_ERROR(err);
  element_store_t *element = get_element_when_ready(store, element_id, &err);
  if (element == NULL) {
    printk_err(&err);
    return;
  }

  spin_lock(&element->spinlock);
  element->state = ELEMENT_ERROR;
  spin_unlock(&element->spinlock);

  wake_up_interruptible(&element->wait_head);
}
