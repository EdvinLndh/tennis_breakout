#include "resourcemanager.h"

#define MAX_TEXTURES 20

HashMap *textureMap;

void init_res_manager() {
    textureMap = createHashMap(MAX_TEXTURES, stringHashFunction, stringCompareFunction);
}

Texture2D* get_texture(char *name) {
    return search(textureMap, name);
} 

void add_texture(char *name, Texture2D *texture) {
    insert(textureMap, name, texture);
}

