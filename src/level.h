#ifndef LEVEL_H
#define LEVEL_H

#include <stdio.h>
#include <stdlib.h>

#include "spriterenderer.h"
#include "shader.h"
#include "object.h"
#include "resourcemanager.h"

#define MAX_LINE 256
#define MAX_ROWS 32


typedef struct TileRow {
    int tiles[MAX_LINE];
    unsigned int len;
} TileRow;

typedef struct GameLevel {
    TileRow rows[MAX_ROWS];
    unsigned int len; 
    Object *bricks; 
    unsigned int b_counter;
} GameLevel;

GameLevel load_level(const char *path, unsigned int width, unsigned int height);

void draw_level(GameLevel *level, Shader shader);

bool level_complete();


void reset_level(GameLevel *level); 

#endif // !LEVEL_H
