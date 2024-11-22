#ifndef CLIENTINTERFACE_H_
#define CLIENTINTERFACE_H_

#include "ChessGame.h"
#include <sys/_types/_uid_t.h>

enum ClientError {
  success = 0,
  does_not_exist,
  unknown_error,
};

template<class GAME>
class GameClientIF {
public:
  virtual ~GameClientIF<GAME> () {}
  virtual ClientError read_board(uid_t id, GAME &game) = 0;
  virtual ClientError write_board(uid_t id, GAME &game) = 0;
  virtual ClientError load_board(GAME &game) = 0;
};


#endif // CLIENTINTERFACE_H_
