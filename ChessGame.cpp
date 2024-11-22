#include "ChessGame.h"
#include "Chess_generated.h"
#include "utils.h"
#include <cassert>
#include <format>
#include <_types/_uint8_t.h>
#include <memory>
#include <sstream>
#include <string>
#include <sys/_types/_int32_t.h>
#include <iostream>
#include <sys/_types/_uid_t.h>
#include <vector>

using namespace ZocoloChess;

std::string Piece::pretty_string() {
  DEBUG(std::format("piece string: empty:{} type:{} color: {}", isEmpty(), PieceToDisplay.at(type), (int)color));

    std::stringstream ss;
    if (isEmpty()) { return ""; }
    ss << "{ " << (color == White ? "W" : "B") << " " << PieceToDisplay.at(type)
       << " }";
    return ss.str();
}

ChessGame::ChessGame(GameClientIF<ChessGame> *cl) {
  client.reset(cl);
  init_new_board();
}

void ChessGame::load_board(uid_t id) {
  //board_data = Serializer::GetChessBoard(board_data_ptr.get());
}

void ChessGame::init_new_board() {
  DEBUG("init board");
  auto piece = Piece(coordinate(A,0), White, Rook);
   std::stringstream ss;
  boardArray[A][0] = piece;
  ss << "piece at A,0: " << boardArray[A][0].pretty_string() << std::endl;
  ss << "assigned at A,0: " << piece.pretty_string() << std::endl;


  boardArray[H][0] = Piece(coordinate(H,0), White, Rook);
  boardArray[B][0] = Piece(coordinate(C,0), White, Bishop);
  boardArray[G][0] = Piece(coordinate(F,0), White, Bishop);
  boardArray[C][0] = Piece(coordinate(B,0), White, Knight);
  boardArray[F][0] = Piece(coordinate(G,0), White, Knight);
  boardArray[E][0] = Piece(coordinate(E,0), White, King);
  boardArray[D][0] = Piece(coordinate(D,0), White, Queen);

  boardArray[A][7] = Piece(coordinate(A,7), Black, Rook);
  boardArray[H][7] = Piece(coordinate(H,7), Black, Rook);
  boardArray[B][7] = Piece(coordinate(C,7), Black, Bishop);
  boardArray[G][7] = Piece(coordinate(F,7), Black, Bishop);
  boardArray[C][7] = Piece(coordinate(C,7), Black, Knight);
  boardArray[F][7] = Piece(coordinate(B,7), Black, Knight);
  boardArray[D][7] = Piece(coordinate(G,7), Black, King);
  boardArray[E][7] = Piece(coordinate(E,7), Black, Queen);

  for (int c = A; c < BOARD_LENGTH; c++) {
    boardArray[c][1] = Piece(coordinate(c,1), White, Pawn);
    boardArray[c][6] = Piece(coordinate(c,6), Black, Pawn);
    for (int r = 2; r < BOARD_LENGTH-2; r++) {
      boardArray[c][r] = Piece(coordinate(c,r)); // Empty
    }
  }
  ss << "piece at G,6: " << boardArray[G][6].pretty_string() << std::endl;
  ss << "piece at A,5: " << boardArray[A][5].pretty_string() << std::endl;
  ss << "piece at C,7: " << boardArray[C][7].pretty_string() << std::endl;
  DEBUG(ss.str());

  whiteKing = &boardArray[D][0];
  blackKing = &boardArray[E][7];

  state = Turn;
  winner = None;
  whiteKingMoved = false;
  blackKingMoved = false;
  blackKingMoved = false;
  whiteARookMoved = false;
  whiteHRookMoved = false;
  blackARookMoved = false;
  blackHRookMoved = false;

}

Color ChessGame::playerTurn() { return White; }

  #define square_length 16
