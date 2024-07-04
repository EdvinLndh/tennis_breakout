#ifndef UTIL_H
#define UTIL_H

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <stdio.h>

GLenum glCheckError_(const char *file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__) 

#endif // !UTIL_H