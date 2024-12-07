#ifndef CLIENTINTERFACE_H_
#define CLIENTINTERFACE_H_

#include <boost/uuid/uuid.hpp>

enum ClientError {
  success = 0,
  does_not_exist,
  unknown_error,
};

class GameClientIF {
public:
  //GameClientIF() {};
  virtual ~GameClientIF () = default;
  virtual void init_buffer() = 0;

  // caller provides the block of data
  // client will reallocate buffer and set new size if more data is needed
  virtual ClientError read_board(boost::uuids::uuid id, char **ret_buffer_ptr, int &buffer_len) = 0;

  // caller provides the block of data and the id
  virtual ClientError write_board(boost::uuids::uuid id, const char *buffer_ptr, int buffer_len) = 0;

  // caller provides block of data.
  // will be returned as a printable string
  // up to client implementation whether this is a db query, fs query, etc.
  // The format is up to the client implementation
  // client will reallocate buffer and set new size if more data is needed
  //
  // Amount of data queried is modifiable by the caller
  // by default, client returns entire list
  virtual ClientError list_games(char **ret_buffer_ptr, int &ret_buffer_size, int startIndex = 0, int length = -1) = 0;
};


#endif // CLIENTINTERFACE_H_