#define square_height 4
#define board_length = 16 * 8
std::string ChessGame::pretty_string(Color orientation) {
  std::stringstream ss;
  std::stringstream tmpSS;
  if (orientation != Black) { orientation = White; }
  Color firstSqColor = orientation;
  Color curSquare = firstSqColor;
  for (int r = 0; r < BOARD_LENGTH; r++) {
    int rowIndex = (playerTurn() == Black) ? r : BOARD_LENGTH - r - 1;
    // Top Boarder
    ss << "   " << std::string(square_length*BOARD_LENGTH+1, '-') << std::endl;
    // Top
    tmpSS << "   "; // left boarder
    for (int c = A; c < BOARD_LENGTH; c++) {
      tmpSS << "|" << std::string(15, (curSquare == White) ? ' ' : '.');
      curSquare = (curSquare == White) ? Black : White;
    }
    tmpSS << "|" << std::endl;
    ss << tmpSS.str() << tmpSS.str();
    tmpSS.str("");
    tmpSS.clear();
    // Middle (state)
    ss << " " << rowIndex << " ";
    for (int c = A; c < BOARD_LENGTH; c++) {
      char fill =  (curSquare == White) ? ' ' : '.';
      if (boardArray[c][r].isEmpty()) {
        ss << "|" << std::string(15, fill);
      } else {
        ss << "|" << std::string(4, fill) << boardArray[(Collumn)c][rowIndex].pretty_string() << std::string(4, fill);
      }
      curSquare = (curSquare == White) ? Black : White;
    }
    ss << "|" << std::endl;
    // Bottom
    tmpSS << "   "; // left boarder
    for (int c = A; c < BOARD_LENGTH; c++) {
      tmpSS << "|" << std::string(15, (curSquare == White) ? ' ' : '.');
      curSquare = (curSquare == White) ? Black : White;
    }
    tmpSS << "|" << std::endl;
    ss << tmpSS.str() << tmpSS.str();
    tmpSS.str("");
    tmpSS.clear();
    // End of Row, switch color
    curSquare = (curSquare == White) ? Black : White;
  }
  // Bottom Boarder
  ss << "   " << std::string(square_length*BOARD_LENGTH+1, '-') << std::endl;
  ss << "   "; // left boarder
  for (int c = A; c < BOARD_LENGTH; c++) {
    ss << std::string(8, ' ') << (Collumn) c << std::string(7, ' ');
  }

  return ss.str();
}

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
  case Pawn: moves = possible_pawn_moves(pos); break;
  case Bishop: moves = possible_bishop_moves(pos); break;
  case Knight: moves = possible_knight_moves(pos); break;
  case Rook: moves = possible_rook_moves(pos); break;
  case Queen: moves = possible_queen_moves(pos); break;
  case King: moves = possible_king_moves(pos); break;
  default:  return Error::bad_state;
  }
  if (std::find(moves.begin(), moves.end(), dest) != moves.end()) {
    return Error::valid;
  } else {
    return Error::invalid_move;
  }
}

Piece &ChessGame::selectSpace(coordinate coord) {
  assert(coord.isValid());
  return boardArray[coord.collumn][coord.row];
}

bool ChessGame::validEmptySpace(coordinate coord) {
  if (coord.isValid()) {
    return selectSpace(coord).isEmpty();
  }
  return false;
}

bool ChessGame::validOccupiedSpaceByColor(coordinate coord, Color color) {
  if (!coord.isValid()) {
    return false;
  }
  return (!selectSpace(coord).isEmpty() && selectSpace(coord).color == color) ;
}

std::vector<coordinate> ChessGame::possibleMoves(coordinate pos) {
  auto piece = selectSpace(pos);
  switch (piece.type) {
  case Pawn: return possible_pawn_moves(pos);
  case Bishop: return possible_bishop_moves(pos);
  case Knight: return possible_knight_moves(pos);
  case Rook: return possible_rook_moves(pos);
  case Queen: return possible_queen_moves(pos);
  case King: return possible_king_moves(pos);
  default: {
    //TRACE("no piece at position")
    return std::vector<coordinate>();
  }
  }
}

