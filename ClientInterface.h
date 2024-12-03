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
  virtual ~GameClientIF () = default;

  // caller provides the block of data
  // client will reallocate buffer and set new size if more data is needed
  virtual ClientError read_board(boost::uuids::uuid id, std::unique_ptr<char> buffer_ptr, int &buffer_len) = 0;

  // caller provides the block of data and the id
  virtual ClientError write_board(boost::uuids::uuid id, const char *buffer_ptr, int buffer_len) = 0;
};


#endif // CLIENTINTERFACE_H_
