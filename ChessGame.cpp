#include "ChessGame.h"
#include "Chess_generated.h"
//#include <format>
#include <_types/_uint8_t.h>
#include <memory>
#include <string>
#include <sys/_types/_int32_t.h>
#include <iostream>
#include <vector>

using namespace ZocoloChess;


ChessGame::ChessGame() {
  //flatbuffers::FlatBufferBuilder builder(1024);
  //board_data_ptr = CreateNewChessGameData(builder);
  load_board_data();
}

void ChessGame::load_board_data() {
  //board_data = Serializer::GetChessBoard(board_data_ptr.get());
}

Color ChessGame::playerTurn() {
  return White;
}

std::string ChessGame::pretty_string() { return "...................."; }

std::string ChessGame::state_string() {
  std::stringstream ss;
  switch(state) {
  case Check:
    ss << "Check! ";
  case Turn:
    ss << playerTurn() << "'s turn.";
    return ss.str();
  case Promotion:
    return "Waiting for player to choose pawn promotion";
  case Checkmate:
  case Conceded:
    ss << winningPlayer() << " wins" << (state == Checkmate ?
                                         " by checkmate!" : "! Player conceded.");
    return ss.str();
  case Stalemate:
    return "Game over: Stalemate.";
  }
}

Error ChessGame::movePiece(coordinate pos, coordinate dest) {
  // Check if
  //Piece piece = boardArray[pos.collumn][pos.row];
  // Check if valid move
  Error isValid = isValidMove(pos, dest);
  if (isValid != Error::valid) {
    return isValid;
  }

  // update state

  // check if piece is taken

  // special case for castle

  return isValid;
}

Error ChessGame::promotePawn(coordinate position, PieceType newPiece) {
  return invalid_move;
}

Error ChessGame::playerConcedes(Color color) {
  if (gameActive()) { // game's already over, can't concede
    return invalid_move;
  }
  state = Conceded;
  winner = (color == White) ? Black : White;
  return valid;
}

Error ChessGame::isValidMove(coordinate pos, coordinate dest) {
  Piece &piece = boardArray[pos.collumn][pos.row];
  // check if game is over, and that its current player's turn
  if (!gameActive()) {
    return Error::game_over;
  }
  if (playerTurn() != piece.color) {
    return Error::not_players_turn;
  }
  if (piece.type == Empty) {
    return Error::invalid_move;
  }
  // check if king is in check
  if (piece.type != King &&
      piece.color == White ? pieceUnderAttack(*whiteKing) : pieceUnderAttack(*blackKing)) {
    return Error::king_in_check;
  }
  // get the possible moves
  std::vector<coordinate> moves;
  switch (piece.type) {
  case Pawn: moves = possible_pawn_moves(pos,piece.color); break;
  case Bishop: moves = possible_bishop_moves(pos,piece.color); break;
  case Knight: moves = possible_knight_moves(pos,piece.color); break;
  case Rook: moves = possible_rook_moves(pos,piece.color); break;
  case Queen: moves = possible_queen_moves(pos,piece.color); break;
  case King: moves = possible_king_moves(pos,piece.color); break;
  default:  return Error::bad_state;
  }
  if (std::find(moves.begin(), moves.end(), dest) != moves.end()) {
    return Error::valid;
  } else {
    return Error::invalid_move;
  }
}

Piece ChessGame::selectSpace(coordinate coord) {
  return boardArray[coord.collumn][coord.row];
}
std::vector<coordinate> ChessGame::possibleMoves(coordinate pos) {
  auto piece = selectSpace(pos);
  switch (piece.type) {
  case Pawn: return possible_pawn_moves(pos,piece.color);
  case Bishop: return possible_bishop_moves(pos,piece.color);
  case Knight: return possible_knight_moves(pos,piece.color);
  case Rook: return possible_rook_moves(pos,piece.color);
  case Queen: return possible_queen_moves(pos,piece.color);
  case King: return possible_king_moves(pos,piece.color);
  default: {
    //TRACE("no piece at position")
    return std::vector<coordinate>();
  }
  }
}

std::vector<coordinate> ChessGame::possible_pawn_moves(coordinate pos, Color color) {
  // get color and direction
  Piece &piece = boardArray[pos.collumn][pos.row];
  return std::vector<coordinate>();
}
std::vector<coordinate> ChessGame::possible_king_moves(coordinate pos, Color color){ return std::vector<coordinate>(); }
std::vector<coordinate> ChessGame::possible_queen_moves(coordinate pos, Color color){ return std::vector<coordinate>(); }
std::vector<coordinate> ChessGame::possible_rook_moves(coordinate pos, Color color){ return std::vector<coordinate>(); }
std::vector<coordinate> ChessGame::possible_bishop_moves(coordinate pos, Color color){ return std::vector<coordinate>(); }
std::vector<coordinate> ChessGame::possible_knight_moves(coordinate pos, Color color){ return std::vector<coordinate>(); }
