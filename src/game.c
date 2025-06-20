#include "game.h"
#include "input.h"
#include "render.h"
#include "texture.h"
#include "utils.h"
#include "vec.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int game_init(game_t *game, const char *title, int width, int height,
              int rend_width, int rend_height) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    return -1;
  }
  game->window = SDL_CreateWindow(title, width, height, 0);
  if (!game->window) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    return -1;
  }

  game->renderer = SDL_CreateRenderer(game->window, NULL);
  if (!game->renderer) {
    fprintf(stderr, "could not create renderer: %s\n", SDL_GetError());
    return -1;
  }

  game->texture =
      SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, rend_width, rend_height);
  if (!game->texture) {
    fprintf(stderr, "could not create texture: %s\n", SDL_GetError());
    return -1;
  }

  game->pixels = xmalloc(rend_width * rend_height * sizeof(uint32_t));

  game->rend_width = rend_width;
  game->rend_height = rend_height;

  game->window_width = width;
  game->window_height = height;

  game->player.pos = (vec2f_t){3.5f, 3.5f};
  game->player.dir = (vec2f_t){1.0f, 0.0f};
  game->player.plane = (vec2f_t){0.0f, 0.66f}; // Perpendicular to the direction
  game->player.move_speed = 3.0f;
  game->player.rot_speed = 2.0f;

  game->exit = false;

  texture_load_all();
  return 0;
}

static inline void update_texture(game_t *game) {
  void *pixels;
  int pitch;
  SDL_LockTexture(game->texture, NULL, &pixels, &pitch);
  memcpy(pixels, game->pixels,
         game->rend_width * game->rend_height * sizeof(uint32_t));
  SDL_UnlockTexture(game->texture);

  SDL_RenderClear(game->renderer);
  SDL_RenderTexture(game->renderer, game->texture, NULL, NULL);
  SDL_RenderPresent(game->renderer);
}

void game_render(game_t *game) {
  memset(game->pixels, 0,
         game->rend_width * game->rend_height * sizeof(uint32_t));
  render_walls(game);
  render_sprites(game);

  update_texture(game);
}

void game_update(game_t *game) {
  uint64_t current_tick = SDL_GetTicks();
  game->delta_time = (current_tick - game->last_tick) / 1000.0f;
  game->last_tick = current_tick;

  if (game->delta_time > 0.1f) {
    game->delta_time = 0.1f; // Cap delta time
  }

  input_handle(game);
  input_update_player(&game->player, game->delta_time);
}

void game_cleanup(game_t *game) {
  texture_free();
  free(game->pixels);
  SDL_DestroyTexture(game->texture);
  SDL_DestroyRenderer(game->renderer);
  SDL_DestroyWindow(game->window);
  SDL_Quit();
}