std::vector<coordinate> ChessGame::possible_pawn_moves(coordinate pos) {
  // get color and direction
  Piece &piece = boardArray[pos.collumn][pos.row];
  std::vector<coordinate> retMoves;
  if (piece.type != Pawn) {
    return retMoves;
  }
  bool up = piece.color == White ? 1 : -1;
  Color opponent = piece.color == White ? Black : White;
  assert(piece.type == Pawn);
  auto moveCoord = pos.incRow(up);
  // collumn move
  if (validEmptySpace(moveCoord)) {
    retMoves.push_back(moveCoord);
    moveCoord = moveCoord.incRow(up);
    if (validEmptySpace(moveCoord)) {
      retMoves.push_back(moveCoord);
    }
  }
  // check diagonal attack
  moveCoord = pos.incDiag(up, false);
  if (validOccupiedSpaceByColor(moveCoord, opponent)) {
    retMoves.push_back(moveCoord);
  }
  moveCoord = pos.incDiag(up, true);
  if (validOccupiedSpaceByColor(moveCoord, opponent)) {
    retMoves.push_back(moveCoord);
  }
  return retMoves;
}
std::vector<coordinate> ChessGame::possible_king_moves(coordinate pos) {
    // get color and direction
  Piece &piece = boardArray[pos.collumn][pos.row];
  bool checkCastle;
  if (piece.color == White) {
    checkCastle = !whiteKingMoved && (!whiteARookMoved || !whiteHRookMoved);
  }
  std::vector<coordinate> retMoves;
  if (piece.type != King) {
    return retMoves;
  }
  Color opponent = piece.color == White ? Black : White;
  auto moveCoord = pos.incDiag(UP, LEFT);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) {
    retMoves.push_back(moveCoord);
  }
  moveCoord = pos.incDiag(UP, RIGHT);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) {
    retMoves.push_back(moveCoord);
  }
  moveCoord = pos.incDiag(DOWN, LEFT);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) {
    retMoves.push_back(moveCoord);
  }
  moveCoord = pos.incDiag(DOWN, RIGHT);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) {
    retMoves.push_back(moveCoord);
  }
  moveCoord = pos.incCol(LEFT);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) {
    bool canCastleLeft = false;
    retMoves.push_back(moveCoord);
    if (checkCastle && !(piece.color == White ? whiteARookMoved : blackARookMoved)) {
      canCastleLeft = true;
      auto castleCoord = moveCoord.incCol(LEFT);
      while (castleCoord.collumn != A) {
        if (!validEmptySpace(castleCoord)) {
          canCastleLeft = false;
          break;
        }
        castleCoord = castleCoord.incCol(LEFT);
      }
    }
    if (canCastleLeft) {
      retMoves.push_back(moveCoord.incRow(LEFT)); // increment one more to the left
    }
  }
  moveCoord = pos.incCol(RIGHT);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) {
    bool canCastleRight = false;
    retMoves.push_back(moveCoord);
    if (checkCastle && !(piece.color == White ? whiteARookMoved : blackARookMoved)) {
      canCastleRight = true;
      auto castleCoord = moveCoord.incCol(RIGHT);
      while (castleCoord.collumn != A) {
        if (!validEmptySpace(castleCoord)) {
          canCastleRight = false;
          break;
        }
        castleCoord = castleCoord.incCol(RIGHT);
      }
    }
    if (canCastleRight) {
      retMoves.push_back(moveCoord.incRow(RIGHT)); // increment one more to the left
    }
  }
  moveCoord = pos.incRow(UP);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) {
    retMoves.push_back(moveCoord);
  }
  moveCoord = pos.incRow(DOWN);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) {
    retMoves.push_back(moveCoord);
  }
  return retMoves;
}
std::vector<coordinate> ChessGame::possible_queen_moves(coordinate pos) {
    // get color and direction
  Piece &piece = boardArray[pos.collumn][pos.row];
  std::vector<coordinate> retMoves;
  if (piece.type != Queen) {
    return retMoves;
  }
  Color opponent = piece.color == White ? Black : White;
  get_possible_moves_in_direction(pos, UP, RIGHT, opponent, retMoves);
  get_possible_moves_in_direction(pos, DOWN, RIGHT, opponent, retMoves);
  get_possible_moves_in_direction(pos, UP, LEFT, opponent, retMoves);
  get_possible_moves_in_direction(pos, DOWN, LEFT, opponent, retMoves);
  get_possible_moves_in_direction(pos, UP, HOLD, opponent, retMoves);
  get_possible_moves_in_direction(pos, DOWN, HOLD, opponent, retMoves);
  get_possible_moves_in_direction(pos, HOLD, RIGHT, opponent, retMoves);
  get_possible_moves_in_direction(pos, HOLD, LEFT, opponent, retMoves);
  return retMoves;
}

