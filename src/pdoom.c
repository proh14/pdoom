#include "pdoom.h"
#include "game.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  game_t game;
  if (game_init(&game, "Pdoom", WINDOW_WIDTH, WINDOW_HEIGHT, RENDER_WIDTH,
                RENDER_HEIGHT) == -1) {
    fprintf(stderr, "Failed to initialize game.\n");
  }

  while (!game.exit) {
    game_update(&game);
    game_render(&game);
  }

  game_cleanup(&game);
  return EXIT_SUCCESS;
}
