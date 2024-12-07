#ifndef CHESSUI_H_
#define CHESSUI_H_

#include "ChessGame.h"
#include "Chess_generated.h"
#include "ClientInterface.h"
#include "FileClientInterface.h"
#include <map>
#include <memory>


namespace ZocoloChess {

enum ChessGameState {
  st_init,
  st_menu,
  st_load,
  st_setup,
  st_play,
  st_selected,
  st_exit,
};

enum ChessResponseType {
  load_game_rt,
  list_games_rt,
  new_game_rt,
  color_white_rt,
  color_black_rt,
  color_random_rt,
  color_observer_rt,
  cancel_rt,
  yes_rt,
  no_rt,
  concede_rt,
  select_piece_rt,
  move_selected_rt,
  move_rt,
  print_rt,
  error_rt,
  empty_rt,
  exit_rt,
  // needed for promo
  queen_rt,
  rook_rt,
  knight_rt,
  bishop_rt,
};



class SimpleChessUI {
  // chessBoard updates the data
  // client provides
  //    API for user to talk to chessboard
  //    API for user to load chessboard
  //    API for user to write chessboard state back
  std::unique_ptr<ChessGame> game;
  std::shared_ptr<GameClientIF> client; // shared with the game object
  // specific state logic
  // game logic
  std::string id;
  Color player_color;
  ChessGameState state;
  coordinate selectedPiece;

  // State machine
  Error run_menu();
  Error run_setup();
  Error run_play();
  Error run_load();
  Error run_list_games();
  Error run_move_selected();


  Error setup();
  Error create_board();
  Error play_game(); // requires game to be set

  ChessResponseType parse_user_command(const std::vector<ChessResponseType> &options,
                                       bool try_load = false,
                                       coordinate *coord = nullptr,
                                       coordinate *secondCoord = nullptr);
  ChessResponseType parse_coordinate_command(const std::vector<ChessResponseType> &options,
                                             std::vector<std::string> &words,
                                             coordinate *coord = nullptr,
                                             coordinate *secondCoord = nullptr);

  ChessResponseType parse_load_game(std::vector<std::string> &words);
  Error parse_coordinate(std::string input, coordinate *coord);

  Error move_piece(coordinate coord, coordinate destCoord);

public:
  int main_loop();
  SimpleChessUI() : state(st_init) {}

};

}


#endif // CHESSUI_H_
