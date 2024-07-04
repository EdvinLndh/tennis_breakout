#include "game.h"

static void do_collisions();
static void add_powerup(PowerUp *pu);
static bool should_spawn(unsigned int chance);
static void spawn_powerups(Object *block);
static void update_powerups(float dt);
static void activate_powerup(enum PowerUpType type);

#define VEC2S(x, y) ((vec2s){{x, y}})
#define VEC3S(x, y, z) ((vec3s){{x, y, z}})

#define MAX_TEXT 30
#define MAX_POWERUPS 5

Texture2D texture;
Shader spriteShader;
Shader particleShader;
Shader effectShader;

GameLevel lvl;

const vec2s PLAYER_SIZE = {{100.0f, 20.0f}};
const float PLAYER_VELOCITY = 500.0f;

const vec2s BALL_VELOCITY = {{100.0f, -350.0f}};
const float BALL_RADIUS = 12.5f;

Object *player;
BallObject *ball;

Object *objects = NULL;
unsigned int o_counter = 0;

PostProcessor *effects;
float shake_time;

PowerUp *powerups[MAX_POWERUPS];
unsigned int pu_counter = 0;

char text_output[MAX_TEXT];

void init_game(Game *game)
{
    init_res_manager();
    spriteShader = compile("shaders/sprite.vs", "shaders/sprite.fs", NULL);
    particleShader = compile("shaders/particleShader.vs", "shaders/particleShader.fs", NULL);
    effectShader = compile("shaders/postProcShader.vs", "shaders/postProcShader.fs", NULL);

    mat4s projection = glms_ortho(0.0f, game->Width, game->Height, 0.0f, -1.0f, 1.0f);
    use_shader(spriteShader);
    set_int(spriteShader, "image", 0);
    set_mat4(spriteShader, "projection", projection);
    use_shader(particleShader);
    set_mat4(particleShader, "projection", projection);
    use_shader(spriteShader);

    init_render_data();
    init_particles();
    init_text(game->Width, game->Height);

    load_face("fonts/OCRAEXT.TTF", 28);

    add_texture("background", load_texture("resources/background.jpg", false));
    add_texture("awesomeface", load_texture("resources/ball.png", true));
    add_texture("block", load_texture("resources/block.png", true));
    add_texture("block_solid", load_texture("resources/block_solid.png", true));
    add_texture("paddle", load_texture("resources/paddle.png", true));
    add_texture("powerup_speed", load_texture("resources/powerup_speed.png", true));
    add_texture("powerup_confuse", load_texture("resources/powerup_confuse.png", true));
    add_texture("powerup_chaos", load_texture("resources/powerup_chaos.png", true));
    add_texture("powerup_pass", load_texture("resources/powerup_passthrough.png", true));
    add_texture("powerup_sticky", load_texture("resources/powerup_sticky.png", true));
    add_texture("powerup_inc", load_texture("resources/powerup_increase.png", true));

    lvl = load_level("levels/one.lvl", game->Width, game->Height / 2);
    player = create_object((vec2s){{game->Width / 2 - PLAYER_SIZE.x / 2.0f, game->Height - PLAYER_SIZE.y}}, PLAYER_SIZE, (vec2s){{0.0f, 0.0f}},
                           (vec3s){{1.0f, 1.0f, 1.0f}}, 0, true, false, get_texture("paddle"));

    vec2s ballPos = glms_vec2_add(player->position, (vec2s){{PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f}});
    Object *b = create_object(ballPos, (vec2s){{BALL_RADIUS * 2.0f, BALL_RADIUS * 2.0f}}, BALL_VELOCITY, glms_vec3_one(), 0.0f, true, false, get_texture("awesomeface"));
    ball = create_ball(b, BALL_RADIUS);

    effects = create_post_processor(effectShader, game->Width, game->Height);
    game->State = GAME_ACTIVE;
}

void process_input(Game *game, float dt)
{
    if (game->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (game->Keys[GLFW_KEY_A])
        {
            if (player->position.x >= 0.0f)
                player->position.x -= velocity;
            if (ball->stuck)
                ball->o->position.x -= velocity;
        }
        if (game->Keys[GLFW_KEY_D])
        {
            if (player->position.x <= game->Width - player->size.x)
                player->position.x += velocity;
            if (ball->stuck)
                ball->o->position.x += velocity;
        }
        if (game->Keys[GLFW_KEY_SPACE])
        {
            ball->stuck = false;
        }
    }
}

