#ifndef CRMNA_WAITER_HEADER
#define CRMNA_WAITER_HEADER
#include "../common.h"

typedef bool (*cond_func)(void *context);

typedef struct {
  bool (*wait)(void *obj, cond_func cond, void *context, int msec,
               crmna_err *err);
  void (*notify)(void *obj);
  void (*free)(void *obj);
} waiter;

typedef struct {
  waiter *interface;
  void *obj;
} waiter_ref;

/**
 * @fn
 * ロックを取得します。
 * @param ref インターフェースリファレンス
 * @param cond 起床する条件
 * @param context 条件関数のコンテキスト
 * @param msec タイムアウト時間(msec)
 * @param err エラー
 * @return 取得に成功した場合はtrue。失敗した場合はfalse。
 */
bool waiter_wait(waiter_ref *ref, cond_func cond, void *context, int msec,
                 crmna_err *err);

/**
 * @fn
 * ロックを解放します。
 * @param ref インターフェースリファレンス
 */
void waiter_notify(waiter_ref *ref);

/**
 * @fn
 * ロックを破棄します。
 * @param ref インターフェースリファレンス
 */
void waiter_free(waiter_ref *ref);

/**
 * @fn
 * waiterを初期化します。
 * @param ref インターフェースリファレンス
 */
void clear_waiter_ref(waiter_ref *ref);

#endif