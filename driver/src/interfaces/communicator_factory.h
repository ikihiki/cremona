#ifndef CRMNA_CPMMUNICATOR_FACTORY_HEADER
#define CRMNA_CPMMUNICATOR_FACTORY_HEADER
#include "commuinicator.h"
#include "../common.h"

typedef struct {
  bool (*create_communicator)(void *obj, communicate_t *communicate,
                              communicator_ref *ref, crmna_err *err);
} communicator_factory;

typedef struct {
  communicator_factory *interface;
  void *obj;
} communicator_factory_ref;

/**
 * @fn
 * communicatorをfactoryから生成します。
 * @param factory 生成に使用するファクトリ
 * @param communicate 受信用communicate
 * @param ref 生成されたcomunicatorのリファレンス
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool create_communicator(communicator_factory_ref *factory,
                         communicate_t *communicate, communicator_ref *ref,
                         crmna_err *err);

#endif