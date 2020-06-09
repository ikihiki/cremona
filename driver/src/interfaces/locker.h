#ifndef CRMNA_LOCKER_HEADER
#define CRMNA_LOCKER_HEADER
#include "../common.h"

typedef struct locker {
  bool (*lock)(void *obj, crmna_err_t *err);
  bool (*unlock)(void *obj, crmna_err_t *err);
  bool (*free)(void *obj, crmna_err_t *err);
} locker_t;

typedef struct locker_ref {
  locker_t *interface;
  void *obj;
} locker_ref_t;

typedef struct locker_factory {
      bool (*create_locker)(locker_ref_t *lock, crmna_err_t *err);

} locker_factory_t;

/**
 * @fn
 * ロックを取得します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 取得に成功した場合はtrue。失敗した場合はfalse。
 */
bool locker_lock(locker_ref_t *ref, crmna_err_t *err);

/**
 * @fn
 * ロックを解放します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 解放に成功した場合はtrue。失敗した場合はfalse。
 */
bool locker_unlock(locker_ref_t *ref, crmna_err_t *err);

/**
 * @fn
 * ロックを破棄します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 破棄に成功した場合はtrue。失敗した場合はfalse。
 */
bool locker_free(locker_ref_t *ref, crmna_err_t *err);

/**
 * @fn
 * lockerをfactoryから生成します。
 * @param factory 生成に使用するファクトリ
 * @param ref 生成されたlockerのリファレンス
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool create_locker(locker_factory_t *factory, locker_ref_t *ref,
                          crmna_err_t *err);

#endif