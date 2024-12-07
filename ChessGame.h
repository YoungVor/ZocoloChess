#ifndef CHESSGAME_H_
#define CHESSGAME_H_

#include <__concepts/destructible.h>
#include <_types/_uint8_t.h>
#include <flatbuffers/flatbuffers.h>
#include <memory>
#include <ostream>
#include <random>
#include <string>
#include <sstream>
#include <iostream>
#include <sys/_types/_int8_t.h>
#include <map>
#include <sys/_types/_uid_t.h>
#include <sys/_types/_uuid_t.h>
#include <unistd.h>
#include "ClientInterface.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
// #include <format>

//#include "Chess_generated.h"
//#include <cstddef>

namespace ZocoloChess {

#define BOARD_LENGTH 8
#define UP 1
#define DOWN -1
#define LEFT -1
#define RIGHT 1
#define HOLD 0
// enum class Collumn : int8_t {
// can't use strong typed enum without specifying the type where I use it
enum Collumn { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7, X=0xFF };
 enum Color { White = 0, Black = 1, Observer = 2, None = 3 };

 enum PieceType {
   Pawn = 0,
   Bishop,
   Knight,
   Rook,
   Queen,
   King = 5,
   Empty = 15,
 };

const std::map<PieceType, std::string> PieceToDisplay  {
  {Pawn,  "P"},
  {Bishop,  "B"},
  {Knight, "N"},
  {Rook, "R"},
  {Queen, "Q"},
  {King, "K"},
  {Empty, "empty"},
};


enum GameState {
  Turn,
  Promotion,
  Check,
  // game over:
  Checkmate,
  Conceded,
  Stalemate,
};

enum Error {
  valid = 0,
  invalid_move = 1,
  waiting_for_promotion,
  king_in_check,
  game_over,
  not_players_turn,
  bad_state,
  out_of_bounds,
  no_such_game,
};

struct coordinate {
  Collumn collumn;
  int8_t row;

  coordinate() {
  collumn = Collumn::X;
    row = 0;
  }
  coordinate(const coordinate &other) : collumn(other.collumn), row(other.row) {}
  coordinate(int8_t co, int8_t ro) {
    set(co,ro);
  }
  void set (int8_t co, int8_t ro) {
    if (co < 0 || co >= BOARD_LENGTH) {
      collumn = X;
    } else {
      collumn = (Collumn)co;
    }
    if (ro < 0 || ro >= BOARD_LENGTH) {
      row = -1;
    } else {
      row = ro;
    }
  }
  bool operator==(const coordinate &other) const {
    return other.collumn == collumn && other.row == row;
  }
  bool isValid() {
    return row >= 0 && row < BOARD_LENGTH && collumn >= A && collumn < BOARD_LENGTH;
  }
  coordinate inc(int rowUp, int colRight) { // ex. inc(UP,LEFT)
    //if (!isValid()) { return coordinate(); }
    return coordinate(collumn + colRight, row + rowUp);
  }
  coordinate incCol(short right) {
    //if (!isValid()) { return coordinate(); }
    return coordinate(collumn + right, row);
  }
  coordinate incRow(short up) {
    //if (!isValid()) { return coordinate(); }
    return coordinate(collumn, row + up);
  }
  coordinate incDiag(short up, short right) {
    //if (!isValid()) { return coordinate(); }
    return coordinate(collumn + right, row + up);
  }

};

struct move {
  coordinate piece;
  coordinate newPos;
  move(const move &other) : piece(other.piece), newPos(other.newPos) {};
  move(coordinate p, coordinate d) : piece(p), newPos(d) {};
};

struct Piece {
  coordinate position;
  Color color;
  PieceType type;
  Piece() :position(), color(None), type(Empty) {}
  Piece(coordinate pos) : position(pos), color(None), type(Empty) {}
  Piece(coordinate pos, Color co, PieceType ty) : position(pos), color(co), type(ty) {}
  bool isEmpty() {
    return (color == None) && (type == Empty);
  }
  std::string pretty_string();
};


class ChessGame {
private:
  //std::shared_ptr<char> board_data_ptr;
  std::shared_ptr<GameClientIF> client;
  //const Serializer::ChessBoard *board_data;
  boost::uuids::uuid id;
  Piece boardArray[8][8];
  std::vector<move> move_log;
  short moveCount;
  Piece *whiteKing;
  Piece *blackKing;
  bool whiteKingCanCastleA;
  bool whiteKingCanCastleH;
  bool blackKingCanCastleA;
  bool blackKingCanCastleH;
  GameState state;
  Color winner = None;
  //cache
 std::vector<coordinate> spaces_defended_white;
 std::vector<coordinate> spaces_defended_black;
  // do I need a reference to the buffer?
 public:
  ChessGame(std::shared_ptr<GameClientIF> cl);
  void init_new_board();

