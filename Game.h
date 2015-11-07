#ifndef GAME_H
#define GAME_H

#include <vector>
#include <memory>
#include "Screen.h"
#include "AI.h"
#include "Util.h"

constexpr int frames_per_update = 2;
// How much time do all the AIs have in total to run for each frame
constexpr auto time_for_ai = std::chrono::nanoseconds(32'000'000);
// How long to wait for an AI that didn't terminate within its allotted time.
// If it doesn't complete in this amount of time, the program will abort.
constexpr auto unresponsive_ai_wait_time = std::chrono::seconds(2);

struct Player {
    Coord head;
    Direction direction;
    Direction last_move;
    bool ai_plays;
    int score;
    bool game_over;
    std::unique_ptr<AI> ai_player;

    PlayerState get_player_state() const {
        return { head, last_move, game_over };
    }
};

/*
 * The grid is a single dimensional vector of ints that represent whether a
 * cell is occupied or not, and which team owns the cell if it is occupied. A
 * value of -1 means that the grid is not occupied. A value of N (where N != -1)
 * means that it is occupied by team N.
 */
class Game {
  private:
    const int num_players;
    const int num_cells_x;
    const int num_cells_y;
    PerfSoftScreen *const scr;
    std::vector<int> grid;
    std::vector<Player> players;
    bool game_running;
    bool game_paused;
    bool step;

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
    Game(int num_players, int num_x, int num_y, PerfSoftScreen *screen);
    ~Game();
    void play();
};

#endif
