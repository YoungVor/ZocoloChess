#ifndef UTILS_H_
#define UTILS_H_

#include "ChessGame.h"
#include "ChessUI.h"

enum LogLevel { INFO=0, TRACE, DEBUG=2,  };

static LogLevel log_level = TRACE;

// TODO: add this to a utils file
void log(LogLevel level, std::string out);

#define LOG(out) log(LogLevel::INFO, (out))
#define TRACE(out) log(LogLevel::TRACE, (out))
#define DEBUG(out) log(LogLevel::DEBUG, (out))
#define CONTAINS(list, value) (std::find((list).begin(), (list).end(), (value)) != (list).end())

// overloads to make printing and streaming easy
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


#endif // UTILS_H_
