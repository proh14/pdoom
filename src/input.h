#pragma once
#ifndef INPUT_H
#define INPUT_H

#include "game.h"
#include "player.h"

void input_handle(game_t *game);
void input_update_player(player_t *player, float delta_time);

#endif // INPUT_H
