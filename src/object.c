#include "object.h"

static enum Direction VectorDirection(vec2s target);
// The size of a PowerUp block
const vec2s PU_SIZE = (vec2s) {{60.0f, 20.0f}};
// Velocity a PowerUp block has when spawned
const vec2s PU_VELOCITY = (vec2s) {{0.0f, 150.0f}};

Object *create_object(vec2s pos, vec2s size, vec2s velocity, vec3s color, float rotation, bool isSolid, bool destroyed, Texture2D *texture) {
    Object *o = malloc(sizeof(Object));
    if (o == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    o->position = pos;
    o->size = size;
    o->velocity = velocity;
    o->color = color;
    o->rotation = rotation;
    o->isSolid = isSolid;
    o->destroyed = destroyed;
    o->sprite = texture;
    return o;
}

BallObject *create_ball(Object *o, float radius) {
    BallObject *b = malloc(sizeof(BallObject)); 
    if (b == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    b->o = o; 
    b->radius = radius; 
    b->stuck = true;
    b->sticky = false;
    b->pass = false;
    return b;
}

PowerUp *create_power_up(enum PowerUpType type, vec3s color, float duration, vec2s position, Texture2D *texture) {
    PowerUp *pu = malloc(sizeof(PowerUp));
    if (pu == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    pu->type = type;
    pu->duration = duration; 
    pu->activated = false;
    pu->o = create_object(position, PU_SIZE, PU_VELOCITY, color, 0.0, false, false, texture);
    return pu;
}

void draw_object(Object *object, Shader shader) {
    use_shader(shader);
    if (!object->destroyed)
        draw_sprite(shader, object->sprite, object->position, object->size, object->rotation, object->color);
}

void move_ball(BallObject *ball, float dt, unsigned int window_w) {

    Object *o = ball->o;
    // if not stuck to player board
    if (!ball->stuck)
    { 
        // move the ball
        o->position = glms_vec2_add(o->position, glms_vec2_scale(o->velocity, dt));

        o->rotation = o->position.y;
        // check if outside window bounds; if so, reverse velocity and restore at correct position
        if (o->position.x <= 0.0f)
        {
            o->velocity.x = -o->velocity.x;

            o->position.x = 0.0f;
        }
        else if (o->position.x + o->size.x >= window_w)
        {
            o->velocity.x = -o->velocity.x;
            o->position.x = window_w - o->size.x;
        }
        if (o->position.y <= 0.0f)
        {
            o->velocity.y = -o->velocity.y;
            o->position.y = 0.0f;
        }
      
    }
}

void reset_ball(BallObject *ball, vec2s pos) {
    ball->o->position = pos; 
    ball->stuck = true;
}


Collision check_ball_collision(BallObject *one, Object *two) // AABB - AABB collision
{

    vec2s center = {{ one->o->position.x + one->radius, one->o->position.y + one->radius}};
    vec2s aabb_half_extents = {{two->size.x / 2.0f, two->size.y / 2.0f}};

    vec2s aabb_center = {{two->position.x + aabb_half_extents.x, two->position.y + aabb_half_extents.y}};

    vec2s diff = glms_vec2_sub(center, aabb_center);
    vec2s clamp = {{glm_clamp(diff.x, -aabb_half_extents.x, aabb_half_extents.x), glm_clamp(diff.y, -aabb_half_extents.y, aabb_half_extents.y)}};

    vec2s closest = glms_vec2_add(aabb_center, clamp);
    diff = glms_vec2_sub(closest, center);
    if (glms_vec2_norm(diff) < one->radius) 
        return (Collision) { .collision = true, .diff_vec = diff, .dir = VectorDirection(diff)};
    else
        return (Collision) { .collision = false, .diff_vec = glms_vec2_zero(), .dir = UP};
}  

bool check_powerup_collision(Object *one, PowerUp *two) {

    // collision x-axis?
    bool collisionX = one->position.x + one->size.x >= two->o->position.x &&
        two->o->position.x + two->o->size.x >= one->position.x;
    // collision y-axis?
    bool collisionY = one->position.y + one->size.y >= two->o->position.y &&
        two->o->position.y + two->o->size.y >= one->position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}



static enum Direction VectorDirection(vec2s target)
{
    vec2s compass[] = {
        (vec2s){{0.0f, 1.0f}},	// up
        (vec2s){{1.0f, 0.0f}},	// right
        (vec2s){{0.0f, -1.0f}},	// down
        (vec2s){{-1.0f, 0.0f}}	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glms_vec2_dot(glms_vec2_normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (enum Direction)best_match;
}    