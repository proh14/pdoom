#include "render.h"
#include "map.h"
#include "vec.h"
#include <math.h>

static uint32_t wall_colors[] = {
    0x000000FF, // 0: unused
    0xFF0000FF, // 1: red
    0x00FF00FF, // 2: green
    0x0000FFFF, // 3: blue
    0xFFFF00FF, // 4: yellow
};

static void perform_dda(game_t *game, int *start, int *end, uint32_t *color,
                        int x) {
  player_t *player = &game->player;
  double camx = 2 * x / ((double)game->rend_width) - 1;
  vec2f_t raydir = {player->dir.x + player->plane.x * camx,
                    player->dir.y + player->plane.y * camx};

  vec2i_t mappos = {(int)player->pos.x, (int)player->pos.y};

  vec2f_t deltadist = {(raydir.x == 0) ? 1e30 : fabs(1 / raydir.x),
                       (raydir.y == 0) ? 1e30 : fabs(1 / raydir.y)};

  double perpd = 0;

  vec2f_t sidedist = {0, 0};

  vec2i_t step = {0, 0};

  int hit = 0;
  int side = 0;

  if (raydir.x < 0) {
    step.x = -1;
    sidedist.x = (player->pos.x - mappos.x) * deltadist.x;
  } else {
    step.x = 1;
    sidedist.x = (mappos.x + 1 - player->pos.x) * deltadist.x;
  }

  if (raydir.y < 0) {
    step.y = -1;
    sidedist.y = (player->pos.y - mappos.y) * deltadist.y;
  } else {
    step.y = 1;
    sidedist.y = (mappos.y + 1 - player->pos.y) * deltadist.y;
  }

  while (hit == 0) {
    if (sidedist.x < sidedist.y) {
      sidedist.x += deltadist.x;
      mappos.x += step.x;
      side = 0;
    } else {
      sidedist.y += deltadist.y;
      mappos.y += step.y;
      side = 1;
    }

    if (mappos.x < 0 || mappos.x >= 8 || mappos.y < 0 || mappos.y >= 8) {
      break;
    }

    hit = map[mappos.x][mappos.y];
  }

  if (side == 0)
    perpd = (mappos.x - player->pos.x + (1 - step.x) / 2.0) / raydir.x;
  else
    perpd = (mappos.y - player->pos.y + (1 - step.y) / 2.0) / raydir.y;
  *color = wall_colors[hit];

  if (side == 1) {
    *color = ((*color & 0xFEFEFE) >> 1);
  }

  int lineheight = (int)(game->rend_height / perpd);

  *start = -lineheight / 2 + game->rend_height / 2;
  if (*start < 0)
    *start = 0;
  *end = lineheight / 2 + game->rend_height / 2;
  if (*end >= game->rend_height)
    *end = game->rend_height - 1;
}

static inline void vertical_line(game_t *game, int start, int end,
                                 uint32_t color, int x) {
  for (int y = start; y <= end; y++) {
    game->pixels[y * game->rend_width + x] = color;
  }
}

void render_walls(game_t *game) {
  for (int x = 0; x < game->rend_width; x++) {
    int start, end;
    uint32_t color;
    perform_dda(game, &start, &end, &color, x);
    vertical_line(game, start, end, color, x);
  }
}

void render_sprites(game_t *game) {
  // TODO: Implement sprite rendering
  (void)game; // Prevent unused variable warning
  return;
}
