#include "ChessGame.h"
#include "Chess_generated.h"
#include "utils.h"
#include <cassert>
#include <cstdlib>
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

ChessGame::ChessGame(GameClientIF *cl) {
  client.reset(cl);
  init_new_board();
}

void ChessGame::load_board(uid_t id) {
  //board_data = Serializer::GetChessBoard(board_data_ptr.get());
}

void ChessGame::init_new_board() {
  DEBUG("init board");
  id = boost::uuids::random_generator()();
  auto piece = Piece(coordinate(A,0), White, Rook);
   std::stringstream ss;
  boardArray[A][0] = piece;
  ss << "piece at A,0: " << boardArray[A][0].pretty_string() << std::endl;
  ss << "assigned at A,0: " << piece.pretty_string() << std::endl;

  // TODO: should be a single authority on the piece position.  maybe remove 'pos' from the Piece
  boardArray[H][0] = Piece(coordinate(H,0), White, Rook);
  boardArray[C][0] = Piece(coordinate(C,0), White, Bishop);
  boardArray[F][0] = Piece(coordinate(F,0), White, Bishop);
  boardArray[B][0] = Piece(coordinate(B,0), White, Knight);
  boardArray[G][0] = Piece(coordinate(G,0), White, Knight);
  boardArray[E][0] = Piece(coordinate(E,0), White, King);
  boardArray[D][0] = Piece(coordinate(D,0), White, Queen);

  boardArray[A][7] = Piece(coordinate(A,7), Black, Rook);
  boardArray[H][7] = Piece(coordinate(H,7), Black, Rook);
  boardArray[C][7] = Piece(coordinate(C,7), Black, Bishop);
  boardArray[F][7] = Piece(coordinate(F,7), Black, Bishop);
  boardArray[B][7] = Piece(coordinate(B,7), Black, Knight);
  boardArray[G][7] = Piece(coordinate(G,7), Black, Knight);
  boardArray[E][7] = Piece(coordinate(E,7), Black, King);
  boardArray[D][7] = Piece(coordinate(D,7), Black, Queen);

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
  whiteKingCanCastleA = true;
  blackKingCanCastleA = true;
  whiteKingCanCastleH = true;
  blackKingCanCastleH = true;
  moveCount = 0;

}

Error ChessGame::write_board_data() {
  flatbuffers::FlatBufferBuilder builder(1024);
  std::vector<Serializer::Coord> white_pawns;
  std::vector<Serializer::Coord> black_pawns;
  std::vector<Serializer::Coord> white_knights;
  std::vector<Serializer::Coord> black_knights;
  std::vector<Serializer::Coord> white_bishops;
  std::vector<Serializer::Coord> black_bishops;
  std::vector<Serializer::Coord> white_rooks;
  std::vector<Serializer::Coord> black_rooks;
  std::vector<Serializer::Coord> white_queens;
  std::vector<Serializer::Coord> black_queens;
  std::vector<Serializer::Coord> white_kings;
  std::vector<Serializer::Coord> black_kings;

  for (int col = 0; col < BOARD_LENGTH; col++) {
    for (int row = 0; row < BOARD_LENGTH; row++) {
      auto &piece = boardArray[col][row];
      if (!piece.isEmpty()) {
        switch (piece.type) {
        case Pawn: {
          if (piece.color == White) {
            white_pawns.push_back(flatbuffers::Pack(piece.position));
          } else {
            black_pawns.push_back(flatbuffers::Pack(piece.position));
          }
          break;
        }
        case Knight: {
          if (piece.color == White) {
            white_knights.push_back(flatbuffers::Pack(piece.position));
          } else {
            black_knights.push_back(flatbuffers::Pack(piece.position));
          }
          break;
        }
        case Bishop: {
          if (piece.color == White) {
            white_bishops.push_back(flatbuffers::Pack(piece.position));
          } else {
            black_bishops.push_back(flatbuffers::Pack(piece.position));
          }
          break;
        }
        case Rook: {
          if (piece.color == White) {
            white_rooks.push_back(flatbuffers::Pack(piece.position));
          } else {
            black_rooks.push_back(flatbuffers::Pack(piece.position));
          }
          break;
        }
        case Queen: {
          if (piece.color == White) {
            white_queens.push_back(flatbuffers::Pack(piece.position));
          } else {
            black_queens.push_back(flatbuffers::Pack(piece.position));
          }
          break;
        }
        case King: {
          if (piece.color == White) {
            white_kings.push_back(flatbuffers::Pack(piece.position));
          } else {
            black_kings.push_back(flatbuffers::Pack(piece.position));
          }
          break;
        }
        default:
          log(INFO, "write_board error: bad piece type" + std::to_string(piece.type));
          return Error::bad_state;
        }
      }
    }
  }
  std::vector<Serializer::Move> moves;
  auto packedID = builder.CreateString(boost::uuids::to_string(id));


  auto board = Serializer::CreateChessBoard(builder,
                                            packedID,
                                            whiteKingCanCastleA,
                                            whiteKingCanCastleH,
                                            blackKingCanCastleA,
                                            blackKingCanCastleH,
                                            moveCount,
                                            builder.CreateVectorOfStructs(white_pawns.data(), white_pawns.size()),
                                            builder.CreateVectorOfStructs(black_pawns.data(), black_pawns.size()),
                                            builder.CreateVectorOfStructs(white_bishops.data(), white_bishops.size()),
                                            builder.CreateVectorOfStructs(black_bishops.data(), black_bishops.size()),
                                            builder.CreateVectorOfStructs(white_knights.data(), white_knights.size()),
                                            builder.CreateVectorOfStructs(black_knights.data(), black_knights.size()),
                                            builder.CreateVectorOfStructs(white_rooks.data(), white_rooks.size()),
                                            builder.CreateVectorOfStructs(black_rooks.data(), black_rooks.size()),
                                            builder.CreateVectorOfStructs(white_queens.data(), white_queens.size()),
                                            builder.CreateVectorOfStructs(black_queens.data(), black_queens.size()),
                                            builder.CreateVectorOfStructs(white_kings.data(), white_kings.size()),
                                            builder.CreateVectorOfStructs(black_kings.data(), black_kings.size()),
                                            builder.CreateVectorOfStructs(moves.data(), moves.size()));
  builder.Finish(board);

  client->write_board(id, (char*)builder.GetBufferPointer(), builder.GetBufferMinAlignment());

  return Error::valid;
}



