#include "AI.h"

AI::AI(int num_cells_x, int num_cells_y, int your_index)
    : num_cells_x(num_cells_x), num_cells_y(num_cells_y),
      your_index(your_index) {}

void AI::init_move(const std::vector<int> &original_grid,
                   const std::vector<PlayerState> &original_player_states) {
  grid = &original_grid;
  player_states = &original_player_states;
}

bool AI::grid_empty(Coord c) { return (*grid)[c.y * num_cells_x + c.x] == -1; }

bool AI::out_of_bounds(Coord c) {
  return (c.x >= num_cells_x) || (c.x < 0) || (c.y >= num_cells_y) || (c.y < 0);
}

const PlayerState &AI::your_player_state() {
  return (*player_states)[your_index];
}
