#ifndef SYSTEM_UTILS_H_
#define SYSTEM_UTILS_H_

#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <format>
#include <sstream>
#include <streambuf>

// TODO: Add this to a proper system_utils file
// Defaults:
#define BLOCK_SIZE_DEFAULT 1024
#define BUFFER_BLOCKS_DEFAULT 10
#define PATH_DEFAULT "/tmp/ZocoloChess/"
static int block_size = -1;
static std::string path = "";


enum LogLevel { INFO=0, TRACE, DEBUG=2,  };

static LogLevel log_level = TRACE;

// TODO: add this to a system_utils file
void log(LogLevel level, std::string out);

#define LOG(out) log(LogLevel::INFO, (out))
#define TRACE(out) log(LogLevel::TRACE, (out))
#define DEBUG(out) log(LogLevel::DEBUG, (out))
#define CONTAINS(list, value)                                                  \
  (std::find((list).begin(), (list).end(), (value)) != (list).end())

template <typename CHAR_TYPE>
struct ostreambuf: public std::basic_streambuf<CHAR_TYPE, std::char_traits<CHAR_TYPE>> {
  // wrapper struct to expose setp(), ability to set stream's buffer
  //
  // calling the wrappers needs to be prefixed with `this->...` to
  // avoid hitting: explicit qualification required to use member 'setp'
  // from dependent base class.
  ostreambuf(CHAR_TYPE * pbegin, std::streamsize length) {
    this->setp(pbegin, pbegin+length);
  }
  void reset_ptr(CHAR_TYPE * pbegin, std::streamsize length) {
    this->setp(pbegin, pbegin+length);
  }
};

// overloads to make printing and streaming easy

#endif // SYSTEM_UTILS_H_
