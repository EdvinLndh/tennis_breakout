#ifndef PARTICLE_H
#define PARTICLE_H

#include <cglm/cglm.h>
#include <cglm/types-struct.h>

#include "object.h"
#include "resourcemanager.h"
#include "texture.h"
#include "shader.h"


typedef struct Particle {
    vec2s pos; 
    vec2s velocity; 
    vec4s color;
    float life; 

} Particle;

void init_particles();

void update_particles(float dt, BallObject *ball, vec2s offset);

void draw_particle(Shader shader);

#endif // !PARTICLE_H
