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

namespace ZocoloChess {
// enum class Collumn : int8_t {
// can't use strong typed enum without specifying the type where I use it
enum Collumn { A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7, X=15 };



//TODO: redefine generated Coord with this using native_type
struct coordinate {
  Collumn collumn;
  int8_t row;

coordinate() {
  collumn = Collumn::X;
    row = 0;
  }
coordinate(int8_t collumn, int8_t row) : collumn((Collumn)collumn), row(row) {
  }
};


} // namespace ZocoloChess

namespace Serializer {
  struct Coord;
  }

namespace flatbuffers {
 Serializer::Coord Pack(const ZocoloChess::coordinate& obj);
 const ZocoloChess::coordinate UnPack(const Serializer::Coord& obj);

 } // namespace flatbuffers

std::ostream& operator<< (std::ostream& os, const ZocoloChess::Collumn &col);
std::ostream &operator<<(std::ostream &os, const ZocoloChess::coordinate &c);

std::string to_string(const ZocoloChess::Collumn &col);
std::string to_string(const ZocoloChess::coordinate &c);








//class ChessPiece

// test: start with existing boards

// load / unpack game state






#endif // CHESSGAME_H_
