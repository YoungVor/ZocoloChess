#include "serializer_utils.h"
#include "ChessGame.h"
#include "Chess_generated.h"

#include <_types/_uint8_t.h>
#include <ios>
#include <memory>
#include <ostream>
#include <string>
#include <sys/_types/_int32_t.h>
#include <iostream>
#include <fstream>
#include <sstream>


using namespace ZocoloChess;
using std::to_string;


// TODO: add this to a serializer_utils file
void log(LogLevel level, std::string out) {
  if (log_level >= level) {
    std::cout << "(" << log_level <<  ")" << out << std::endl;
  }
};


std::ostream& operator<< (std::ostream& os, const Collumn &col)
{
  switch (col) {
    case(A): os << "A"; break;
    case(B): os << "B"; break;
    case(C): os << "C";  break;
    case(D): os << "D";  break;
    case(E): os << "E";  break;
    case(F): os << "F";  break;
    case(G): os << "G";  break;
    case(H): os << "H";  break;
    case(X): os << "X";  break;
  };
  return os;
};

std::ostream& operator<< (std::ostream& os, const LogLevel &ll)
{
  switch (ll) {
    case(INFO): os << "INFO"; break;
    case(TRACE): os << "TRACE"; break;
    case(DEBUG): os << "DEBUG";  break;
  };
  return os;
};

std::ostream& operator<< (std::ostream& os, const Color &co)
{
  switch (co) {
    case(White): os << "White"; break;
    case(Black): os << "Black"; break;
    case(None): os << "None";  break;
  case(Observer) : os << "Observer"; break;
  };
  return os;
};

std::ostream& operator<<(std::ostream& os, const coordinate &c) {
  return os << c.collumn << (int)c.row;
}
/*
template<typename CLASS>
std::string to_string(const CLASS &obj) {
  std::stringstream ss;
  ss << obj;
  return ss.str();
}*/


namespace flatbuffers {
  Serializer::Coord Pack(const coordinate& obj) {
    return Serializer::Coord(obj.collumn, obj.row);
  }

  const coordinate UnPack(const Serializer::Coord& obj) {
    return coordinate(obj.column(), obj.row());
  }

  Serializer::Move Pack(const move& obj) {
    return Serializer::Move(Pack(obj.piece), Pack(obj.newPos));
  }

  const move UnPack(const Serializer::Move& obj) {
    return move(UnPack(obj.piece()), UnPack(obj.dest()));
  }

}
