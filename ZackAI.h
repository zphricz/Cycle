#ifndef ZACKAI_H
#define ZACKAI_H

#include <vector>
#include "AI.h"

class ZackAI : public AI {
private:
  std::vector<int> lookup_grid;

  bool lookup_grid_empty(Coord c);
  int num_empty_spaces(Coord c);

public:
  ZackAI(int num_x, int num_y, int your_index);
  ~ZackAI() override;
  Direction move() override;
};

#endif
