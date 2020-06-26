#ifndef CRMNA_ID_MAPPER_HEADER
#define CRMNA_ID_MAPPER_HEADER

#include "../common.h"

typedef struct {
  bool (*next)(void *obj, crmna_err_t *err);
  void *(*get_value)(void *obj, crmna_err_t *err);
  bool *(*free)(void *obj, crmna_err_t *err);
} id_mapper_iterator;

typedef struct {
  id_mapper_iterator *interface;
  void *obj;
} id_mapper_iterator_ref;

typedef struct {
  bool (*add_get_id)(void *obj, void *target, int *id, crmna_err_t *err);
  void *(*find)(void *obj, int id, crmna_err_t *err);
  bool (*remove)(void *obj, int id, crmna_err_t *err);
  bool (*get_iterator)(void *obj, id_mapper_iterator_ref *iterator,
                       crmna_err_t *err);
  bool (*free)(void *obj, crmna_err_t *err);
} id_mapper;

typedef struct {
  id_mapper *interface;
  void *obj;
} id_mapper_ref;


/**
 * @fn
 * iteratorの参照先を進めます。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 生成されたid。エラーの場合は-1。
 * @detail iteratorが最後に到達したらerrは空でfalseを返す。
 */
bool id_mapper_iterator_next(id_mapper_iterator_ref *ref, crmna_err_t *err);

/**
 * @fn
 * 現在iteratorが参照している値を取得します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 現在iteratorが参照しているポインタ。エラーの場合はNULL。
 */
void *id_mapper_iterator_get_value(id_mapper_iterator_ref *ref,
                                          crmna_err_t *err);
/**
 * @fn
 * iteratorを破棄します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool id_mapper_iterator_free(id_mapper_iterator_ref *ref,
                                    crmna_err_t *err);

/**
 * @fn
 * 新しいポインタを登録し、idを生成します。
 * @param ref インターフェースリファレンス
 * @param target 登録するポインタ
 * @param err エラー
 * @return 生成されたid。エラーの場合は-1。
 */
bool id_mapper_add_get_id(id_mapper_ref *ref, void *target, int *id,
                                 crmna_err_t *err);
/**
 * @fn
 * 指定されたidのポインタを取得します。
 * @param ref インターフェースリファレンス
 * @param target 検索するid
 * @param err エラー
 * @return 発見されたポインタ。エラーの場合はNULL。
 */
void *id_mapper_find(id_mapper_ref *ref, int id, crmna_err_t *err);

/**
 * @fn
 * idを削除します
 * @param ref インターフェースリファレンス
 * @param id 削除するid
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool id_mapper_remove(id_mapper_ref *ref, int id, crmna_err_t *err);

/**
 * @fn
 * iteratorを取得します。
 * @param ref インターフェースリファレンス
 * @param iterator output
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool id_mapper_get_iterator(id_mapper_ref *ref,
                                   id_mapper_iterator_ref *iterator,
                                   crmna_err_t *err);
/**
 * @fn
 * mapを破棄します。
 * @param ref インターフェースリファレンス
 * @param err エラー
 * @return 破棄に成功した場合はtrue。失敗した場合はfalse。
 */
bool id_mapper_free(id_mapper_ref *ref, crmna_err_t *err);

/**
 * @fn
 * id_mapperを初期化します。
 * @param ref インターフェースリファレンス
 */
void clear_id_mapper_ref(id_mapper_ref *ref);

#endif