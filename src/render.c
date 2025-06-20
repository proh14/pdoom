#include "render.h"
#include "map.h"
#include "texture.h"
#include "vec.h"
#include <math.h>

static void perform_dda(game_t *game, int *start, int *end, int *val, int *side,
                        int *textx, double *textpos, double *textstep, int x) {
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
      *side = 0;
    } else {
      sidedist.y += deltadist.y;
      mappos.y += step.y;
      *side = 1;
    }

    if (mappos.x < 0 || mappos.x >= 8 || mappos.y < 0 || mappos.y >= 8) {
      break;
    }

    hit = map[mappos.x][mappos.y];
  }

  if (*side == 0)
    perpd = (mappos.x - player->pos.x + (1 - step.x) / 2.0) / raydir.x;
  else
    perpd = (mappos.y - player->pos.y + (1 - step.y) / 2.0) / raydir.y;

  int lineheight = (int)(game->rend_height / perpd);

  *start = -lineheight / 2 + game->rend_height / 2;
  if (*start < 0)
    *start = 0;
  *end = lineheight / 2 + game->rend_height / 2;
  if (*end >= game->rend_height)
    *end = game->rend_height - 1;

  *val = hit;

  double wallx;
  if (*side == 0)
    wallx = player->pos.y + perpd * raydir.y;
  else
    wallx = player->pos.x + perpd * raydir.x;
  wallx -= floor(wallx);

  *textx = (int)(wallx * TEXTURE_WIDTH);
  if (*side == 0 && raydir.x < 0)
    *textx = TEXTURE_WIDTH - *textx - 1;
  if (*side == 1 && raydir.y < 0)
    *textx = TEXTURE_WIDTH - *textx - 1;

  *textstep = 1.0 * TEXTURE_HEIGHT / lineheight;

  *textpos = (*start - (double)game->rend_height / 2 + (double)lineheight / 2) *
             (*textstep);
}

static inline void draw_texture_column(game_t *game, int x, int start, int end,
                                       int val, int side, double textpos,
                                       double textstep, int textx) {
  texture_t *texture = texture_get(val);

  for (int y = start; y < end; y++) {
    int texy = (int)textpos & (TEXTURE_HEIGHT - 1);
    textpos += textstep;

    uint32_t color = texture[texy * TEXTURE_WIDTH + textx];
    if (side == 1) {
      // Darken the color for walls facing north/south
      color = (color >> 1) & 0x7F7F7F;
    }

    game->pixels[y * game->rend_width + x] = color;
  }
}

void render_walls(game_t *game) {
  for (int x = 0; x < game->rend_width; x++) {
    int start, end, val, side, textx;
    double step, textpos;
    perform_dda(game, &start, &end, &val, &side, &textx, &textpos, &step, x);
    draw_texture_column(game, x, start, end, val, side, textpos, step, textx);
  }
}

void render_sprites(game_t *game) {
  // TODO: Implement sprite rendering
  (void)game; // Prevent unused variable warning
  return;
}
