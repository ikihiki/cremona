#ifndef CRMNA_LOCKER_FACTORY_HEADER
#define CRMNA_LOCKER_FACTORY_HEADER
#include "locker.h"

typedef struct {
  bool (*create_locker)(void *obj, locker_ref *lock, crmna_err *err);
} locker_factory;

typedef struct {
  locker_factory *interface;
  void *obj;
} locker_factory_ref;

/**
 * @fn
 * lockerをfactoryから生成します。
 * @param factory 生成に使用するファクトリ
 * @param ref 生成されたlockerのリファレンス
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool create_locker(locker_factory_ref *factory, locker_ref *ref,
                   crmna_err *err);

#endif