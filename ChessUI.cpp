#include "ChessUI.h"
#include "ChessGame.h"
#include <cassert>
#include <memory>
#include <ostream>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <__format/format_functions.h>


//
// Menu
//   Load Game [ game_id ]
//   New Game
//
// Setup
//   Select Color [ white, black, both ]
//
// Play Loop [
//   Enter Piece [ A4 ]
//      Enter Move [ possible moves ]
//   Castle
//   Save???
//   Concede
//   Exit Game
// ]
//
// Promote Pawn
//    Select Piece [ Q, B, N, R ]
//
// prompt loop -
// * maps response to state
// * state dictates the prompt and possible responses. prompt can have some dynasism
// * parse
// *   will see if command matches possible prompts
// *   calls the correct response function
//
//

using namespace ZocoloChess;


const std::map<std::string, ChessResponseType> ResponseMap{
    {"l", load_game_rt},    {"load", load_game_rt},
    {"n", load_game_rt},    {"new", load_game_rt},
    {"w", color_white_rt},  {"white", color_white_rt},
    {"b", color_black_rt},  {"black", color_black_rt},
    {"m", color_random_rt}, {"random", color_random_rt},
    {"a", color_all_rt},    {"all", color_all_rt},
    {"d", concede_rt},      {"concede", concede_rt},
    {"c", cancel_rt},       {"cancel", cancel_rt},
    {"y", yes_rt},          {"yes", yes_rt},
    {"n", no_rt},           {"no", no_rt},
    {"x", exit_rt}, {"exit", exit_rt},
    {"q", queen_rt},           {"queen", queen_rt},
    {"r", rook_rt},           {"rook", rook_rt},
    {"k", knight_rt},           {"knight", knight_rt},
    {"i", bishop_rt},           {"bishop", bishop_rt},
};

const std::map<char, int8_t> CollumnMap{
    {'a', A}, {'A', A}, {'b', B}, {'B', B}, {'c', C}, {'C', C},
    {'d', D}, {'D', D}, {'e', E}, {'E', E}, {'f', F}, {'F', F},
    {'g', G}, {'G', G}, {'h', H}, {'H', H},
};
const std::map<ChessResponseType, PieceType> PieceMap {
  { queen_rt, Queen },
  { bishop_rt, Bishop },
  { rook_rt, Rook },
  { knight_rt, Knight },
};



Error SimpleChessUI::parse_coordinate(std::string input, coordinate *coord) {
  assert(coord != nullptr);
  assert(input.size() == 2);
  auto col = CollumnMap.find(input[0]);
  if (col == CollumnMap.end()) {
    return out_of_bounds;
  }
  int row = std::stoi(input.substr(1,1));
  *coord = coordinate(col->second, row);
  return valid;
}

ChessResponseType SimpleChessUI::parse_coordinate_command(const std::vector<ChessResponseType> &options,
                                                          std::vector<std::string> &words,
                                                          coordinate *coord,
                                                          coordinate *secondCoord) {
  assert(coord != nullptr);
  // parse first coord for piece selection or move selected
  if (!((CONTAINS(options, select_piece_rt) ||
       CONTAINS(options, move_selected_rt) ||
       CONTAINS(options, move_rt)) &&
        words.size() <= 2)) {
    return error_rt;
  }
  if (words.front().size() == 2) {
    parse_coordinate(words.front(), coord);
    if (words.size() == 1) {
      if (CONTAINS(options, select_piece_rt)) { return select_piece_rt; }
      if (CONTAINS(options, move_selected_rt)) { return move_selected_rt; }
    } else {
      if (words.size() != 2 ||
          !CONTAINS(options,move_rt) ||
          words.at(1).length() != 2) { return error_rt; } // case: [D4 E5]
      assert(secondCoord != nullptr);
      parse_coordinate(words.at(1), secondCoord);
    }
  } else if (words.front().size() == 5) {
    if (words.size() != 1 ||
      !CONTAINS(options, move_rt) ||
          (words.front().at(2) != 'x' &&
           words.front().at(2) != 'X')) { return error_rt; } // case: D4xE5
      assert(secondCoord != nullptr);
      parse_coordinate(words.front().substr(0,2), coord);
      parse_coordinate(words.front().substr(2,2), secondCoord);
      return move_rt;
  }
  return error_rt;
}

ChessResponseType SimpleChessUI::parse_user_command(const std::vector<ChessResponseType> &options,
                                                    coordinate *coord,
                                                    coordinate *secondCoord)
{
  std::string word;
  std::vector<std::string> words;
  while (std::getline(std::cin, word, ' ')) {
    if (!word.empty()) {
      words.push_back(word);
    }
  }
  //DEBUG(std::format("input length",))
  if (words.empty()) {
    return empty_rt;
  }
  auto resp = ResponseMap.find(words.front());
  if (resp != ResponseMap.end() &&
      CONTAINS(options, resp->second)) { // found command
    // TODO: handle arguments
    return resp->second;
  }
  auto coordResp = parse_coordinate_command(options, words, coord, secondCoord);
  if (coordResp != error_rt) {
    return coordResp;
  }
  return error_rt;
}

Error SimpleChessUI::run_menu() {
  std::cout << "Options:" << std::endl;
  std::cout << "  [l]/[load] game" << std::endl;
  std::cout << "  [n]/[new] game" << std::endl;
  std::cout << "  [q]/[quit] " << std::endl;
  auto resp = parse_user_command({ new_game_rt, load_game_rt, exit_rt });
  switch(resp) {
  case new_game_rt:
    game.reset(new ChessGame ());
    state = st_setup;
  case load_game_rt:
    std::cout << "Loading Game not supported yet.";
    return invalid_move;
  case exit_rt:
    game.reset();
    state = st_exit;
    return valid;
  default:
    return bad_state;
  }
}

