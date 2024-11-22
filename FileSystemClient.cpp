#include "FileSystemClient.h"
#include "ChessGame.h"
#include "ClientInterface.h"
#include <sys/_types/_uid_t.h>

ClientError ChessClientFS::read_board(uid_t id, ZocoloChess::ChessGame &game) {
  ClientError err = success;
  return err;
}


ClientError ChessClientFS::write_board(uid_t id, ZocoloChess::ChessGame &game) {
  ClientError err = success;
  return err;
}


ClientError ChessClientFS::load_board(ZocoloChess::ChessGame &game) {
  ClientError err = success;
  return err;
}
