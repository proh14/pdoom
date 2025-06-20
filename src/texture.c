#include "texture.h"
#include "utils.h"
#include <SDL3/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

texture_t *textures[MAX_TEXTURES] = {NULL};

static void texture_load_png(const char *filename, int index) {
  // Use stb to load images

  int width, height, channels;
  unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
  if (!data) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load texture: %s",
                 filename);
    return;
  }
  printf("Loaded texture %s (%d x %d)\n", filename, width, height);
  if (index < 0 || index >= MAX_TEXTURES) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Texture index out of bounds: %d", index);
    stbi_image_free(data);
    return;
  }
  textures[index] = malloc(sizeof(texture_t) * TEXTURE_HEIGHT * TEXTURE_WIDTH);
  memcpy(textures[index], data,
         TEXTURE_HEIGHT * TEXTURE_WIDTH * sizeof(uint32_t));
  stbi_image_free(data);
}

void texture_load_all(void) {
  texture_load_png("./assets/wall_1.png", 0);
  texture_load_png("./assets/wall_1.png", 1);
  texture_load_png("./assets/wall_1.png", 2);
  texture_load_png("./assets/wall_1.png", 3);
  texture_load_png("./assets/wall_1.png", 4);
}

void texture_free(void) {
  for (int i = 0; i < MAX_TEXTURES; i++) {
    free(textures[i]);
    textures[i] = NULL;
  }
}

texture_t *texture_get(int index) {
  if (index < 0 || index >= MAX_TEXTURES) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Texture index out of bounds: %d", index);
    return NULL;
  }
  return textures[index];
}
