#ifndef CRMNA_COMMUNICATOR_H
#define CRMNA_COMMUNICATOR_H

#include "../common.h"

typedef void (*reciver_func)(void);

typedef struct communicator {
  int (*send_message)(void *obj, uint32_t pid, int type, crmna_buf_t *buf,
                      crmna_err_t *err);
  bool (*register_reciver_func)(void *obj, reciver_func *func,
                                crmna_err_t *err);
  bool (*free)(void *obj, crmna_err_t *err);
} communicator_t;

typedef struct communicator_ref {
  communicator_t *interface;
  void *obj;
} communicator_ref_t;

typedef struct communicator_factory {
  bool (*create_communicator)(communicator_ref_t *ref, crmna_err_t *err);
} communicator_factory_t;

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
int communicator_send_message(communicator_ref_t *ref, uint32_t pid, int type,
                                     crmna_buf_t *data, crmna_err_t *err);

/**
 * @fn
 * データを送信します。
 * @param ref インターフェースリファレンス
 * @param func 登録する関数
 * @param err エラー
 * @return 送信に成功したバイト数。エラーの場合は-1。
 */
bool communicator_register_reciver_func(communicator_ref_t *ref,
                                               reciver_func *func, crmna_err_t *err);

/**
 * @fn
 * communictorを破棄します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 送信に成功したバイト数。エラーの場合は-1。
 */
bool communicator_free(communicator_ref_t *ref,
                              crmna_err_t *err);

/**
 * @fn
 * communicatorをfactoryから生成します。
 * @param factory 生成に使用するファクトリ
 * @param ref 生成されたcomunicatorのリファレンス
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool create_communicator(communicator_factory_t *factory, communicator_ref_t *ref,
                                crmna_err_t *err);

#endif