Error SimpleChessUI::run_setup() {
  std::cout << "Select a color: [w]hite, [b]lack, rando[m], [a]ll: ";
  auto resp = parse_user_command({ color_white_rt, color_black_rt, color_all_rt, color_random_rt });
  switch (resp) {
  case color_white_rt:
    player_color = White;
    break;
  case color_black_rt:
    player_color = Black;
    break;
  case color_random_rt:
    player_color = random() % 2 ? White : Black;
    break;
  case color_all_rt:
    player_color = All;
    break;
  default:
    return bad_state;
  }
  // transition to playing
  std::cout << game->pretty_string() << std::endl;
  state = st_play;
  return valid;
}

Error SimpleChessUI::run_play() {
  std::cout << game->state_string() << std::endl;
  std::vector<ChessResponseType> options = { exit_rt, print_rt };
  if (game->winningPlayer() != All) {
    std::cout << "  [p, print] the board )" << std::endl;
    std::cout << "  [x, exit]" << std::endl;
  } else {
    std::cout << "  [c, concede] the game" << std::endl;
    options.push_back(concede_rt);
    if (player_color != All && player_color != game->playerTurn()) {
        std::cout << "Waiting for other player." << std::endl;
    } else {
      // Player's turn
        std::cout << "Options:" << std::endl;
        std::cout << "  Select a Piece. example: [A4]/[a4] )" << std::endl;
        std::cout << "  Make a move. example: [A4xA6] )" << std::endl;
        options.push_back(select_piece_rt);
        options.push_back(move_rt);
    }
  }
  coordinate coord;
  coordinate destCoord;
  auto resp = parse_user_command(options);
  switch (resp) {
  case select_piece_rt:
  case move_rt: {
        auto piece = game->selectSpace(coord);
        if (piece.color != game->playerTurn()) {
          std::cout << "Please select a '" << game->playerTurn() << "' piece." << std::endl;
        } else if (resp == select_piece_rt) {
          state = st_selected;
          selectedPiece = coord;
        } else {// move_t
            return move_piece(coord, destCoord);
        }
        break;
  }
  case concede_rt: {
        if (player_color == All) {
            std::cout << "You are not a formal player. Only white or black player can concede." << std::endl;
        }
        std::cout << "Are you sure? [y]es/[n]o: " ;
      {
        auto confirm = parse_user_command( { yes_rt, no_rt } );
        if (confirm == yes_rt) {
            game->playerConcedes(player_color);
        }
      }
  }
  case print_rt: {
        std::cout << game->pretty_string() << std::endl;
        break;
  }
  case exit_rt: {
        std::cout << "Are you sure? [y]es/[n]o: " ;
      {
        auto confirm = parse_user_command( { yes_rt, no_rt } );
            if (confirm == yes_rt) {
            state = st_init;
        }
      }
  }
  default: {
    return bad_state;
  }
  }
  return valid;
}

Error SimpleChessUI::run_move_selected() {
  auto possibleMoves = game->possibleMoves(selectedPiece);
  coordinate destCoord;
  std::cout << "Make a move (";
  for (auto m : possibleMoves) { std::cout << m << ","; }
  std::cout << ")" << std::endl;
  std::cout << "or [c]ancel";
  auto resp = parse_user_command( { move_selected_rt, cancel_rt }, &destCoord);
  switch (resp) {
  case move_selected_rt: {
    if (move_piece(selectedPiece, destCoord) == valid) {
      state = st_play;
    }
    break;
  }
  case cancel_rt: {
    selectedPiece = coordinate(); // reset selected
    state = st_play;
    break;
  }
  default: {
    return bad_state;
  }
  }
  return valid;
}

Error SimpleChessUI::move_piece(coordinate coord, coordinate destCoord) {
  auto ret = game->movePiece(coord, destCoord);
  switch(ret) {
  case waiting_for_promotion: {
    std::cout << "Promote pawn, choose [q]ueen, [r]ook, [k]night, b[i]shop, or [c]ancel: ";
    ChessResponseType pieceChoice = queen_rt;
    while (pieceChoice == error_rt) {
      pieceChoice = parse_user_command( { queen_rt, rook_rt, knight_rt, bishop_rt } );
      if (pieceChoice == cancel_rt) {
        state = st_play;
        break;
      }
      if (pieceChoice == queen_rt || pieceChoice == rook_rt || pieceChoice == knight_rt || pieceChoice == bishop_rt) {
        game->promotePawn(destCoord, PieceMap.at(pieceChoice));
        break;
      }
    } // fall through to valid move
  }
  case valid: {
    std::cout << game->pretty_string();
    return valid;
  }
  case invalid_move: {
    std::cout << coord << " -> " << destCoord << " is not a valid move." << std::endl;
    return invalid_move;
  }
  default: {
    std:: cout << "Error, game returned'" << ret << "'" << std::endl;
    return bad_state;
  }
  }
  return valid;
}

int SimpleChessUI::main_loop() {
  Error err;
  while(true) {
    switch (state) {
    case st_init:
      std::cout << "Welcome to Zocolo Chess Text Player!" << std::endl;
      state = st_menu;
    case st_menu:
      err = run_menu();
      break;
    case st_setup:
      err = run_setup();
      break;
    case st_play:
      err = run_play();
      break;
    case st_selected:
      err = run_move_selected();
      break;
    case st_exit:
      std::cout << "See you next time!" << std::endl;
      return 0;
    }
  }
}
