#ifndef CRMNA_ID_MAPPER_FACTORY_HEADER
#define CRMNA_ID_MAPPER_FACTORY_HEADER
#include "id_mapper.h"

typedef struct {
  bool (*create_id_mapper)(void *obj, id_mapper_ref *ref, int min, int max,
                           crmna_err *err);
} id_mapper_factory;

typedef struct {
  id_mapper_factory *interface;
  void *obj;
} id_mapper_factory_ref;

/**
 * @fn
 * id_mapperをfactoryから生成します。
 * @param factory 生成に使用するファクトリ
 * @param ref 生成されたid_mapperのリファレンス
 * @param min 最小ID
 * @param max 最大ID
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool create_id_mapper_range(id_mapper_factory_ref *factory, id_mapper_ref *ref,
                            int min, int max, crmna_err *err);

/**
 * @fn
 * id_mapperをfactoryから生成します。
 * @param factory 生成に使用するファクトリ
 * @param ref 生成されたid_mapperのリファレンス
 * @param min 最小ID
 * @param max 最大ID
 * @param err エラー
 * @return 成功した場合はtrue。エラーの場合はfalse。
 */
bool create_id_mapper(id_mapper_factory_ref *factory, id_mapper_ref *ref,
                      crmna_err *err);

#endif