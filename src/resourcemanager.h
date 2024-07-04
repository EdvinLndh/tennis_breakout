#ifndef RES_MANAGER_H
#define RES_MANAGER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "shader.h"
#include "texture.h"

void init_res_manager();

Texture2D* get_texture(char *name); 

void add_texture(char *name, Texture2D *texture);


#endif // !RES_MANAGER_H