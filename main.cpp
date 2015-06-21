#include <SDL2/SDL.h>
#include <iostream>
#include "Screen.h"
#include "Game.h"

static void error(char *name) {
  printf("Usage: %s [num_players] [Cells_x Cells_y] [Screen_x Screen_y]\n",
         name);
  exit(1);
}

int main(int argc, char *argv[]) {
  SDL_DisplayMode display;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    exit(1);
  }
  SDL_GetCurrentDisplayMode(0, &display);
  atexit(SDL_Quit);

  int num_players = 2;
  int screen_width = display.w;
  int screen_height = display.h;
  int num_cells_x = screen_width / 12;
  int num_cells_y = screen_height / 12;

  switch (argc) {
  case 6:
    screen_width = atoi(argv[4]);
    screen_height = atoi(argv[5]);
    if (screen_width <= 0 || screen_height <= 0) {
      error(argv[0]);
    }
  case 4:
    num_cells_x = atoi(argv[2]);
    num_cells_y = atoi(argv[3]);
    if (num_cells_x <= 0 || num_cells_y <= 0) {
      error(argv[0]);
    }
  case 2:
    num_players = atoi(argv[1]);
    if (num_players < 2 || num_players > 32) {
      printf("num_players must be within [2-32]\n");
      error(argv[0]);
    }
  case 1:
    break;
  default:
    error(argv[0]);
  }

  bool full_screen;
  if (screen_width == display.w && screen_height == display.h) {
    full_screen = true;
  } else {
    full_screen = false;
  }

  SoftScreen scr(screen_width, screen_height, "Cycle", full_screen, true);
  Game g(num_players, num_cells_x, num_cells_y, &scr);
  g.play();
  return 0;
}
