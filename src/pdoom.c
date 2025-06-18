#include "pdoom.h"
#include "utils.h"
#include <assert.h>
#include <bits/stdint-uintn.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

game_t state = {
    .window = NULL,
    .renderer = NULL,
    .texture = NULL,
    .exit = false,
};

// clang-format off
int map[8][8] = {
    {1, 1, 2, 2, 3, 3, 4, 4},
    {1, 0, 0, 0, 0, 0, 0, 4},
    {1, 0, 1, 0, 2, 0, 0, 4},
    {1, 0, 0, 0, 0, 0, 0, 4},
    {1, 0, 3, 0, 4, 0, 0, 4},
    {1, 0, 0, 0, 0, 0, 0, 4},
    {1, 0, 0, 0, 0, 0, 0, 4},
    {1, 1, 2, 2, 3, 3, 4, 4},
};
// clang-format on
uint32_t wall_colors[] = {
    0x000000FF, // 0: unused
    0xFF0000FF, // 1: red
    0x00FF00FF, // 2: green
    0x0000FFFF, // 3: blue
    0xFFFF00FF, // 4: yellow
};

static int init_game(void) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    return -1;
  }
  state.window = SDL_CreateWindow("pdoom", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if (!state.window) {
    fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  state.renderer = SDL_CreateRenderer(state.window, NULL);
  if (!state.renderer) {
    fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  state.texture =
      SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
  if (!state.texture) {
    fprintf(stderr, "Could not create texture: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }
  return 0;
}

void cleanup_game(void) {
  SDL_DestroyTexture(state.texture);
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
}

static void vertical_line(int x, int y1, int y2, uint32_t color) {
  for (int y = y1; y <= y2; y++) {
    state.pixels[y * WINDOW_WIDTH + x] = color;
  }
}
static void render(void) {
  for (int x = 0; x < WINDOW_WIDTH; x++) {
    double camx = 2 * x / ((double)WINDOW_WIDTH) - 1;
    vec2f_t raydir = {state.dir.x + state.plane.x * camx,
                      state.dir.y + state.plane.y * camx};

    vec2i_t mappos = {(int)state.pos.x, (int)state.pos.y};

    vec2f_t deltadist = {(raydir.x == 0) ? 1e30 : fabs(1 / raydir.x),
                         (raydir.y == 0) ? 1e30 : fabs(1 / raydir.y)};

    double perpd = 0;

    vec2f_t sidedist = {0, 0};

    vec2i_t step = {0, 0};

    int hit = 0;
    int side = 0;

    if (raydir.x < 0) {
      step.x = -1;
      sidedist.x = (state.pos.x - mappos.x) * deltadist.x;
    } else {
      step.x = 1;
      sidedist.x = (mappos.x + 1 - state.pos.x) * deltadist.x;
    }

    if (raydir.y < 0) {
      step.y = -1;
      sidedist.y = (state.pos.y - mappos.y) * deltadist.y;
    } else {
      step.y = 1;
      sidedist.y = (mappos.y + 1 - state.pos.y) * deltadist.y;
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
      hit = map[mappos.x][mappos.y];
    }
    if (side == 0)
      perpd = (sidedist.x - deltadist.x);
    else
      perpd = (sidedist.y - deltadist.y);

    uint32_t color = wall_colors[hit];

    if (side == 1) {
      color = ((color & 0xFEFEFE) >> 1);
    }

    int lineheight = (int)(WINDOW_HEIGHT / perpd);

    int start = -lineheight / 2 + WINDOW_HEIGHT / 2;
    if (start < 0)
      start = 0;
    int end = lineheight / 2 + WINDOW_HEIGHT / 2;
    if (end >= WINDOW_HEIGHT)
      end = WINDOW_HEIGHT - 1;

    vertical_line(x, start, end, color);
  }
}
static void rotate(float angle) {
  const vec2f_t olddir = state.dir;
  const vec2f_t oldplane = state.plane;
  state.dir.x = olddir.x * cosf(angle) - olddir.y * sinf(angle);
  state.dir.y = olddir.x * sinf(angle) + olddir.y * cosf(angle);
  state.plane.x = oldplane.x * cosf(angle) - oldplane.y * sinf(angle);
  state.plane.y = oldplane.x * sinf(angle) + oldplane.y * cosf(angle);
}

static void gameloop(void) {
  float rotspeed = 3.0f * 0.01f;  // Rotation speed
  float movespeed = 3.0f * 0.01f; // Movement speed

  float new_x, new_y;

  while (!state.exit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        state.exit = true;
        break;
      case SDL_EVENT_KEY_DOWN:
        printf("Key pressed: %s\n", SDL_GetScancodeName(event.key.scancode));
        switch (event.key.scancode) {
        case SDL_SCANCODE_ESCAPE:
          state.exit = true;
          break;
        case SDL_SCANCODE_W:
          new_x = state.pos.x + state.dir.x * movespeed;
          new_y = state.pos.y + state.dir.y * movespeed;
          if (map[(int)new_x][(int)state.pos.y] == 0)
            state.pos.x = new_x;
          if (map[(int)state.pos.x][(int)new_y] == 0)
            state.pos.y = new_y;
          break;
        case SDL_SCANCODE_S:
          new_x = state.pos.x - state.dir.x * movespeed;
          new_y = state.pos.y - state.dir.y * movespeed;
          if (map[(int)new_x][(int)state.pos.y] == 0)
            state.pos.x = new_x;
          if (map[(int)state.pos.x][(int)new_y] == 0)
            state.pos.y = new_y;
          break;
        case SDL_SCANCODE_A: {
          rotate(rotspeed);
          break;
        }
        case SDL_SCANCODE_D: {
          rotate(-rotspeed);
          break;
        }
        default:
          break;
        }
        break;
      }
    }
    render();
    SDL_UpdateTexture(state.texture, NULL, state.pixels,
                      WINDOW_WIDTH * sizeof(uint32_t));
    SDL_RenderTexture(state.renderer, state.texture, NULL, NULL);
    SDL_RenderPresent(state.renderer);
  }
}

int main(void) {
  if (init_game() != 0) {
    fprintf(stderr, "Failed to initialize state.\n");
    return EXIT_FAILURE;
  }

  state.pos =
      (vec2f_t){.x = 3.5f, .y = 3.5f};      // Start near the center of the map
  state.dir = normalize(((vec2f_t){1, 0})); // Looking right (positive X)
  state.plane =
      (vec2f_t){.x = 0.0f, .y = 0.66f}; // Standard camera plane for FOV

  gameloop();
  cleanup_game();
  return EXIT_SUCCESS;
}
