#include "player.h"
#include <math.h>

void player_init(player_t *player, vec2f_t pos, vec2f_t dir, vec2f_t plane,
                 float move_speed, float rot_speed) {
  player->pos = pos;
  player->dir = dir;
  player->plane = plane;
  player->move_speed = move_speed;
  player->rot_speed = rot_speed;
}

void player_move_backward(player_t *player, float delta_time) {
  player->pos.x -= player->dir.x * player->move_speed * delta_time;
  player->pos.y -= player->dir.y * player->move_speed * delta_time;
}

void player_move_forward(player_t *player, float delta_time) {
  player->pos.x += player->dir.x * player->move_speed * delta_time;
  player->pos.y += player->dir.y * player->move_speed * delta_time;
}

void player_rotate_right(player_t *player, float delta_time) {
  float old_dir_x = player->dir.x;
  player->dir.x = player->dir.x * cosf(player->rot_speed * delta_time) -
                  player->dir.y * sinf(player->rot_speed * delta_time);
  player->dir.y = old_dir_x * sinf(player->rot_speed * delta_time) +
                  player->dir.y * cosf(player->rot_speed * delta_time);

  float old_plane_x = player->plane.x;
  player->plane.x = player->plane.x * cosf(player->rot_speed * delta_time) -
                    player->plane.y * sinf(player->rot_speed * delta_time);
  player->plane.y = old_plane_x * sinf(player->rot_speed * delta_time) +
                    player->plane.y * cosf(player->rot_speed * delta_time);
}

void player_rotate_left(player_t *player, float delta_time) {
  float old_dir_x = player->dir.x;
  player->dir.x = player->dir.x * cosf(-player->rot_speed * delta_time) -
                  player->dir.y * sinf(-player->rot_speed * delta_time);
  player->dir.y = old_dir_x * sinf(-player->rot_speed * delta_time) +
                  player->dir.y * cosf(-player->rot_speed * delta_time);

  float old_plane_x = player->plane.x;
  player->plane.x = player->plane.x * cosf(-player->rot_speed * delta_time) -
                    player->plane.y * sinf(-player->rot_speed * delta_time);
  player->plane.y = old_plane_x * sinf(-player->rot_speed * delta_time) +
                    player->plane.y * cosf(-player->rot_speed * delta_time);
}
