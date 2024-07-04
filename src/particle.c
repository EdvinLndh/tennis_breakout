#include "particle.h"

Particle *particles;

#define NUM_PARTICLES 500

#define NUM_NEW_PARTICLES 2

unsigned int particleVAO;

static unsigned int first_unused_particle();
static void respawn_particle(int idx, BallObject *ball, vec2s offset);

void init_particles()
{
    particles = calloc(sizeof(Particle), NUM_PARTICLES);
    for (int i = 0; i < NUM_PARTICLES; ++i)
    {
        Particle p = (Particle){.pos = glms_vec2_zero(), .life = 0.0f, .velocity = glms_vec2_zero(), .color = glms_vec4_one()};
        particles[i] = p;
    }
    // set up mesh and attribute properties
    unsigned int VBO;
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f};
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(particleVAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glBindVertexArray(0);

    // Check for erros in future, mb =))
    add_texture("particle", load_texture("resources/speed_particle.png", true));
}

void update_particles(float dt, BallObject *ball, vec2s offset)
{
    if (!ball->stuck)
    {
        for (int i = 0; i < NUM_NEW_PARTICLES; ++i)
        {
            int unusedParticle = first_unused_particle();
            respawn_particle(unusedParticle, ball, offset);
        }
    }

    for (int i = 0; i < NUM_PARTICLES; ++i)
    {
        Particle p = particles[i];

        p.life -= dt;
        if (p.life > 0.0f)
        {
            p.pos = glms_vec2_sub(p.pos, glms_vec2_scale(p.velocity, dt));
            p.color.a -= dt * 2.5f;
        }
        particles[i] = p;
    }
}

unsigned int last_used_particle = 0;
static unsigned int first_unused_particle()
{
    for (unsigned int i = last_used_particle; i < NUM_PARTICLES; ++i)
    {
        if (particles[i].life <= 0.0f)
        {
            last_used_particle = i;
            return i;
        }
    }
    for (unsigned int i = 0; i < last_used_particle; ++i)
    {
        if (particles[i].life <= 0.0f)
        {
            last_used_particle = i;
            return i;
        }
    }
    last_used_particle = 0;
    return 0;
}

void draw_particle(Shader shader)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    use_shader(shader);
    for (int i = 0; i < NUM_PARTICLES; ++i)
    {
        Particle particle = particles[i];
        if (particle.life > 0.0f)
        {
            set_vec2(shader, "offset", particle.pos);
            set_vec4(shader, "color", particle.color);
            bind(get_texture("particle"));
            glBindVertexArray(particleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void respawn_particle(int idx, BallObject *ball, vec2s offset)
{
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    Particle particle = particles[idx];
    particle.pos = glms_vec2_sub(glms_vec2_add(ball->o->position, glms_vec2_adds(offset, random)), glms_vec2_scale(ball->o->velocity, 0.03f));
    particle.color = (vec4s){{rColor, rColor, rColor, 1.0f}};
    particle.life = 1.0f;
    particle.velocity = glms_vec2_scale(ball->o->velocity, 0.3f);
    particles[idx] = particle;
}