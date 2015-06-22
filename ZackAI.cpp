#include "ZackAI.h"

ZackAI::ZackAI(int num_cells_x, int num_cells_y, int your_index)
    : AI(num_cells_x, num_cells_y, your_index) {
  lookup_grid.resize(num_cells_x * num_cells_y);
}

ZackAI::~ZackAI() {}

bool ZackAI::lookup_grid_empty(Coord c) {
  return lookup_grid[c.y * num_cells_x + c.x] == -1;
}

int ZackAI::num_empty_spaces(Coord c) {
  int size = 0;
  copy(grid->begin(), grid->end(), lookup_grid.begin());
  std::vector<Coord> v;
  v.push_back(c);
  while (!v.empty()) {
    Coord new_c = v.back();
    v.pop_back();
    size++;
    lookup_grid[new_c.x + new_c.y * num_cells_x] = true;
    Coord c1{new_c.x - 1, new_c.y};
    Coord c2{new_c.x + 1, new_c.y};
    Coord c3{new_c.x, new_c.y - 1};
    Coord c4{new_c.x, new_c.y + 1};
    if (!out_of_bounds(c1) && lookup_grid_empty(c1)) {
      v.push_back(c1);
    }
    if (!out_of_bounds(c2) && lookup_grid_empty(c2)) {
      v.push_back(c2);
    }
    if (!out_of_bounds(c3) && lookup_grid_empty(c3)) {
      v.push_back(c3);
    }
    if (!out_of_bounds(c4) && lookup_grid_empty(c4)) {
      v.push_back(c4);
    }
  }
  return size;
}

Direction
ZackAI::move(std::chrono::high_resolution_clock::time_point end_time) {
  const PlayerState &p = your_player_state();
  const Coord &head = p.head;
  Coord up{head.x, head.y - 1};
  Coord down{head.x, head.y + 1};
  Coord left{head.x - 1, head.y};
  Coord right{head.x + 1, head.y};
  int num_spaces_up = -1;
  int num_spaces_down = -1;
  int num_spaces_left = -1;
  int num_spaces_right = -1;
  Direction chosen_direction = Direction::UP;
  if (!out_of_bounds(up) && grid_empty(up)) {
    num_spaces_up = num_empty_spaces(up);
  }
  if (std::chrono::high_resolution_clock::now() +
          std::chrono::milliseconds(1) >=
      end_time) {
    return p.last_move;
  }
  if (!out_of_bounds(down) && grid_empty(down)) {
    num_spaces_down = num_empty_spaces(down);
  }
  if (std::chrono::high_resolution_clock::now() +
          std::chrono::milliseconds(1) >=
      end_time) {
    return p.last_move;
  }
  if (!out_of_bounds(left) && grid_empty(left)) {
    num_spaces_left = num_empty_spaces(left);
  }
  if (std::chrono::high_resolution_clock::now() +
          std::chrono::milliseconds(1) >=
      end_time) {
    return p.last_move;
  }
  if (!out_of_bounds(right) && grid_empty(right)) {
    num_spaces_right = num_empty_spaces(right);
  }
  if (std::chrono::high_resolution_clock::now() +
          std::chrono::milliseconds(1) >=
      end_time) {
    return p.last_move;
  }
  int max_spaces = -1;
  for (int i = 0; i < 4; ++i) {
    switch (i) {
    case 0:
      if (num_spaces_up > max_spaces) {
        max_spaces = num_spaces_up;
        chosen_direction = Direction::UP;
      }
      break;
    case 1:
      if (num_spaces_down > max_spaces) {
        max_spaces = num_spaces_down;
        chosen_direction = Direction::DOWN;
      }
      break;
    case 2:
      if (num_spaces_left > max_spaces) {
        max_spaces = num_spaces_left;
        chosen_direction = Direction::LEFT;
      }
      break;
    case 3:
      if (num_spaces_right > max_spaces) {
        max_spaces = num_spaces_right;
        chosen_direction = Direction::RIGHT;
      }
      break;
    default:
      break;
    }
  }
  return chosen_direction;
}
