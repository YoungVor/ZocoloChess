#ifndef CHESSGAME_H_
#define CHESSGAME_H_

#include <_types/_uint8_t.h>
#include <flatbuffers/flatbuffers.h>
#include <memory>
#include <ostream>
#include <string>
#include <sys/_types/_int8_t.h>
#include <map>
//#include "Chess_generated.h"
//#include <cstddef>

namespace ChessGame {
// enum class Collumn : int8_t {
// can't use strong typed enum without specifying the type where I use it
enum Collumn { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7, X=15 };



//TODO: redefine generated Coord with this using native_type
struct coordinate {
  int8_t row;
  Collumn collumn;

  coordinate() {
    row = 0;
    collumn = Collumn::X;
  }
  coordinate(int8_t row, int8_t collumn) : row(row), collumn((Collumn)collumn) {
  }

};
} // namespace ChessGame

namespace ZocoloChess {
  struct Coord;
  }

namespace flatbuffers {
 ZocoloChess::Coord Pack(const ChessGame::coordinate& obj);
 const ChessGame::coordinate UnPack(const ZocoloChess::Coord& obj);

 } // namespace flatbuffers

std::ostream& operator<< (std::ostream& os, const ChessGame::Collumn &col);
std::ostream &operator<<(std::ostream &os, const ChessGame::coordinate &c);

std::string to_string(const ChessGame::Collumn &col);
std::string to_string(const ChessGame::coordinate &c);








//class ChessPiece

// test: start with existing boards

// load / unpack game state






#endif // CHESSGAME_H_
