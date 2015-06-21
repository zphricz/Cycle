#ifndef GAME_H
#define GAME_H

#include <vector>
#include <memory>
#include "Screen.h"
#include "AI.h"
#include "Util.h"

const int frames_per_update = 2;

struct Player {
  Coord head;
  Direction direction;
  Direction last_move;
  bool ai_plays;
  int score;
  bool game_over;
  std::unique_ptr<AI> ai_player;

  PlayerState get_player_state() const { return {head, last_move, game_over}; }
};

/*
 * The grid is a single dimensional vector of ints that represent whether a
 * cell is occupied or not, and which team owns the cell if it is occupied. A
 * value of -1 means that the grid is not occupied. A value of N (where N != -1)
 * means that it is occupied by team N.
 */
class Game {
private:
  const int num_cells_x;
  const int num_cells_y;
  SoftScreen *const scr;
  std::vector<int> grid;
  std::vector<Player> players;
  bool game_running;
  bool game_paused;
  int num_players;

  bool all_over();
  int &grid_at(Coord c);
  void handle_input();
  void draw_game();
  void draw_cell(Coord cell, SDL_Color color);
  bool collides_with_trail(Coord c);
  bool out_of_bounds(Coord c);
  void init_game();
  void step_game();

public:
  Game(int num_x, int num_y, SoftScreen *screen);
  ~Game();
  void play();
};

#endif
