#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <cglm/cglm.h>
#include <stb_image.h>
#include <string.h>

typedef struct Texture2D {
    unsigned int ID; 
    unsigned int Width, Height; 
    unsigned int Internal_Format; 
    unsigned int Image_Format;
    unsigned int Wrap_S;
    unsigned int Wrap_T;
    unsigned int Filter_Min;
    unsigned int Filter_Max;

} Texture2D; 

// Texture2D texture_create_from_path(char *path);
  
Texture2D* load_texture(const char* path, bool alpha);

void generate(Texture2D *texture, unsigned int width, unsigned int height, unsigned char* data);
void bind(Texture2D *texture);



#endif // !TEXTURE_H