void update(Game *game, float dt)
{
    move_ball(ball, dt, game->Width);
    do_collisions(game);
    update_particles(dt, ball, (vec2s){{ball->radius / 2.0f, ball->radius / 2.0f}});
    update_powerups(dt);

    // Check death
    if (ball->o->position.y >= game->Height)
    {
        if (--game->lives == 0)
        {
            reset_level(&lvl);
            game->lives = 3;
        }
        reset_ball(ball, glms_vec2_add(player->position, (vec2s){{PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f}}));
        effects->chaos = false;
        effects->confuse = false;
    }

    if (shake_time > 0.0f)
    {
        shake_time -= dt;
        if (shake_time <= 0.0f)
            effects->shake = false;
    }
}

void render_game(Game *game)
{
    if (game->State == GAME_ACTIVE)
    {
        begin_render(effects);
        draw_sprite(spriteShader, get_texture("background"), (vec2s){{0.0f, 0.0f}}, (vec2s){{game->Width, game->Height}}, 0.0f, (vec3s){{1.0f, 1.0f, 1.0f}});
        draw_level(&lvl, spriteShader);
        draw_object(player, spriteShader);
        draw_object(ball->o, spriteShader);
        draw_particle(particleShader);

        for (int i = 0; i < pu_counter; i++)
        {
            draw_object(powerups[i]->o, spriteShader);
        }
        end_render(effects);

        render_effects(effects, glfwGetTime());
        sprintf(text_output, "Lives: %d", game->lives);
        render_text(text_output, 5.0f, 5.0f, 1.0f, VEC3S(1.0f, 1.0f, 1.0f));
    }
}

static void do_collisions(Game *game)
{
    for (unsigned int i = 0; i < lvl.b_counter; ++i)
    {
        Object box = lvl.bricks[i];
        if (!box.destroyed)
        {
            Collision collision = check_ball_collision(ball, &box);
            if (collision.collision)
            {
                if (!box.isSolid)
                {
                    box.destroyed = true;
                    spawn_powerups(&box);
                }
                else
                {
                    effects->shake = true;
                    shake_time = 0.5f;
                }

                if (collision.dir == LEFT || collision.dir == RIGHT)
                {
                    ball->o->velocity.x *= -1;
                    float penetration = ball->radius - fabs(collision.diff_vec.x);
                    if (collision.dir == LEFT)
                    {
                        ball->o->position.x += penetration;
                    }
                    else
                    {
                        ball->o->position.x -= penetration;
                    }
                }
                else
                {
                    ball->o->velocity.y *= -1;
                    float penetration = ball->radius - fabs(collision.diff_vec.y);
                    if (collision.dir == DOWN)
                    {
                        ball->o->position.x += penetration;
                    }
                    else
                    {
                        ball->o->position.x -= penetration;
                    }
                }
            }
        }
        lvl.bricks[i] = box;
    }

    Collision result = check_ball_collision(ball, player);

    if (!ball->stuck && result.collision)
    {
        float centerBoard = player->position.x + player->size.x / 2.0f;
        float distance = (ball->o->position.x + ball->radius) - centerBoard;
        float percentage = distance / (player->size.x / 2.0f);

        float str = 2.0f;
        vec2s old_velocity = ball->o->velocity;
        ball->o->velocity.x = BALL_VELOCITY.x * percentage * str;

        ball->o->velocity.y = -1 * fabs(ball->o->velocity.y);
        ball->o->velocity = glms_vec2_scale(glms_vec2_normalize(ball->o->velocity), glms_vec2_norm(old_velocity));
        ball->stuck = ball->sticky;
    }

    for (int i = 0; i < pu_counter; i++)
    {
        PowerUp *powerUp = powerups[i];
        if (!powerUp->o->destroyed)
        {
            if (powerUp->o->position.y >= game->Height)
                powerUp->o->destroyed = true;
            if (check_powerup_collision(player, powerUp))
            { // collided with player, now activate powerup
                activate_powerup(powerUp->type);
                powerUp->o->destroyed = true;
                powerUp->activated = true;
            }
        }
        else
        {
        }
        powerups[i] = powerUp;
    }
}

