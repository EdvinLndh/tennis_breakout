#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <ft2build.h>
#include FT_FREETYPE_H  
#include "texture.h"
#include <glad/glad.h>
#include <cglm/cglm.h>
#include <cglm/types-struct.h>
#include <cglm/struct.h>
#include "shader.h"

typedef struct TextChar {
    unsigned int textureID;
    ivec2s size; 
    ivec2s bearing;
    unsigned int advance;
} TextChar;


void text_init(unsigned int width, unsigned int height);

void load_face(char *path, unsigned int size);

void render_text(char *text, float x, float y, float scale, vec3s color);

#endif // !TEXT_RENDERER_H