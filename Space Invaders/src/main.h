#ifndef MAIN_H
#define MAIN_H

#if defined(__MINGW32__)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
static const int WINDOW_WIDTH = 448;
static const int WINDOW_HEIGHT = 512;

static const SDL_Rect PLAYER_CLIP = {.x = 64, .y = 31, .w = 55, .h = 36 };
static const SDL_Rect INVADER1_CLIP = {.x = 122, .y = 30, .w = 51, .h = 38 };
static const SDL_Rect INVADER2_CLIP = {.x = 1, .y = 68, .w = 47, .h = 27 };
static const SDL_Rect INVADER3_CLIP = {.x = 55, .y = 69, .w = 37, .h = 25 };
static const SDL_Rect LASER_CLIP = {.x = 251, .y = 33, .w = 17, .h = 16 };
static const SDL_Rect DYING_INVADER_CLIP = {.x = 2, .y = 0, .w = 58, .h = 30 };

struct list* objects;

struct object* player;
struct object* player_laser;

struct object* invaders[55];
struct object* invader_lasers[11];


struct object* mystery_ship;

#endif
