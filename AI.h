#ifndef AI_H
#define AI_H

#include <vector>
#include "Util.h"

struct PlayerState {
  const Coord head;
  const Direction last_move;
  const bool game_over;
};

/*
 * To create your own AI, simply create a subclass of it that publicly extends
 * AI. This subclass must implement a constructor that initializes the AI base
 * class, and it must override the move() member function. Your AI will be
 * re-initialized on every new game.
 */

class AI {
protected:
  const int num_cells_x;
  const int num_cells_y;
  const std::vector<int> *grid;
  const std::vector<PlayerState> *player_states;
  const int your_index;

  bool grid_empty(Coord c) { return (*grid)[c.y * num_cells_x + c.x] == -1; }
  bool out_of_bounds(Coord c) {
    return (c.x >= num_cells_x) || (c.x < 0) || (c.y >= num_cells_y) ||
           (c.y < 0);
  }
  const PlayerState& your_player_state() {
    return (*player_states)[your_index];
  }

public:
  AI(int num_cells_x, int num_cells_y, int your_index)
      : num_cells_x(num_cells_x), num_cells_y(num_cells_y),
        your_index(your_index) {}
  virtual ~AI() {}
  void init_move(const std::vector<int> &original_grid,
                 const std::vector<PlayerState> &original_player_states) {
    grid = &original_grid;
    player_states = &original_player_states;
  }
  virtual Direction move() = 0;
};

#endif
