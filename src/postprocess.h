/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <cglm/cglm.h>
#include <stdlib.h>
#include <glad/glad.h>

#include "texture.h"
#include "spriterenderer.h"
#include "shader.h"


typedef struct PostProcessor {
    Shader shader; 
    Texture2D *texture;
    unsigned int width, height; 
    unsigned int MSFBO, FBO, RBO, VAO;
    bool confuse, chaos, shake;
} PostProcessor;

// PostProcessor hosts all PostProcessing effects for the Breakout
// Game. It renders the game on a textured quad after which one can
// enable specific effects by enabling either the Confuse, Chaos or 
// Shake boolean. 
// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.
PostProcessor *create_post_processor(Shader shader, unsigned int width, unsigned int height);
// prepares the postprocessor's framebuffer operations before rendering the game
void begin_render(PostProcessor *p);
// should be called after rendering the game, so it stores all the rendered data into a texture object
void end_render(PostProcessor *p);
// renders the PostProcessor texture quad (as a screen-encompassing large sprite)
void render_effects(PostProcessor *p, float time);

#endif