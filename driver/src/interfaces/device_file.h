#ifndef CRMNA_DEVICE_FILE_HEADER
#define CRMNA_DEVICE_FILE_HEADER
#include "../common.h"

typedef struct {
  void (*free)(void *obj);
} device_file;

typedef struct {
  device_file *interface;
  void *obj;
} device_file_ref;

/**
 * @fn
 * デバイスファイルを破棄します。
 * @param ref インターフェースリファレンス
 */
void device_file_free(device_file_ref *ref);

/**
 * @fn
 * idevice_fileを初期化します。
 * @param ref インターフェースリファレンス
 */
void clear_device_file_ref(device_file_ref *ref);

#endif