std::vector<coordinate> ChessGame::possible_rook_moves(coordinate pos) {
    // get color and direction
  Piece &piece = boardArray[pos.collumn][pos.row];
  std::vector<coordinate> retMoves;
  if (piece.type != Rook) {
    return retMoves;
  }
  Color opponent = piece.color == White ? Black : White;
  get_possible_moves_in_direction(pos, UP, HOLD, opponent, retMoves);
  get_possible_moves_in_direction(pos, DOWN, HOLD, opponent, retMoves);
  get_possible_moves_in_direction(pos, HOLD, RIGHT, opponent, retMoves);
  get_possible_moves_in_direction(pos, HOLD, LEFT, opponent, retMoves);
  return retMoves;
}

std::vector<coordinate> ChessGame::possible_bishop_moves(coordinate pos){
  // get color and direction
  Piece &piece = boardArray[pos.collumn][pos.row];
  std::vector<coordinate> retMoves;
  if (piece.type != Bishop) {
    return retMoves;
  }
  Color opponent = piece.color == White ? Black : White;
  get_possible_moves_in_direction(pos, UP, RIGHT, opponent, retMoves);
  get_possible_moves_in_direction(pos, DOWN, RIGHT, opponent, retMoves);
  get_possible_moves_in_direction(pos, UP, LEFT, opponent, retMoves);
  get_possible_moves_in_direction(pos, DOWN, LEFT, opponent, retMoves);
  return retMoves;
}

std::vector<coordinate> ChessGame::possible_knight_moves(coordinate pos) {
  // get color and direction
  Piece &piece = boardArray[pos.collumn][pos.row];
  std::vector<coordinate> retMoves;
  if (piece.type != Knight) {
    return retMoves;
  }
  Color opponent = piece.color == White ? Black : White;
  auto moveCoord = pos.inc(UP*2, LEFT);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) { retMoves.push_back(moveCoord); }
  moveCoord = pos.inc(UP*2, RIGHT);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) { retMoves.push_back(moveCoord); }
  moveCoord = pos.inc(DOWN*2, LEFT);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) { retMoves.push_back(moveCoord); }
  moveCoord = pos.inc(DOWN*2, RIGHT);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) { retMoves.push_back(moveCoord); }
  moveCoord = pos.inc(UP, LEFT*2);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) { retMoves.push_back(moveCoord); }
  moveCoord = pos.inc(UP, RIGHT*2);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) { retMoves.push_back(moveCoord); }
  moveCoord = pos.inc(DOWN, LEFT*2);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) { retMoves.push_back(moveCoord); }
  moveCoord = pos.inc(DOWN, RIGHT*2);
  if (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) { retMoves.push_back(moveCoord); }
  return retMoves;
}

void ChessGame::get_possible_moves_in_direction(coordinate pos, int colUp, int rowLeft, Color opponent, std::vector<coordinate> &retMoves) {
  auto moveCoord = pos.inc(colUp, rowLeft);
  while (validEmptySpace(moveCoord) || validOccupiedSpaceByColor(moveCoord, opponent)) {
    retMoves.push_back(moveCoord);
    moveCoord = moveCoord.inc(colUp,rowLeft);
  }
  if (validOccupiedSpaceByColor(moveCoord,opponent)) {
    retMoves.push_back(moveCoord);
  }
}
