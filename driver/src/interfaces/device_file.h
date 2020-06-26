#ifndef CRMNA_DEVICE_FILE_HEADER
#define CRMNA_DEVICE_FILE_HEADER
#include "../common.h"

typedef struct {
  bool (*free)(void *obj, crmna_err_t *err);
} device_file;

typedef struct {
  device_file *interface;
  void *obj;
} device_file_ref;

/**
 * @fn
 * デバイスファイルを破棄します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 破棄に成功した場合はtrue。失敗した場合はfalse。
 */
bool device_file_free(device_file_ref *ref, crmna_err_t *err);

#endif