#ifndef CRMNA_WAITER_HEADER
#define CRMNA_WAITER_HEADER
#include "../common.h"

typedef bool (*cond_func)(void *context);

typedef struct {
  bool (*wait)(void *obj, cond_func cond, void *context, int msec, crmna_err_t *err);
  bool (*notify)(void *obj, crmna_err_t *err);
  bool (*free)(void *obj, crmna_err_t *err);
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
                 crmna_err_t *err);

/**
 * @fn
 * ロックを解放します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 解放に成功した場合はtrue。失敗した場合はfalse。
 */
bool waiter_notify(waiter_ref *ref, crmna_err_t *err);

/**
 * @fn
 * ロックを破棄します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 破棄に成功した場合はtrue。失敗した場合はfalse。
 */
bool waiter_free(waiter_ref *ref, crmna_err_t *err);

/**
 * @fn
 * waiterを初期化します。
 * @param ref インターフェースリファレンス
 */
void clear_waiter_ref(waiter_ref *ref);

#endif