  // convenience functions
  Color playerTurn();
  void displayBoard();
  bool gameActive() { return state < Checkmate; }
  // NOTE: using
  // TODO: translate to classic chess notation
  Error isValidMove(coordinate piece, coordinate destination);
  Error isValidMove(); // allow king castles
  Color winningPlayer() { return winner; } // returns 'All' if still active
  Error promotePawn(coordinate position, PieceType newPiece);
  Error movePiece(coordinate piece, coordinate destination);
  Error playerConcedes(Color color);
  std::vector<coordinate> possibleMoves(Color player);
  std::vector<coordinate> possibleMoves(coordinate piece);
  Piece &selectSpace(coordinate coord);
  bool validEmptySpace(coordinate coord);
  bool validOccupiedSpaceByColor(coordinate coord, Color color);
  bool pieceUnderAttack(Piece &piece) {return false;} // TODO: figure this out.
 // One possible way - convenience function to determine if any piece is under attack

  std::string pretty_string(Color orientation);
  std::string state_string();
  Error load_board_data(char *read_buffer);
 private:
  Piece findPiece(coordinate position) {
    return boardArray[position.collumn][position.row];
  }
  std::vector<coordinate> spaces_defended(Color player);
  std::vector<coordinate> possible_pawn_moves(coordinate pos);
  std::vector<coordinate> possible_king_moves(coordinate pos);
  std::vector<coordinate> possible_queen_moves(coordinate pos);
  std::vector<coordinate> possible_rook_moves(coordinate pos);
  std::vector<coordinate> possible_bishop_moves(coordinate pos);
  std::vector<coordinate> possible_knight_moves(coordinate pos);
  void get_possible_moves_in_direction(coordinate pos, int colRight, int rowLeft, Color opponent, std::vector<coordinate> &retMoves);

  Error write_board_data();
  Error update_board_data();
  void update_board();
  void load_board(uid_t id);
  bool validate_board_state(); // check that board_data and board are in sync
};

//std::shared_ptr<char> CreateNewChessGameData(flatbuffers::FlatBufferBuilder64 &builder);

} // namespace ZocoloChess

namespace Serializer {
  struct Coord;
  struct Move;
  //std::shared_ptr<char> CreateNewChessGameData(flatbuffers::FlatBufferBuilder &builder);
}

namespace flatbuffers {
 Serializer::Coord Pack(const ZocoloChess::coordinate& obj);
 const ZocoloChess::coordinate UnPack(const Serializer::Coord& obj);
 Serializer::Move Pack(const ZocoloChess::move& obj);
 const ZocoloChess::move UnPack(const Serializer::Move& obj);
} // namespace flatbuffers



/*template<> struct std::formatter<ZocoloChess::coordinate> {
   constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
  auto format(const ZocoloChess::coordinate& c, std::format_context& ctx) const {
    return std::format_to(ctx.out(), "{}", 1); // TODO - try direct stream
  }
};
template<> struct std::formatter<ZocoloChess::Color> {
   constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
  auto format(const ZocoloChess::Color& c, std::format_context& ctx) const {
    return std::format_to(ctx.out(), "{}", to_string(c)); // TODO - try direct stream
  }
};
template<> struct std::formatter<ZocoloChess::Collumn> {
   constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
  auto format(const ZocoloChess::Collumn& c, std::format_context& ctx) const {
    return std::format_to(ctx.out(), "{}", 2); // TODO - try direct stream
  }
};
template<> struct std::formatter<ZocoloChess::PieceType> {
   constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
  auto format(const ZocoloChess::PieceType& t, std::format_context& ctx) const {
    return std::format_to(ctx.out(), "{}", 3); // TODO - try direct stream
  }
};*/





//class ChessPiece

// test: start with existing boards

// load / unpack game state






#endif // CHESSGAME_H_
