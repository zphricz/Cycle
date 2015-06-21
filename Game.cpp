#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <iomanip>
#include <random>
#include "Game.h"
#include "ZackAI.h"

SDL_Color bg_color{25, 25, 25};
SDL_Color trail_color1{0, 200, 0};
SDL_Color trail_color2{0, 0, 255};
SDL_Color dead_color{100, 0, 0};

Game::Game(int num_x, int num_y, SoftScreen *screen)
    : num_cells_x(num_x), num_cells_y(num_y), scr(screen), game_running(true) {
  if (num_cells_x < 4 || num_cells_y < 4) {
    std::cout << "ERROR: Too few cells to play with" << std::endl;
    exit(1);
  }
  if (num_cells_x > scr->width / 2 || num_cells_y > scr->height / 2) {
    std::cout << "ERROR: Too many cells to display on this screen" << std::endl;
    exit(1);
  }
  num_players = 2;
  grid.resize(num_cells_x * num_cells_y);
  players.resize(num_players);
  scr->set_recording_style("images", 5);
}

Game::~Game() {}

bool Game::all_over() {
  bool is_all_over = true;
  for (Player &player : players) {
    if (!player.game_over) {
      is_all_over = false;
      break;
    }
  }
  return is_all_over;
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
        if (players[1].last_move != Direction::RIGHT && !game_paused) {
          players[1].direction = Direction::LEFT;
        }
        break;
      }
      case 'd': {
        if (players[1].last_move != Direction::LEFT && !game_paused) {
          players[1].direction = Direction::RIGHT;
        }
        break;
      }
      case 'w': {
        if (players[1].last_move != Direction::DOWN && !game_paused) {
          players[1].direction = Direction::UP;
        }
        break;
      }
      case 's': {
        if (players[1].last_move != Direction::UP && !game_paused) {
          players[1].direction = Direction::DOWN;
        }
        break;
      }
      case SDLK_LEFT: {
        if (players[0].last_move != Direction::RIGHT && !game_paused) {
          players[0].direction = Direction::LEFT;
        }
        break;
      }
      case SDLK_RIGHT: {
        if (players[0].last_move != Direction::LEFT && !game_paused) {
          players[0].direction = Direction::RIGHT;
        }
        break;
      }
      case SDLK_UP: {
        if (players[0].last_move != Direction::DOWN && !game_paused) {
          players[0].direction = Direction::UP;
        }
        break;
      }
      case SDLK_DOWN: {
        if (players[0].last_move != Direction::UP && !game_paused) {
          players[0].direction = Direction::DOWN;
        }
        break;
      }
      case SDLK_SPACE:
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
        } else if (team == 0) {
          scr->fill_rect(x1, y1, x2, y2, trail_color1);
        } else if (team == 1) {
          scr->fill_rect(x1, y1, x2, y2, trail_color2);
        } else {
          // what do?
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
      if (collides_with_trail(next) || out_of_bounds(next)) {
        player.game_over = true;
        if (all_over()) {
          game_paused = true;
        }
      } else {
        player.head = next;
        grid_at(next) = index;
      }
    }
    index++;
  }
}

void Game::init_game() {
  fill(grid.begin(), grid.end(), -1);
  game_paused = true;
  players[0].ai_player = std::make_unique<ZackAI>(num_cells_x, num_cells_y, 0);
  players[1].ai_player = std::make_unique<ZackAI>(num_cells_x, num_cells_y, 1);
  players[0].direction = Direction::RIGHT;
  players[1].direction = Direction::LEFT;
  players[0].head = {0, num_cells_y / 2};
  players[1].head = {num_cells_x - 1, num_cells_y / 2};
  int index = 0;
  for (Player &player : players) {
    player.score = 0;
    player.game_over = false;
    player.last_move = Direction::NONE;
    player.ai_plays = false;
    grid_at(player.head) = index;
    index++;
  }
  std::cout << "Game Start" << std::endl;
}

// Runs the game loop
void Game::play() {
  init_game();
  int i = 0;
  while (game_running) {
    handle_input();
    if (!game_paused) {
      if (i % frames_per_update == 0) {
        std::vector<PlayerState> player_states;
        player_states.reserve(num_players);
        for (Player &player : players) {
          player_states.push_back(player.get_player_state());
        }
        int index = 0;
        for (Player &player : players) {
          if (player.ai_plays) {
            player.ai_player->init_move(grid, player_states);
            player.direction = player.ai_player->move();
          }
          index++;
        }
        step_game();
      }
      i++;
    }
    draw_game();
    scr->commit();
  }
}
