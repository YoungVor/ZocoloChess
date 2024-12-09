#ifndef UTILS_H_
#define UTILS_H_

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

#endif // UTILS_H_
