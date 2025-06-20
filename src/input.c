#include "input.h"
#include <SDL3/SDL.h>
#include <stdbool.h>

static const bool *keyboard_state;

void input_handle(game_t *game) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
      game->exit = 1;
      break;
    case SDL_EVENT_KEY_DOWN:
      if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
        game->exit = 1;
      }
      break;
    default:
      break;
    }
  }
  keyboard_state = SDL_GetKeyboardState(NULL);
}

void input_update_player(player_t *player, float delta_time) {
  if (keyboard_state[SDL_SCANCODE_W]) {
    player_move_forward(player, delta_time);
  }
  if (keyboard_state[SDL_SCANCODE_S]) {
    player_move_backward(player, delta_time);
  }
  if (keyboard_state[SDL_SCANCODE_A]) {
    player_rotate_left(player, delta_time);
  }
  if (keyboard_state[SDL_SCANCODE_D]) {
    player_rotate_right(player, delta_time);
  }
}
