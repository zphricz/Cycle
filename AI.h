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

  bool grid_empty(Coord c);
  bool out_of_bounds(Coord c);
  const PlayerState& your_player_state();

public:
  AI(int num_cells_x, int num_cells_y, int your_index);
  virtual ~AI() {}
  void init_move(const std::vector<int> &original_grid,
                 const std::vector<PlayerState> &original_player_states);
  virtual Direction move() = 0;
};

#endif
