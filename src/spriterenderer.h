#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "shader.h"
#include "texture.h"
#include <cglm/struct.h>

void draw_sprite(Shader shader, Texture2D *texture, vec2s position,
        vec2s size, float rotation, vec3s color);
void init_render_data();

#endif // !SPRITE_RENDERER_H
