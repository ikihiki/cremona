#ifndef CRMNA_DEVICE_FILE_FACTORY_HEADER
#define CRMNA_DEVICE_FILE_FACTORY_HEADER
#include "device_file.h"

typedef struct {
  bool (*create_device_file)(void *obj, cremona_device_t *device, device_file_ref *lock, crmna_err_t *err);
} device_file_factory;

typedef struct {
  device_file_factory *interface;
  void *obj;
} device_file_factory_ref;

/**
 * @fn
 * device_fileをfactoryから生成します。
 * @param factory 生成に使用するファクトリ
 * @param device イベント受信用device
 * @param ref 生成されたdevice_fileのリファレンス
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool create_device_file(device_file_factory_ref *factory,
                        cremona_device_t *device, device_file_ref *ref,
                        crmna_err_t *err);

#endif