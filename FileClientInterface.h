#ifndef FILECLIENTINTERFACE_H_
#define FILECLIENTINTERFACE_H_

#include "ClientInterface.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>

// TODO: Add this to a proper utils file
// Defaults:
#define BLOCK_SIZE_DEFAULT 1024
#define BUFFER_BLOCKS_DEFAULT 10
#define PATH_DEFAULT "/tmp/ZocoloChess/"
static int block_size = -1;
static std::string path = "";


enum LogLevel { INFO=0, TRACE, DEBUG=2,  };

static LogLevel log_level = TRACE;

// TODO: add this to a utils file
void log(LogLevel level, std::string out);

#define LOG(out) log(LogLevel::INFO, (out))
#define TRACE(out) log(LogLevel::TRACE, (out))
#define DEBUG(out) log(LogLevel::DEBUG, (out))
#define CONTAINS(list, value) (std::find((list).begin(), (list).end(), (value)) != (list).end())

// overloads to make printing and streaming easy


// TODO: creqte a static initializer to set variables like path and block size


class FileClientIF : public GameClientIF {
private:
  int buffer_size;
  std::unique_ptr<char> buffer;
  std::string path;
  std::string get_path();
  std::string file_path_from_id(boost::uuids::uuid id);
  ClientError resize_buffer(int messageSize);
public:
  ~FileClientIF() {} ;

  void init_buffer() override {
    buffer_size = BLOCK_SIZE_DEFAULT * BUFFER_BLOCKS_DEFAULT;
    buffer.reset(new char[buffer_size]);
  }
// client manages its own buffer for reads
  ClientError read_board(boost::uuids::uuid id, char **ret_buffer_ptr, int &buffer_len) override;
// caller provides the block of data and the id
  ClientError write_board(boost::uuids::uuid id, const char *buffer_ptr, int buffer_len) override;
  // data will be returned as a printable string
  // up to client implementation whether this is a db query, fs query, etc.
  // The format is up to the client implementation
  // client will reallocate buffer and set new size if more data is needed
  //
  // Amount of data queried is modifiable by the caller
  // by default, client returns entire list
  ClientError list_games(char **ret_buffer_ptr, int &ret_buffer_size, int startIndex = 0, int length = -1) override;
};


#endif // FILECLIENTINTERFACE_H_
