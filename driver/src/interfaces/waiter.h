#ifndef CRMNA_WAITER_HEADER
#define CRMNA_WAITER_HEADER
#include "../common.h"

typedef struct waiter {
  bool (*wait)(void *obj, crmna_err_t *err);
  bool (*notify)(void *obj, crmna_err_t *err);
  bool (*free)(void *obj, crmna_err_t *err);
} waiter_t;

typedef struct waiter_ref {
  waiter_t *interface;
  void *obj;
} waiter_ref_t;

typedef struct waiter_factory {
      bool (*create_waiter)(waiter_ref_t *lock, crmna_err_t *err);

} waiter_factory_t;

/**
 * @fn
 * ロックを取得します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 取得に成功した場合はtrue。失敗した場合はfalse。
 */
bool waiter_wait(waiter_ref_t *ref, crmna_err_t *err);

/**
 * @fn
 * ロックを解放します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 解放に成功した場合はtrue。失敗した場合はfalse。
 */
bool waiter_notify(waiter_ref_t *ref, crmna_err_t *err);

/**
 * @fn
 * ロックを破棄します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 破棄に成功した場合はtrue。失敗した場合はfalse。
 */
bool waiter_free(waiter_ref_t *ref, crmna_err_t *err);

/**
 * @fn
 * waiterをfactoryから生成します。
 * @param factory 生成に使用するファクトリ
 * @param ref 生成されたwaiterのリファレンス
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool create_waiter(waiter_factory_t *factory, waiter_ref_t *ref,
                          crmna_err_t *err);
#endif