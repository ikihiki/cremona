#ifndef CRMNA_LOCKER_HEADER
#define CRMNA_LOCKER_HEADER
#include "../common.h"

typedef struct {
  bool (*lock)(void *obj, crmna_err_t *err);
  bool (*unlock)(void *obj, crmna_err_t *err);
  bool (*free)(void *obj, crmna_err_t *err);
} locker;

typedef struct {
  locker *interface;
  void *obj;
} locker_ref;


/**
 * @fn
 * ロックを取得します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 取得に成功した場合はtrue。失敗した場合はfalse。
 */
bool locker_lock(locker_ref *ref, crmna_err_t *err);

/**
 * @fn
 * ロックを解放します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 解放に成功した場合はtrue。失敗した場合はfalse。
 */
bool locker_unlock(locker_ref *ref, crmna_err_t *err);

/**
 * @fn
 * ロックを破棄します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 破棄に成功した場合はtrue。失敗した場合はfalse。
 */
bool locker_free(locker_ref *ref, crmna_err_t *err);

/**
 * @fn
 * lockerを初期化します。
 * @param ref インターフェースリファレンス
 */
void clear_locker_ref(locker_ref *ref);

#endif