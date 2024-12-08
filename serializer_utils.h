#ifndef SERIALIZER_UTILS_H_
#define SERIALIZER_UTILS_H_

// Redefine serializer_utils - cannot depend on the other files
// separate file can define packing and tracing of game specific structs
#include "ChessGame.h"
#include "ChessUI.h"

std::ostream &operator<<(std::ostream &os, const ZocoloChess::Collumn &col);
std::ostream &operator<<(std::ostream &os, const ZocoloChess::Color &co);
std::ostream &operator<<(std::ostream &os, const LogLevel &ll);
std::ostream &operator<<(std::ostream &os, const ZocoloChess::coordinate &c);
std::ostream &operator<<(std::ostream &os, const ZocoloChess::PieceType &c);

template <typename CLASS> std::string to_string(const CLASS &obj) {
 std::stringstream ss;
 ss << obj;
 return ss.str();
}



#endif // SERIALIZER_UTILS_H_
