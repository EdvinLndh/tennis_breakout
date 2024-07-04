#include "spriterenderer.h"

unsigned int quadVAO;
void init_render_data()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f};

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void draw_sprite(Shader shader, Texture2D *texture, vec2s position,
                 vec2s size, float rotation, vec3s color)
{
    use_shader(shader);
    mat4s model = glms_mat4_identity();
    model = glms_translate(model, (vec3s){{position.x, position.y, 0.0f}});

    model = glms_translate(model, (vec3s){{0.5f * size.x, 0.5f * size.y, 0.0f}});
    model = glms_rotate(model, glm_rad(rotation), (vec3s){{0.0f, 0.0f, 1.0f}});
    model = glms_translate(model, (vec3s){{-0.5f * size.x, -0.5f * size.y, 0.0f}});

    model = glms_scale(model, (vec3s){{size.x, size.y, 1.0f}});

    set_mat4(shader, "model", model);
    set_vec3(shader, "spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    bind(texture);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}