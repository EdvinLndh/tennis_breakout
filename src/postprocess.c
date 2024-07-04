#include "postprocess.h"


static void init_post_processor(PostProcessor *p);

PostProcessor *create_post_processor(Shader shader, unsigned int width, unsigned int height) {
    PostProcessor *p = malloc(sizeof(PostProcessor));
    p->width = width;
    p->height = height;
    p->confuse = false;
    p->shake = false;
    p->chaos = false;
    p->shader = shader;

    // initialize renderbuffer/framebuffer object
    glGenFramebuffers(1, &p->MSFBO);
    glGenFramebuffers(1, &p->FBO);
    glGenRenderbuffers(1, &p->RBO);
    // initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
    glBindFramebuffer(GL_FRAMEBUFFER, p->MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, p->RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height); // allocate storage for render buffer object
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, p->RBO); // attach MS render buffer object to framebuffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "Error Framebuffer\n");
    // also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
    glBindFramebuffer(GL_FRAMEBUFFER, p->FBO);

    p->texture = load_texture(NULL, false);
    generate(p->texture, p->width, p->height, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, p->texture->ID, 0); // attach texture to framebuffer as its color attachment
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "ERROR::POSTPROCESSOR: Failed to initialize FBO\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // initialize render data and uniforms
    use_shader(p->shader);
    init_post_processor(p);
    set_int(p->shader, "scene", 0);
    float offset = 1.0f / 300.0f;
    float offsets[9][2] = {
        { -offset,  offset  },  // top-left
        {  0.0f,    offset  },  // top-center
        {  offset,  offset  },  // top-right
        { -offset,  0.0f    },  // center-left
        {  0.0f,    0.0f    },  // center-center
        {  offset,  0.0f    },  // center - right
        { -offset, -offset  },  // bottom-left
        {  0.0f,   -offset  },  // bottom-center
        {  offset, -offset  }   // bottom-right    
    };

    glUniform2fv(glGetUniformLocation(p->shader.ID, "offsets"), 9, (float*)offsets);
    int edge_kernel[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };
    glUniform1iv(glGetUniformLocation(p->shader.ID, "edge_kernel"), 9, edge_kernel);
    float blur_kernel[9] = {
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    };
    glUniform1fv(glGetUniformLocation(p->shader.ID, "blur_kernel"), 9, blur_kernel);    

    return p;
}
// prepares the postprocessor's framebuffer operations before rendering the game
void begin_render(PostProcessor *p) {
    glBindFramebuffer(GL_FRAMEBUFFER, p->MSFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
// should be called after rendering the game, so it stores all the rendered data into a texture object
void end_render(PostProcessor *p) {
        // now resolve multisampled color-buffer into intermediate FBO to store to texture
    glBindFramebuffer(GL_READ_FRAMEBUFFER, p->MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, p->FBO);
    glBlitFramebuffer(0, 0, p->width, p->height, 0, 0, p->width, p->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // binds both READ and WRITE framebuffer to default framebuffer
}
// renders the PostProcessor texture quad (as a screen-encompassing large sprite)
void render_effects(PostProcessor *p, float time) {
    // set uniforms/options
    use_shader(p->shader);
    set_float(p->shader, "time", time);
    set_int(p->shader, "chaos", p->chaos);
    set_int(p->shader, "shake", p->shake);
    set_int(p->shader, "confuse", p->confuse);
    // render textured quad
    glActiveTexture(GL_TEXTURE0);
    bind(p->texture);
    glBindVertexArray(p->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

static void init_post_processor(PostProcessor *p){
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos        // tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &p->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(p->VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}