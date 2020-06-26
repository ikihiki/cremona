#ifndef CRMNA_CPMMUNICATOR_FACTORY_HEADER
#define CRMNA_CPMMUNICATOR_FACTORY_HEADER
#include "commuinicator.h"
#include "../common.h"

typedef struct {
  bool (*create_communicator)(void *obj, cremona_device_manager_t *device_manager, communicator_ref *ref,  crmna_err_t *err);
} communicator_factory;

typedef struct {
  communicator_factory *interface;
  void *obj;
} communicator_factory_ref;

/**
 * @fn
 * communicatorをfactoryから生成します。
 * @param factory 生成に使用するファクトリ
 * @param device_manager 受信用device_manager
 * @param ref 生成されたcomunicatorのリファレンス
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool create_communicator(communicator_factory_ref *factory,
                         cremona_device_manager_t *device_manager,
                         communicator_ref *ref, crmna_err_t *err);

#endif