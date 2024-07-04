#ifndef OBJECT_H
#define OBJECT_H

#include <cglm/cglm.h>
#include <cglm/types-struct.h>


#include "texture.h"
#include "spriterenderer.h"


enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

enum PowerUpType {
    CHAOS, 
    SPEED,
    STICKY,
    PASS,
    PAD_INC,
    CONFUSE,
};

typedef struct Collision {
    bool collision;
    enum Direction dir;
    vec2s diff_vec;
} Collision;


typedef struct Object
{
    vec2s position;
    vec2s size;
    vec2s velocity;
    vec3s color;
    float rotation;
    bool isSolid;
    bool destroyed;

    Texture2D *sprite;

} Object;


//TODO Make ball and powerups in their own file


typedef struct BallObject {
    Object *o;
    float radius; 
    bool stuck; 
    bool sticky;
    bool pass;
} BallObject;



typedef struct PowerUp {
    enum PowerUpType type; 
    float duration; 
    bool activated;
    Object *o;
} PowerUp;


Object *create_object(vec2s pos, vec2s size, vec2s velocity, vec3s color, float rotation, bool isSolid, bool destroyed, Texture2D *texture);
void draw_object(Object *object, Shader shader);

BallObject *create_ball(Object *o, float radius); 
void move_ball(BallObject *ball, float dt, unsigned int window_w);

PowerUp *create_power_up(enum PowerUpType type, vec3s color, float duration, vec2s position, Texture2D *texture);

void reset_ball(BallObject *ball, vec2s pos);

Collision check_ball_collision(BallObject *one, Object *two);
bool check_powerup_collision(Object *one, PowerUp *two);

#endif // !OBJECT_H
