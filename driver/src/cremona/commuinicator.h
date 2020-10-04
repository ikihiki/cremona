#ifndef CRMNA_COMMUNICATOR_H
#define CRMNA_COMMUNICATOR_H

#include "error.h"
#include "buffer.h"

typedef struct {
  bool (*send_message)(void *obj, uint32_t pid, int type, crmna_buf_t *buf,
                         crmna_err_t *err);
  void (*free)(void *obj);
} communicator_t;

typedef struct {
  communicator_t *interface;
  void *obj;
} communicator_ref_t;

/**
 * @fn
 * データを送信します。
 * @param ref インターフェースリファレンス
 * @param pid 送信先プロセスのpid
 * @param type 送信するデータタイプ
 * @param data 送信するデータ
 * @param err エラー
 * @return 送信に成功したかどうか
 */
bool communicator_send_message(communicator_ref_t *ref, uint32_t pid, int type,
                              crmna_buf_t *data, crmna_err_t *err);

/**
 * @fn
 * communictorを破棄します。
 * @param ref インターフェースリファレンス
 * @note NULLポインタなど不完全な状態でも使用可
 */
void communicator_free(communicator_ref_t *ref);

/**
 * @fn
 * communictorを初期化します。
 * @param ref インターフェースリファレンス
 */
void clear_communicator_ref(communicator_ref_t *ref);

#endif