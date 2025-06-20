#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "vec.h"

typedef struct player {
  vec2f_t pos;
  vec2f_t dir;
  vec2f_t plane;

  float move_speed;
  float rot_speed;
} player_t;

void player_init(player_t *player, vec2f_t pos, vec2f_t dir, vec2f_t plane,
                 float move_speed, float rot_speed);
void player_move_forward(player_t *player, float delta_time);
void player_move_backward(player_t *player, float delta_time);
void player_rotate_left(player_t *player, float delta_time);
void player_rotate_right(player_t *player, float delta_time);

#endif // PLAYER_H
