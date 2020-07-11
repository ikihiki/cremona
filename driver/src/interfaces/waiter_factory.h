#ifndef CRMNA_WAITER_FACTORY_HEADER
#define CRMNA_WAITER_FACTORY_HEADER
#include "waiter.h"

typedef struct {
  bool (*create_waiter)(void *obj, waiter_ref *lock, crmna_err *err);
} waiter_factory;

typedef struct {
  waiter_factory *interface;
  void *obj;
} waiter_factory_ref;

/**
 * @fn
 * waiterをfactoryから生成します。
 * @param factory 生成に使用するファクトリ
 * @param ref 生成されたwaiterのリファレンス
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool create_waiter(waiter_factory_ref *factory, waiter_ref *ref,
                   crmna_err *err);

#endif