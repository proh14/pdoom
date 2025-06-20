#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#define MAX_TEXTURES 20

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

#include <stdint.h>

typedef uint32_t texture_t;

void texture_load_all(void);
void texture_free(void);

texture_t *texture_get(int index);

#endif // TEXTURE_H
