#include "pdoom.h"
#include "utils.h"
#include <assert.h>
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
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
};
// clang-format on

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
    // Calculate ray position and direction
    const float cameraX = (2.0f * (x / (float)(WINDOW_WIDTH))) - 1.0f;
    const vec2f_t dir = {
        .x = state.dir.x + state.plane.x * cameraX,
        .y = state.dir.y + state.plane.y * cameraX,
    };

    // Raycasting algorithm
    vec2f_t pos = state.pos;
    vec2i_t ipos = {
        .x = (int)pos.x,
        .y = (int)pos.y,
    };

    const vec2f_t deltaDist = {
        .x = fabsf(dir.x) < 1e-20 ? 1e30 : fabsf(1.0f / dir.x),
        .y = fabsf(dir.y) < 1e-20 ? 1e30 : fabsf(1.0f / dir.y),
    };

    vec2f_t sideDist = {
        .x = (dir.x < 0) ? (pos.x - ipos.x) * deltaDist.x
                         : (ipos.x + 1.0f - pos.x) * deltaDist.x,
        .y = (dir.y < 0) ? (pos.y - ipos.y) * deltaDist.y
                         : (ipos.y + 1.0f - pos.y) * deltaDist.y,
    };

    vec2i_t step = {
        .x = (int)sign(dir.x),
        .y = (int)sign(dir.y),
    };

    int hit = 0;
    int side = 0;
    while (!hit) {
      if (sideDist.x < sideDist.y) {
        sideDist.x += deltaDist.x;
        ipos.x += step.x;
        side = 0; // x-axis
      } else {
        sideDist.y += deltaDist.y;
        ipos.y += step.y;
        side = 1; // y-axis
      }

      hit = map[ipos.x][ipos.y]; // Hit a wall
    }
    uint32_t color = 0xFF0000FF; // Default color (red)

    if (side == 1) {
      const uint32_t br = ((color & 0xFF0FF) * 0xC0) >> 8;
      const uint32_t g = ((color & 0x00FF00) * 0xC0) >> 8;

      color = 0xFF000000 | (br & 0xFF00FF) | (g & 0x00FF00);
    }

    double perpDist = 0;
    if (side == 0) {
      perpDist = (sideDist.x - deltaDist.x);
    } else {
      perpDist = (sideDist.y - deltaDist.y);
    }

    const int height = (int)(WINDOW_HEIGHT / perpDist);
    const int drawStart = max(0, (WINDOW_HEIGHT / 2) - (height / 2));
    const int drawEnd =
        min(WINDOW_HEIGHT - 1, (WINDOW_HEIGHT / 2) + (height / 2));

    assert(ipos.x >= 0 && ipos.x < 8 && ipos.y >= 0 && ipos.y < 8);

    // vertical_line(x, 0, drawStart, 0xFF202020);               // Sky color
    vertical_line(x, drawStart, drawEnd, color); // Wall color
    // vertical_line(x, drawEnd, WINDOW_HEIGHT - 1, 0xFF000000); // Floor color
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
          state.pos.x += state.dir.x * movespeed;
          state.pos.y += state.dir.y * movespeed;
          break;
        case SDL_SCANCODE_S:
          state.pos.x -= state.dir.x * movespeed;
          state.pos.y -= state.dir.y * movespeed;
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
    memset(state.pixels, 0, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));
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

  state.pos = (vec2f_t){.x = 2, .y = 2};
  state.dir = normalize(((vec2f_t){-1.0f, 0.1f}));
  state.plane = (vec2f_t){.x = 0.0f, .y = 0.66f}; // Camera plane

  gameloop();
  cleanup_game();
  return EXIT_SUCCESS;
}
