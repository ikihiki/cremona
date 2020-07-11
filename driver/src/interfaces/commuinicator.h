#ifndef CRMNA_COMMUNICATOR_H
#define CRMNA_COMMUNICATOR_H

#include "../common.h"

typedef struct {
  int (*send_message)(void *obj, uint32_t pid, int type, crmna_buf_t *buf,
                      crmna_err *err);
  void (*free)(void *obj);
} communicator;

typedef struct {
  communicator *interface;
  void *obj;
} communicator_ref;

/**
 * @fn
 * データを送信します。
 * @param ref インターフェースリファレンス
 * @param pid 送信先プロセスのpid
 * @param type 送信するデータタイプ
 * @param data 送信するデータ
 * @param err エラー
 * @return 送信に成功したバイト数。エラーの場合は-1。
 */
int communicator_send_message(communicator_ref *ref, uint32_t pid, int type,
                                     crmna_buf_t *data, crmna_err *err);

/**
 * @fn
 * communictorを破棄します。
 * @param ref インターフェースリファレンス
 * @note NULLポインタなど不完全な状態でも使用可
 */
void communicator_free(communicator_ref *ref);

/**
 * @fn
 * communictorを初期化します。
 * @param ref インターフェースリファレンス
 */
void clear_communicator_ref(communicator_ref *ref);

#endif