#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <glad/glad.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "resourcemanager.h"
#include "spriterenderer.h"
#include "level.h"
#include "particle.h"
#include "postprocess.h"
#include "textrenderer.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

typedef struct Game {
    enum GameState State; 
    bool Keys[1024];
    unsigned int Width, Height; 
    unsigned int lives;
} Game; 



Game create_game(unsigned int width, unsigned int height);

void init_game(Game *game);

void process_input(Game *game, float dt);

void update(Game *game, float dt);

void render_game(Game *game);

#endif // !GAME_H