#pragma once
#ifndef PDOOM_H_
#define PDOOM_H_

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#include "utils.h"
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct game {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  uint32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];

  vec2f_t pos, dir, plane;

  bool exit;
} game_t;

extern game_t game;

#endif // PDOOM_H_