static bool should_spawn(unsigned int chance)
{
    return (rand() % chance) == 0;
}
static void spawn_powerups(Object *block)
{
    if (should_spawn(15))
        add_powerup(create_power_up(SPEED, (vec3s){{0.5f, 0.5f, 1.0f}}, 0.0f, block->position, get_texture("powerup_speed")));
    if (should_spawn(15))
        add_powerup(create_power_up(STICKY, VEC3S(1.0f, 0.5f, 1.0f), 20.0f, block->position, get_texture("powerup_sticky")));

    // if (ShouldSpawn(75))
    //     powerups[pu_counter++] =
    //         PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, tex_pass
    //     ));
    // if (ShouldSpawn(75))
    //     powerups[pu_counter++] =
    //         PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, tex_size
    //     ));

    if (should_spawn(15))
        add_powerup(create_power_up(CHAOS, VEC3S(0.9f, 0.25f, 0.25f), 15.0f, block->position, get_texture("powerup_chaos")));
    if (should_spawn(15))
        add_powerup(create_power_up(CONFUSE, VEC3S(1.0f, 0.25f, 0.25f), 15.0f, block->position, get_texture("powerup_confuse")));
}

static void add_powerup(PowerUp *pu)
{
    if (pu_counter == 0) {
        powerups[pu_counter++] = pu;
        return;
    }

    for (unsigned int i = 0; i < pu_counter && i < MAX_POWERUPS; i++)
    {
        if (powerups[i] != NULL)
        {
            if (powerups[i]->o->destroyed)
            {
                free(powerups[i]->o);
                free(powerups[i]);
                powerups[i] = pu;
            }
        }
        else
        {
            powerups[pu_counter++] = pu;
        }
    }
}

static void activate_powerup(enum PowerUpType type)
{
    if (type == SPEED)
    {
        ball->o->velocity = glms_vec2_scale(ball->o->velocity, 1.2f);
    }
    else if (type == STICKY)
    {
        ball->sticky = true;
        player->color = VEC3S(1.0f, 0.5f, 1.0f);
    }
    else if (type == PASS)
    {
        ball->pass = true;
        ball->o->color = VEC3S(1.0f, 0.5f, 0.5f);
    }
    else if (type == PAD_INC)
    {
        player->size.x += 50;
    }
    else if (type == CONFUSE)
    {
        if (!effects->chaos)
            effects->confuse = true; // only activate if chaos wasn't already active
    }
    else if (type == CHAOS)
    {
        if (!effects->confuse)
            effects->chaos = true;
    }
}

static void update_powerups(float dt)
{
    for (int i = 0; i < pu_counter; i++)
    {
        PowerUp *powerUp = powerups[i];
        powerUp->o->position = glms_vec2_add(powerUp->o->position, glms_vec2_scale(powerUp->o->velocity, dt));

        if (powerUp->activated)
        {
            powerUp->duration -= dt;

            if (powerUp->duration <= 0.0f)
            {
                // remove powerup from list (will later be removed)
                powerUp->activated = false;
                // deactivate effects
                if (powerUp->type == STICKY)
                {
                    // if (!isOtherPowerUpActive(this->PowerUps, "sticky"))
                    // {	// only reset if no other PowerUp of type sticky is active
                    ball->sticky = false;
                    player->color = GLMS_VEC3_ONE;
                    // }
                }
                else if (powerUp->type == PASS)
                {
                    // if (!powerups_active())
                    // {	// only reset if no other PowerUp of type pass-through is active
                    ball->pass = false;
                    ball->o->color = GLMS_VEC3_ONE;
                    // }
                }
                else if (powerUp->type == CHAOS)
                {
                    // if (!isOtherPowerUpActive(this->PowerUps, "chaos"))
                    // {	// only reset if no other PowerUp of type chaos is active
                    effects->confuse = false;
                    // }
                }
                else if (powerUp->type == CONFUSE)
                {
                    // if (!isOtherPowerUpActive(this->PowerUps, "chaos"))
                    // {	// only reset if no other PowerUp of type chaos is active
                    effects->chaos = false;
                    // }
                }
            }
        }
        powerups[i] = powerUp;
    }
}

// static bool powerups_active() {
//     bool active = false;

//     for (int i = 0; i < pu_counter; i++) {
//         if (powerups[i]->activated)
//     }
// }
