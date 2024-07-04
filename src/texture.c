#include "texture.h"

static Texture2D* create_texture(bool alpha);

Texture2D* load_texture(const char* path, bool alpha) {
    Texture2D *texture = create_texture(alpha);
    if(path == NULL) {
        return texture;
    }
    int width, height, nr_ch;
    unsigned char* data = stbi_load(path, &width, &height, &nr_ch, 0);
    if (data == NULL)
        fprintf(stderr, "Failed to load texture %s ", path);
    generate(texture, width, height, data);
    stbi_image_free(data);
    return texture;
}

static Texture2D* create_texture(bool alpha) {
    Texture2D *texture = malloc(sizeof(Texture2D));
    texture->Width = 0;
    texture->Height = 0;
    texture->Internal_Format = alpha ? GL_RGBA : GL_RGB;
    texture->Image_Format = alpha ? GL_RGBA : GL_RGB;
    texture->Wrap_S = GL_REPEAT;
    texture->Wrap_T = GL_REPEAT;
    texture->Filter_Min = GL_LINEAR;
    texture->Filter_Max = GL_LINEAR;

    glGenTextures(1, &(texture->ID));

    return texture;
}

void generate(Texture2D *texture, unsigned int width, unsigned int height, unsigned char* data) {
    texture->Width = width; 
    texture->Height = height;

    glBindTexture(GL_TEXTURE_2D, texture->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, texture->Internal_Format, width, height, 0, texture->Image_Format, GL_UNSIGNED_BYTE, data);
    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->Filter_Max);
        // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}


void bind(Texture2D *texture) {
    glBindTexture(GL_TEXTURE_2D, texture->ID);
}
