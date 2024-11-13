#include "ChessGame.h"
#include "Chess_generated.h"

#include <__format/format_functions.h>
#include <_types/_uint8_t.h>
#include <ios>
#include <memory>
#include <ostream>
#include <string>
#include <sys/_types/_int32_t.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace ChessGame;

std::ostream& operator<< (std::ostream& os, const Collumn &col)
{
  switch (col) {
    case(ChessGame::A): os << "A ";
    case(ChessGame::B): os << "B";
    case(ChessGame::C): os << "C";
    case(ChessGame::D): os << "D";
    case(ChessGame::E): os << "E";
    case(ChessGame::F): os << "F";
    case(ChessGame::G): os << "G";
    case(ChessGame::H): os << "H";
    case(ChessGame::X): os << "X";
  };
  return os;
};

std::ostream& operator<<(std::ostream& os, const coordinate &c) {
  return os << c.collumn << c.row;
}

std::string to_string(const ChessGame::Collumn &col) {
  std::stringstream ss;
  ss << col;
  return ss.str();
}

std::string to_string(const ChessGame::coordinate& c) {
  std::stringstream ss;
  ss << c;
  return ss.str();
}

namespace flatbuffers {
  ZocoloChess::Coord Pack(const ChessGame::coordinate& obj) {
    return ZocoloChess::Coord(obj.row, obj.collumn);
  }

  const ChessGame::coordinate UnPack(const ZocoloChess::Coord& obj) {
    return ChessGame::coordinate(obj.row(), obj.column());
  }
}


void readBoard(char *buffer_ptr) {
  auto board = ZocoloChess::GetChessBoard(buffer_ptr);
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

  auto board2 = ZocoloChess::UnPackChessBoard(buffer_ptr);
  auto &black_kings = board2->black_kings;
  std::cout << out << std::endl;
  std::cout << std::format("black kings size:{}", black_kings.size());
  for (int i = 0; i < black_kings.size(); i++) {
    ChessGame::coordinate black_king = black_kings.at(i);
    std::cout << "K:" << black_kings.at(i);
    // TODO: think about adding a std::to_string overload for my classes
    //out += std::format("k:{}", black_kings.at(i));
  }
}

int main() {
  flatbuffers::FlatBufferBuilder builder(1024);
  // black bishops
  ZocoloChess::Coord black_bishops_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::C, 7), ZocoloChess::Coord(ChessGame::Collumn::F, 7) };
  auto black_bishops = builder.CreateVectorOfStructs(black_bishops_ar, 2);
  // black knights
  ZocoloChess::Coord black_knights_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::B, 7), ZocoloChess::Coord(ChessGame::Collumn::G, 7) };
  auto black_knights = builder.CreateVectorOfStructs(black_knights_ar, 2);
  // black rooks
  ZocoloChess::Coord black_rooks_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::A, 7), ZocoloChess::Coord(ChessGame::Collumn::H, 7) };
  auto black_rooks = builder.CreateVectorOfStructs(black_rooks_ar, 2);
  // black king
  ZocoloChess::Coord black_kings_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::D, 7)};
  auto black_kings = builder.CreateVectorOfStructs(black_kings_ar, 1);
  // black queens
  ZocoloChess::Coord black_queens_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::E, 7)};
  auto black_queens = builder.CreateVectorOfStructs(black_queens_ar, 1);
  // black pawns
  ZocoloChess::Coord black_pawns_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::A, 6), ZocoloChess::Coord(ChessGame::Collumn::B, 6), ZocoloChess::Coord(ChessGame::Collumn::C, 6), ZocoloChess::Coord(ChessGame::Collumn::D, 6), ZocoloChess::Coord(ChessGame::Collumn::E, 6), ZocoloChess::Coord(ChessGame::Collumn::F, 6), ZocoloChess::Coord(ChessGame::Collumn::G, 6), ZocoloChess::Coord(ChessGame::Collumn::H, 6) };
  auto black_pawns = builder.CreateVectorOfStructs(black_pawns_ar, 8);
  // white bishops
  ZocoloChess::Coord white_bishops_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::C, 0), ZocoloChess::Coord(ChessGame::Collumn::F, 0) };
  auto white_bishops = builder.CreateVectorOfStructs(white_bishops_ar, 2);
  // white knights
  ZocoloChess::Coord white_knights_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::B, 0), ZocoloChess::Coord(ChessGame::Collumn::G, 0) };
  auto white_knights = builder.CreateVectorOfStructs(white_knights_ar, 2);
  // white rooks
  ZocoloChess::Coord white_rooks_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::A, 0), ZocoloChess::Coord(ChessGame::Collumn::H, 0) };
  auto white_rooks = builder.CreateVectorOfStructs(white_rooks_ar, 2);
  // white king
  ZocoloChess::Coord white_kings_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::E, 0)};
  auto white_kings = builder.CreateVectorOfStructs(white_kings_ar, 0);
  // white queens
  ZocoloChess::Coord white_queens_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::D, 0)};
  auto white_queens = builder.CreateVectorOfStructs(white_queens_ar, 0);
  // white pawns
  ZocoloChess::Coord white_pawns_ar[] = { ZocoloChess::Coord(ChessGame::Collumn::A, 1), ZocoloChess::Coord(ChessGame::Collumn::B, 1), ZocoloChess::Coord(ChessGame::Collumn::C, 1), ZocoloChess::Coord(ChessGame::Collumn::D, 1), ZocoloChess::Coord(ChessGame::Collumn::E, 1), ZocoloChess::Coord(ChessGame::Collumn::F, 1), ZocoloChess::Coord(ChessGame::Collumn::G, 1), ZocoloChess::Coord(ChessGame::Collumn::H, 1) };
  auto white_pawns = builder.CreateVectorOfStructs(white_pawns_ar, 8);
  // moves
  ZocoloChess::Move moves_ar[] = {};
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
                                white_kings,
                                black_kings,
                                moves);
  builder.Finish(board);
  std::cout << "finished serializing board" << std::endl;

  // Test reading
  char *buffer_ptr = (char*)builder.GetBufferPointer();
  readBoard(buffer_ptr);

  // TODO: test a few moves

  // Test Serialize
  std::ofstream outfile;
  outfile.open("chessboard_test.zo", std::ios_base::binary | std::ios_base::out ); // clears contents first
  outfile.write(buffer_ptr, builder.GetSize());
  outfile.close();
  std::cout << "Seriazed Chessboard" << std::endl;

  // Test Deserialize
  std::ifstream infile;
  infile.open("chessboard_test.zo", std::ios_base::binary | std::ios_base::in ); // clears contents first
  infile.seekg(0, std::ios_base::end);
  int size = infile.tellg();
  infile.seekg(0, std::ios_base::beg);
  char *read_buffer = new char[size];
  infile.read(read_buffer, size);
  std::cout << "Deseriazed Chessboard" << std::endl;
  readBoard(read_buffer);

}
