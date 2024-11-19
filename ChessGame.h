#ifndef CHESSGAME_H_
#define CHESSGAME_H_

#include <_types/_uint8_t.h>
#include <flatbuffers/flatbuffers.h>
#include <memory>
#include <ostream>
#include <string>
#include <sstream>
#include <iostream>
#include <sys/_types/_int8_t.h>
#include <map>
#include <unistd.h>
// #include <format>

//#include "Chess_generated.h"
//#include <cstddef>

namespace ZocoloChess {

#define BOARD_LENGTH 8

enum LogLevel { INFO=0, TRACE, DEBUG=2,  };

static LogLevel log_level = DEBUG;

// TODO: add this to a utils file
void log(LogLevel level, std::string out);

#define LOG(out) log(LogLevel::DEBUG, (out))
#define TRACE(out) log(LogLevel::DEBUG, (out))
#define DEBUG(out) log(LogLevel::DEBUG, (out))
#define CONTAINS(list, value) (std::find((list).begin(), (list).end(), (value)) != (list).end())

// enum class Collumn : int8_t {
// can't use strong typed enum without specifying the type where I use it
enum Collumn { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7, X=15 };
  enum Color { White = 0, Black = 1, All = 2 };
enum PieceType {
  Pawn = 0,
  Bishop,
  Knight,
  Rook,
  Queen,
  King = 5,
  Empty = 15,
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
};

struct coordinate {
  Collumn collumn;
  int8_t row;

  coordinate() {
  collumn = Collumn::X;
    row = 0;
  }
  coordinate(int8_t collumn, int8_t row) : collumn((Collumn)collumn), row(row) {
  }

  bool operator==(const coordinate &other) const {
    return other.collumn == collumn && other.row == row;
  }
};

struct Piece {
  coordinate position;
  Color color;
  PieceType type;
  Piece() :position(), color(All), type(Empty) {}
  Piece(coordinate pos) : position(pos), color(All), type(Empty) {}
  Piece(coordinate pos, Color co, PieceType type) : position(pos), color(All), type(Empty) {}
};


class ChessGame {
private:
  std::shared_ptr<char> board_data_ptr;
  //const Serializer::ChessBoard *board_data;
  Piece boardArray[8][8];
  Piece *whiteKing;
  Piece *blackKing;
  bool whiteKingMoved;
  bool blackKingMoved;
  GameState state;
  Color winner = All;
  //cache
 std::vector<coordinate> spaces_defended_white;
 std::vector<coordinate> spaces_defended_black;
  // do I need a reference to the buffer?
 public:
  ChessGame();
  ChessGame(ChessGame &other);

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
  Piece selectSpace(coordinate coord);
  bool pieceUnderAttack(Piece &piece) {return true;} // TODO: figure this out.
 // One possible way - convenience function to determine if any piece is under attack

  std::string pretty_string();
  std::string state_string();
 private:
  Piece findPiece(coordinate position) {
    return boardArray[position.collumn][position.row];
  }
  void updateBoard();
  std::vector<coordinate> spaces_defended(Color player);
  std::vector<coordinate> possible_pawn_moves(coordinate pos, Color color);
  std::vector<coordinate> possible_king_moves(coordinate pos, Color color);
  std::vector<coordinate> possible_queen_moves(coordinate pos, Color color);
  std::vector<coordinate> possible_rook_moves(coordinate pos, Color color);
  std::vector<coordinate> possible_bishop_moves(coordinate pos, Color color);
  std::vector<coordinate> possible_knight_moves(coordinate pos, Color color);
  void load_board_data();
  bool validate_board_state(); // check that board_data and board are in sync
};

//std::shared_ptr<char> CreateNewChessGameData(flatbuffers::FlatBufferBuilder64 &builder);

} // namespace ZocoloChess

namespace Serializer {
  struct Coord;
  //std::shared_ptr<char> CreateNewChessGameData(flatbuffers::FlatBufferBuilder &builder);
}

namespace flatbuffers {
 Serializer::Coord Pack(const ZocoloChess::coordinate& obj);
 const ZocoloChess::coordinate UnPack(const Serializer::Coord& obj);
} // namespace flatbuffers


// overloads to make printing and streaming easy
 std::ostream &operator<<(std::ostream &os, const ZocoloChess::Collumn &col);
std::ostream &operator<<(std::ostream &os, const ZocoloChess::Color &co);

std::ostream &operator<<(std::ostream &os, const ZocoloChess::coordinate &c);
std::ostream &operator<<(std::ostream &os, const ZocoloChess::PieceType &c);

std::string to_string(const ZocoloChess::Collumn &col);
std::string to_string(const ZocoloChess::Color &co);
std::string to_string(const ZocoloChess::coordinate &c);
std::string to_string(const ZocoloChess::PieceType &c);

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
