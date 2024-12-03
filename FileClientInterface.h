#ifndef FILECLIENTINTERFACE_H_
#define FILECLIENTINTERFACE_H_

#include "ClientInterface.h"
#include <iostream>
#include <fstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>

// TODO: Add this to a proper utils file
// Defaults:
#define BLOCK_SIZE_DEFAULT 1024
#define PATH_DEFAULT "/tmp/ZocoloChess"
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
public:
  ~FileClientIF() {}
// caller provides the block of data
// client will reallocate it if more data is needed
  ClientError read_board(boost::uuids::uuid id, std::unique_ptr<char> buffer_ptr, int &buffer_len) override;
// caller provides the block of data and the id
  ClientError write_board(boost::uuids::uuid id, const char *buffer_ptr, int buffer_len) override;
};


#endif // FILECLIENTINTERFACE_H_
