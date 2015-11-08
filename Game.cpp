#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <iomanip>
#include <random>
#include "Threadpool.h"
#include "Game.h"
#include "ZackAI.h"

SDL_Color bg_color{0, 0, 0};
// Colors taken from:
// http://stackoverflow.com/questions/2328339/how-to-generate-n-different-colors-for-any-natural-number-n
SDL_Color trail_colors[32] = {
    {0xFF, 0xFF, 0x00}, {0x1C, 0xE6, 0xFF}, {0xFF, 0x34, 0xFF},
    {0xFF, 0x4A, 0x46}, {0x00, 0x89, 0x41}, {0x00, 0x6F, 0xA6},
    {0xA3, 0x00, 0x59}, {0xFF, 0xDB, 0xE5}, {0x7A, 0x49, 0x00},
    {0x00, 0x00, 0xA6}, {0x63, 0xFF, 0xAC}, {0xB7, 0x97, 0x62},
    {0x00, 0x4D, 0x43}, {0x8F, 0xB0, 0xFF}, {0x99, 0x7D, 0x87},
    {0x61, 0x61, 0x5A}, {0xBA, 0x09, 0x00}, {0x6B, 0x79, 0x00},
    {0x00, 0xC2, 0xA0}, {0xFF, 0xAA, 0x92}, {0xFF, 0x90, 0xC9},
    {0xB9, 0x03, 0xAA}, {0xD1, 0x61, 0x00}, {0xDD, 0xEF, 0xFF},
    {0x00, 0x00, 0x35}, {0x7B, 0x4F, 0x4B}, {0xA1, 0xC2, 0x99},
    {0x30, 0x00, 0x18}, {0x0A, 0xA6, 0xD8}, {0x01, 0x33, 0x49},
    {0x00, 0x84, 0x6F}, {0x37, 0x21, 0x01}};
SDL_Color dead_color{100, 0, 0};

Game::Game(int num_players, int num_x, int num_y, PerfSoftScreen *screen)
    : num_players(num_players), num_cells_x(num_x), num_cells_y(num_y),
      scr(screen), game_running(true) {
  int num_on_left = num_players / 4 + (num_players % 4 > 0);
  int num_on_up = num_players / 4 + (num_players % 4 > 2);
  if (num_cells_x < 2 || num_cells_x < num_on_up * 2 || num_cells_y < 2 ||
      num_cells_y < num_on_left * 2) {
    std::cout << "ERROR: Too few cells to play with" << std::endl;
    exit(1);
  }
  players.resize(num_players);
  grid.resize(num_cells_x * num_cells_y);
  scr->set_recording_style("images", 5);
}

Game::~Game() {}

bool Game::all_over() {
  int num_over = 0;
  for (Player &player : players) {
    if (player.game_over) {
      num_over++;
      if (num_over >= num_players - 1) {
        return true;
      }
    }
  }
  return false;
}

void Game::handle_input() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: {
      game_running = false;
      break;
    }
    case SDL_KEYDOWN: {
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE: {
        game_running = false;
        break;
      }
      case 'a': {
        if (players[0].last_move != Direction::RIGHT && !game_paused) {
          players[0].direction = Direction::LEFT;
        }
        break;
      }
      case 'd': {
        if (players[0].last_move != Direction::LEFT && !game_paused) {
          players[0].direction = Direction::RIGHT;
        }
        break;
      }
      case 'w': {
        if (players[0].last_move != Direction::DOWN && !game_paused) {
          players[0].direction = Direction::UP;
        }
        break;
      }
      case 's': {
        if (players[0].last_move != Direction::UP && !game_paused) {
          players[0].direction = Direction::DOWN;
        }
        break;
      }
      case SDLK_LEFT: {
        if (players[1].last_move != Direction::RIGHT && !game_paused) {
          players[1].direction = Direction::LEFT;
        }
        break;
      }
      case SDLK_RIGHT: {
        if (players[1].last_move != Direction::LEFT && !game_paused) {
          players[1].direction = Direction::RIGHT;
        }
        break;
      }
      case SDLK_UP: {
        if (players[1].last_move != Direction::DOWN && !game_paused) {
          players[1].direction = Direction::UP;
        }
        break;
      }
      case SDLK_DOWN: {
        if (players[1].last_move != Direction::UP && !game_paused) {
          players[1].direction = Direction::DOWN;
        }
        break;
      }
      case SDLK_SPACE: {
        step = true;
        break;
      }
      case SDLK_RETURN: {
        if (all_over()) {
          init_game();
          game_paused = true;
        } else {
          game_paused = !game_paused;
        }
        break;
      }
      case SDLK_1: {
        players[0].ai_plays = !players[0].ai_plays;
        break;
      }
      case SDLK_2: {
        players[1].ai_plays = !players[1].ai_plays;
        break;
      }
      case SDLK_0: {
        scr->toggle_recording();
        break;
      }
      default: { break; }
      }
      break;
    }
    default: { break; }
    }
  }
}

void Game::draw_game() {
  scr->fill_screen(bg_color);
  // Draw the background
  for (int y = 0; y < num_cells_y; ++y) {
    for (int x = 0; x < num_cells_x; ++x) {
      int team = grid_at({x, y});
      int x1 = x * scr->width / num_cells_x;
      int x2 = (x + 1) * scr->width / num_cells_x - 1;
      int y1 = y * scr->height / num_cells_y;
      int y2 = (y + 1) * scr->height / num_cells_y - 1;
      if (team == -1) {
        scr->fill_rect(x1, y1, x2, y2, bg_color);
      } else {
        if (players[team].game_over) {
          scr->fill_rect(x1, y1, x2, y2, dead_color);
        } else {
          scr->fill_rect(x1, y1, x2, y2, trail_colors[team]);
        }
      }
    }
  }
}

