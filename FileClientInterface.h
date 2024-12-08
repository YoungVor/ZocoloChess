#ifndef FILECLIENTINTERFACE_H_
#define FILECLIENTINTERFACE_H_

#include "ClientInterface.h"
#include "system_utils.h"

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
