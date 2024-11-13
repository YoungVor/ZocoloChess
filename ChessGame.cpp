#include "ChessGame.h"
#include "Chess_generated.h"
#include <__format/format_functions.h>
#include <_types/_uint8_t.h>
#include <memory>
#include <string>
#include <sys/_types/_int32_t.h>
#include <iostream>

using namespace Zocolo::Chess;


void readBoard(uint8_t *buffer_ptr) {
  auto board = GetChessBoard(buffer_ptr);
  auto black_bishops = board->black_bishops();
  auto moves = board->moves();
  auto white_pawns = board->white_pawns();
  std::string out = std::format("white pawns size:{} ", white_pawns->size());
  for (int i = 0; i < white_pawns->size(); i++) {
    auto pawn_ptr = white_pawns->Get(i);
    out += std::format("p{}:{}{},", i, pawn_ptr->column(), pawn_ptr->row());
  }
  std::cout << out << std::endl;
  out = std::format("white pawns size:{} ", white_pawns->size());
  auto black_rooks = board->black_rooks();
  for (int i = 0; i < black_rooks->size(); i++) {
    auto rook_ptr = black_rooks->Get(i);
    out += std::format("p{}:{}{},", i, rook_ptr->column(), rook_ptr->row());
  }

  std::cout << out << std::endl;
  out = std::format("moves size:{}, ", moves->size());
  for (int i = 0; i < moves->size(); i++) {
    auto move_ptr = moves->Get(i);
    out += std::format("{}-->{}{}, ", (uint8_t)move_ptr->type(), move_ptr->move().column(), move_ptr->move().row());
  }
  std::cout << out << std::endl;
  std::cout << std::format("black king:{}{}", board->black_king()->column(), board->black_king()->row()) << std::endl;
}

int main() {
  flatbuffers::FlatBufferBuilder builder(1024);
  // black bishops
  Coord black_bishops_ar[] = { Coord(Collumn::C, 8), Coord(Collumn::F, 8) };
  auto black_bishops = builder.CreateVectorOfStructs(black_bishops_ar, 2);
  // black knights
  Coord black_knights_ar[] = { Coord(Collumn::B, 8), Coord(Collumn::G, 8) };
  auto black_knights = builder.CreateVectorOfStructs(black_knights_ar, 2);
  // black rooks
  Coord black_rooks_ar[] = { Coord(Collumn::A, 8), Coord(Collumn::H, 8) };
  auto black_rooks = builder.CreateVectorOfStructs(black_rooks_ar, 2);
  // black king
  auto black_king = Coord(Collumn::D, 8);
  // black queens
  Coord black_queens_ar[] = { Coord(Collumn::E, 8)};
  auto black_queens = builder.CreateVectorOfStructs(black_queens_ar, 1);
  // black pawns
  Coord black_pawns_ar[] = { Coord(Collumn::A, 7), Coord(Collumn::B, 7), Coord(Collumn::C, 7), Coord(Collumn::D, 7), Coord(Collumn::E, 7), Coord(Collumn::F, 7), Coord(Collumn::G, 7), Coord(Collumn::H, 7) };
  auto black_pawns = builder.CreateVectorOfStructs(black_pawns_ar, 8);
  // white bishops
  Coord white_bishops_ar[] = { Coord(Collumn::C, 1), Coord(Collumn::F, 1) };
  auto white_bishops = builder.CreateVectorOfStructs(white_bishops_ar, 2);
  // white knights
  Coord white_knights_ar[] = { Coord(Collumn::B, 1), Coord(Collumn::G, 1) };
  auto white_knights = builder.CreateVectorOfStructs(white_knights_ar, 2);
  // white rooks
  Coord white_rooks_ar[] = { Coord(Collumn::A, 1), Coord(Collumn::H, 1) };
  auto white_rooks = builder.CreateVectorOfStructs(white_rooks_ar, 2);
  // white king
  auto white_king = Coord(Collumn::E, 1);
  // white queens
  Coord white_queens_ar[] = { Coord(Collumn::D, 1)};
  auto white_queens = builder.CreateVectorOfStructs(white_queens_ar, 1);
  // white pawns
  Coord white_pawns_ar[] = { Coord(Collumn::A, 2), Coord(Collumn::B, 2), Coord(Collumn::C, 2), Coord(Collumn::D, 2), Coord(Collumn::E, 2), Coord(Collumn::F, 2), Coord(Collumn::G, 2), Coord(Collumn::H, 2) };
  auto white_pawns = builder.CreateVectorOfStructs(white_pawns_ar, 8);
  // moves
  Move moves_ar[] = {};
  auto moves = builder.CreateVectorOfStructs(moves_ar, 0);
  // Serialize the board
  auto board = CreateChessBoard(builder,
                                white_pawns,
                                black_pawns,
                                white_bishops,
                                black_bishops,
                                white_knights,
                                black_knights,
                                white_rooks,
                                black_rooks,
                                white_queens,
                                black_queens,
                                &white_king,
                                &black_king,
                                moves);
  builder.Finish(board);
  std::cout << "finished serializing board" << std::endl;

  // Test deserialization
  uint8_t *buffer_ptr = builder.GetBufferPointer();
  readBoard(buffer_ptr);
}
