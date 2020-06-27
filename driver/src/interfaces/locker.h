#ifndef CRMNA_LOCKER_HEADER
#define CRMNA_LOCKER_HEADER
#include "../common.h"

typedef struct {
  void (*lock)(void *obj);
  void (*unlock)(void *obj);
  void (*free)(void *obj);
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
 */
void locker_lock(locker_ref *ref);

/**
 * @fn
 * ロックを解放します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 */
void locker_unlock(locker_ref *ref);

/**
 * @fn
 * ロックを破棄します。
 * @param ref インターフェースリファレンス
 */
void locker_free(locker_ref *ref);

/**
 * @fn
 * lockerを初期化します。
 * @param ref インターフェースリファレンス
 */
void clear_locker_ref(locker_ref *ref);

#endif