Color ChessGame::playerTurn() {
  return (moveCount % 2 == 1) ? Black : White;
}

  #define square_length 16
#define square_height 4
#define board_length = 16 * 8
std::string ChessGame::pretty_string(Color orientation) {
  std::stringstream ss;
  std::stringstream tmpSS;
  if (orientation != Black) { orientation = White; }
  Color firstSqColor = White; // bottom square to the right of each player is always white
  Color curSquare = firstSqColor;
  for (int r = 0; r < BOARD_LENGTH; r++) {
    int rowIndex = (orientation == Black) ? r : BOARD_LENGTH - r - 1;
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
      int colIndex = (orientation == White) ? c : BOARD_LENGTH - c - 1;
      char fill =  (curSquare == White) ? ' ' : '.';
      if (boardArray[colIndex][rowIndex].isEmpty()) {
        ss << "|" << std::string(15, fill);
      } else {
        ss << "|" << std::string(4, fill) << boardArray[(Collumn)colIndex][rowIndex].pretty_string() << std::string(4, fill);
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

  Piece &piece = selectSpace(pos);

  // check if piece is taken

  // special case for castling

  if (piece.type == King) {
    if (piece.color == White) {
      whiteKingCanCastleA = false;
      whiteKingCanCastleH = false;
    }
    if (piece.color == Black) {
      blackKingCanCastleA = false;
      blackKingCanCastleH = false;
    }
  }
    if (piece.type == Rook) {
      if (piece.color == White) {
        if (whiteKingCanCastleA && piece.position.collumn == A) {
          whiteKingCanCastleA = false;
        }
      } else {
        if (blackKingCanCastleA && piece.position.collumn == A) {
          blackKingCanCastleA = false;
        }
      }
      if (piece.color == White) {
        if (whiteKingCanCastleH && piece.position.collumn == H) {
          whiteKingCanCastleH = false;
        }
      } else {
        if (blackKingCanCastleH && piece.position.collumn == H) {
          blackKingCanCastleH = false;
        }
      }
  }
  // move piece
  selectSpace(dest) = piece;
  selectSpace(dest).position = dest;
  piece = Piece(pos);

  move_log.push_back(move(pos, dest));
  moveCount++;
  assert(moveCount == move_log.size());

  // update durable state
  write_board_data();
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
  short up = (piece.color == White) ? 1 : -1;
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
    if (piece.color == White ? whiteKingCanCastleA : blackKingCanCastleA) { // check castle
      canCastleLeft = true;
      auto castleCoord = moveCoord;
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
    if (piece.color == White ? whiteKingCanCastleH : blackKingCanCastleH) { // check castle
      canCastleRight = true;
      auto castleCoord = moveCoord;
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
    if (validOccupiedSpaceByColor(moveCoord, opponent)) {
      break; // can't move past opponent's piece
    }
    moveCoord = moveCoord.inc(colUp,rowLeft);
  }
  if (validOccupiedSpaceByColor(moveCoord,opponent)) {
    retMoves.push_back(moveCoord);
  }
}
