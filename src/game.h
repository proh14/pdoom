#pragma once
#ifndef GAME_H
#define GAME_H

#include "player.h"
#include <SDL3/SDL.h>
#include <stdbool.h>

typedef struct game {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  int rend_width, rend_height;
  int window_width, window_height;
  uint32_t *pixels;

  float delta_time;
  uint64_t last_tick;

  player_t player;
  bool exit;
} game_t;

int game_init(game_t *game, const char *title, int width, int height,
              int rend_width, int rend_height);
void game_cleanup(game_t *game);

void game_render(game_t *game);
void game_update(game_t *game);

#endif // GAME_H