int &Game::grid_at(Coord c) { return grid[c.y * num_cells_x + c.x]; }

bool Game::collides_with_trail(Coord c) { return grid_at(c) != -1; }

bool Game::out_of_bounds(Coord c) {
  return (c.x >= num_cells_x) || (c.x < 0) || (c.y >= num_cells_y) || (c.y < 0);
}

void Game::step_game() {
  int index = 0;
  std::vector<Coord> next_moves;
  next_moves.resize(num_players);
  for (Player &player : players) {
    if (!player.game_over) {
      Coord next = player.head;
      switch (player.direction) {
      case Direction::UP: {
        next.y--;
        break;
      }
      case Direction::DOWN: {
        next.y++;
        break;
      }
      case Direction::LEFT: {
        next.x--;
        break;
      }
      case Direction::RIGHT: {
        next.x++;
        break;
      }
      default: { break; }
      }
      player.last_move = player.direction;
      if (out_of_bounds(next) || collides_with_trail(next)) {
        player.game_over = true;
      } else {
        next_moves[index] = next;
      }
    } else {
      next_moves[index] = player.head;
    }
    index++;
  }
  for (int i = 0; i < num_players; ++i) {
    if (!players[i].game_over) {
      for (int j = 0; j < num_players; ++j) {
        if (i == j) {
          continue;
        }
        if (next_moves[i] == next_moves[j]) {
          players[i].game_over = true;
        }
      }
      players[i].head = next_moves[i];
      grid_at(players[i].head) = i;
      if (!players[i].game_over) {
        players[i].score++;
      }
    }
  }
  if (all_over()) {
    game_paused = true;
  }
}

void Game::init_game() {
  static bool first_init = true;
  step = false;
  fill(grid.begin(), grid.end(), -1);
  game_paused = true;
  int index = 0;
  int num_on_left = num_players / 4 + (num_players % 4 > 0);
  int num_on_right = num_players / 4 + (num_players % 4 > 1);
  int num_on_up = num_players / 4 + (num_players % 4 > 2);
  int num_on_down = num_players / 4 + (num_players % 4 > 3);
  int l = 1;
  int r = 1;
  int u = 1;
  int d = 1;
  // Place each player at their starting positions
  for (Player &player : players) {
    if (index % 4 == 0) {
      player.direction = Direction::RIGHT;
      player.head = {0, l * num_cells_y / (num_on_left + 1)};
      l++;
    } else if (index % 4 == 1) {
      player.direction = Direction::LEFT;
      player.head = {num_cells_x - 1, r * num_cells_y / (num_on_right + 1)};
      r++;
    } else if (index % 4 == 2) {
      player.direction = Direction::DOWN;
      player.head = {u * num_cells_x / (num_on_up + 1), 0};
      u++;
    } else if (index % 4 == 3) {
      player.direction = Direction::UP;
      player.head = {d * num_cells_x / (num_on_down + 1), num_cells_y - 1};
      d++;
    }
    player.ai_player =
        std::make_unique<ZackAI>(num_cells_x, num_cells_y, index);
    player.score = 0;
    player.game_over = false;
    player.last_move = Direction::NONE;
    if (first_init) {
      if (index < 2) {
        player.ai_plays = false;
      } else {
        player.ai_plays = true;
      }
    }
    grid_at(player.head) = index;
    index++;
  }
  if (first_init) {
    first_init = false;
  }
  std::cout << "Game Start" << std::endl;
}

// Runs the game loop
void Game::play() {
  init_game();
  int i = 0;
  while (game_running) {
    handle_input();
    if (!game_paused || (step && !all_over())) {
      step = false;
      if (i % frames_per_update == 0) {
        std::vector<PlayerState> player_states;
        player_states.reserve(num_players);
        for (Player &player : players) {
          player_states.push_back(player.get_player_state());
        }
        int num_times = num_players / std::thread::hardware_concurrency() +
                        (num_players % std::thread::hardware_concurrency());
        auto time_for_one_round = time_for_ai / num_times;
        for (int j = 0; j < num_players;
             j += std::thread::hardware_concurrency()) {
          std::vector<std::future<Direction>> futures;
          futures.reserve(num_players);

          std::chrono::high_resolution_clock::time_point end_of_waiting =
              std::chrono::high_resolution_clock::now() + time_for_one_round;
          for (int k = 0; k < (int)std::thread::hardware_concurrency(); ++k) {
            if (j + k >= num_players) {
              break;
            }
            if (players[j + k].ai_plays && !players[j + k].game_over) {
              players[j + k].ai_player->init_move(grid, player_states);
              futures.emplace_back(Threadpool::submit_contract(
                  &AI::move, players[j + k].ai_player.get(), end_of_waiting));
            }
          }
          std::vector<std::future<Direction> *> unresponsive_futures;
          int index = j;
          for (auto &future : futures) {
            while (!(players[index].ai_plays && !players[index].game_over)) {
              index++;
            }
            auto status = future.wait_until(end_of_waiting);
            if (status == std::future_status::ready) {
              players[index].direction = future.get();
            } else {
              unresponsive_futures.push_back(&future);
              players[index].game_over = true;
            }
            index++;
          }
          for (auto &future : unresponsive_futures) {
            auto status = future->wait_for(unresponsive_ai_wait_time);
            if (status != std::future_status::ready) {
              std::cout << "ERROR: AI unresponsive!" << std::endl;
              abort();
            }
          }
        }
        step_game();
      }
      i++;
    }
    draw_game();
    scr->commit();
  }
}
