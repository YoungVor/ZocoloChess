#ifndef FILECLIENTINTERFACE_H_
#define FILECLIENTINTERFACE_H_

#include "ChessGame.h"
#include "Chess_generated.h"
#include "ClientInterface.h"
#include <memory>
#include <sys/_types/_uid_t.h>

class ChessClientFS : public GameClientIF<ZocoloChess::ChessGame> {
  std::shared_ptr<char> game_data;
public:
  ClientError read_board(uid_t id, ZocoloChess::ChessGame &game);
  ClientError write_board(uid_t id, ZocoloChess::ChessGame &game);
  ClientError load_board(ZocoloChess::ChessGame &game);
};

#endif // FILECLIENTINTERFACE_H_
