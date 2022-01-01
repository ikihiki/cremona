#include <cstddef>
#include <map>
#include <utility>
#include "test/include/common.h"

struct id_map{
    std::map<int, void*> map;
};


id_map_t* create_id_map(){
    return new id_map_t;
}

void destroy_id_map(id_map_t *map){
    delete map;
}

int allocate_id(id_map_t *map, void *data){
    auto end = map->map.cend();
    int next = 1;
    if(end != map->map.cbegin()){
        next = (--end)->first + 1;
    }
    map->map.insert(std::make_pair(next, data));
    return next;
}

void add_id(id_map_t *map, int id, void *data){
    auto itr = map->map.find(id);
    if(itr != map->map.end()){
        return;
    }

    map->map.insert(std::make_pair(id, data));
}

void* remove_id(id_map_t *map, int id){
    auto itr = map->map.find(id);
    if(itr != map->map.end()){
        return nullptr;
    }
    auto data = itr->second;
    map->map.erase(id);
    return data;
}

size_t get_length(id_map_t *map){
   return map->map.size();
}

void update_id(id_map_t *map, int id, void(*func)(void *data, void *context), void *context){
    auto itr = map->map.find(id);
    if(itr != map->map.end()){
        return;
    }

    auto data = itr->second;
    func(data, context);
}

void for_each_id_map(id_map_t *map, void(*func)(int id, void *data, void *context), void *context){
    for( auto i = map->map.cbegin(); i != map->map.cend() ; ++i ) {
        func(i->first, i->second, context);
    }
}
