#include "ChessGame.h"
#include "Chess_generated.h"

//#include <__format/format_functions.h>
#include <_types/_uint8_t.h>
#include <ios>
#include <memory>
#include <ostream>
#include <string>
#include <sys/_types/_int32_t.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace ZocoloChess;
using std::to_string;

// TODO: add this to a utils file
void log(LogLevel level, std::string out) {
  if (log_level >= level) {
    std::cout << out;
  }
};


std::ostream& operator<< (std::ostream& os, const Collumn &col)
{
  switch (col) {
    case(A): os << "A"; break;
    case(B): os << "B"; break;
    case(C): os << "C";  break;
    case(D): os << "D";  break;
    case(E): os << "E";  break;
    case(F): os << "F";  break;
    case(G): os << "G";  break;
    case(H): os << "H";  break;
    case(X): os << "X";  break;
  };
  return os;
};

std::ostream& operator<< (std::ostream& os, const Color &co)
{
  switch (co) {
    case(White): os << "White"; break;
    case(Black): os << "Black"; break;
    case(All): os << "None";  break;
  };
  return os;
};


std::ostream& operator<<(std::ostream& os, const coordinate &c) {
  return os << c.collumn << (int)c.row;
}

std::string to_string(const Collumn &col) {
  std::stringstream ss;
  ss << col;
  return ss.str();
}

std::string to_string(const Color &co) {
  std::stringstream ss;
  ss << co;
  return ss.str();
}

std::string to_string(const coordinate& c) {
  std::stringstream ss;
  ss << c;
  return ss.str();
}

namespace flatbuffers {
  Serializer::Coord Pack(const coordinate& obj) {
    return Serializer::Coord(obj.collumn, obj.row);
  }

  const coordinate UnPack(const Serializer::Coord& obj) {
    return coordinate(obj.column(), obj.row());
  }
}


void readBoard(char *buffer_ptr) {
  auto board = Serializer::GetChessBoard(buffer_ptr);
  auto black_bishops = board->black_bishops();
  auto black_rooks = board->black_rooks();
  auto moves = board->moves();
  auto white_pawns = board->white_pawns();
  /*std::string out = std::format("white pawns size:{} ", white_pawns->size());
  for (int i = 0; i < white_pawns->size(); i++) {
    auto pawn_ptr = white_pawns->Get(i);
    out += std::format("p{}:{}{},", i, pawn_ptr->column(), pawn_ptr->row());
  }
  std::cout << out << std::endl;
  out = std::format("black rooks size:{} ", black_rooks->size());
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
  out = std::format("moves size:{}, ", moves->size());
  for (int i = 0; i < moves->size(); i++) {
    auto move_ptr = moves->Get(i);
    out += std::format("{}-->{}{}, ", (uint8_t)move_ptr->type(), move_ptr->move().column(), move_ptr->move().row());
  }
  std::cout << out << std::endl;
  out = std::format("moves size:{}, ", moves->size());
  for (int i = 0; i < moves->size(); i++) {
    auto move_ptr = moves->Get(i);
    out += std::format("{}-->{}{}, ", (uint8_t)move_ptr->type(), move_ptr->move().column(), move_ptr->move().row());
  }

  std::cout << out << std::endl;
  auto black_kings1 = board->black_kings();
  out = std::format("black kings (basic serialization) size:{} ", white_pawns->size());
  for (int i = 0; i < black_kings1->size(); i++) {
    auto king_ptr = black_kings1->Get(i);
    out += std::format("K{}:{}{},", i, king_ptr->column(), king_ptr->row());
  }
*/
  auto board2 = Serializer::UnPackChessBoard(buffer_ptr);
  auto &black_kings = board2->black_kings;
  //std::cout << std::format("black kings (object api) size:{} ", black_kings.size());
  for (int i = 0; i < black_kings.size(); i++) {
    coordinate black_king = black_kings.at(i);
    std::cout << "K:" << black_kings.at(i);
    std::cout << "   K:" << to_string(black_king);
        // TODO: think about adding a std::to_string overload for my classes
    //out += std::format("k:{}", black_kings.at(i));
  }

  std::cout << std::endl;
  for (int i = 0 ; i < board2->white_bishops.size(); i++) {
    std::cout << "  White B:" << board2->white_bishops.at(i);
  }
  std::cout << std::endl;
  std::cout << std::endl << "white Pawns:";
  for (int i = 0 ; i < board2->white_pawns.size(); i++) { std::cout << "  " << board2->white_pawns.at(i); }
  std::cout << std::endl << "black Pawns:";
  for (int i = 0 ; i < board2->black_pawns.size(); i++) { std::cout << "  " << board2->black_pawns.at(i); }
  std::cout << std::endl << "white bishops:";
  for (int i = 0 ; i < board2->white_bishops.size(); i++) { std::cout << "  " << board2->white_bishops.at(i); }
  std::cout << std::endl << "black bishops:";
  for (int i = 0 ; i < board2->black_bishops.size(); i++) { std::cout << "  " << board2->black_bishops.at(i); }
  std::cout << std::endl << "white knights";
  for (int i = 0 ; i < board2->white_knights.size(); i++) { std::cout << "  " << board2->white_knights.at(i); }
  std::cout << std::endl << "black knights";
  for (int i = 0 ; i < board2->black_knights.size(); i++) { std::cout << "  " << board2->black_knights.at(i); }
  std::cout << std::endl << "white rooks";
  for (int i = 0 ; i < board2->white_rooks.size(); i++) { std::cout << "  " << board2->white_rooks.at(i); }
  std::cout << std::endl << "black rooks";
  for (int i = 0 ; i < board2->black_rooks.size(); i++) { std::cout << "  " << board2->black_rooks.at(i); }
  std::cout << std::endl << "white queens";
  for (int i = 0 ; i < board2->white_queens.size(); i++) { std::cout << "  " << board2->white_queens.at(i); }
  std::cout << std::endl << "black queens";
  for (int i = 0 ; i < board2->black_queens.size(); i++) { std::cout << "  " << board2->black_queens.at(i); }
  std::cout << std::endl << "white kings";
  for (int i = 0 ; i < board2->white_kings.size(); i++) { std::cout << "  " << board2->white_kings.at(i); }
  std::cout << std::endl << "black kings";
  for (int i = 0 ; i < board2->black_kings.size(); i++) { std::cout << "  " << board2->black_kings.at(i); }
}

std::shared_ptr<char> CreateNewChessGameData(flatbuffers::FlatBufferBuilder64 &builder) {
  // black bishops
  Serializer::Coord black_bishops_ar[] = { Serializer::Coord(Collumn::C, 7), Serializer::Coord(Collumn::F, 7) };
  auto black_bishops = builder.CreateVectorOfStructs(black_bishops_ar, 2);
  // black knights
  Serializer::Coord black_knights_ar[] = { Serializer::Coord(Collumn::B, 7), Serializer::Coord(Collumn::G, 7) };
  auto black_knights = builder.CreateVectorOfStructs(black_knights_ar, 2);
  // black rooks
  Serializer::Coord black_rooks_ar[] = { Serializer::Coord(Collumn::A, 7), Serializer::Coord(Collumn::H, 7) };
  auto black_rooks = builder.CreateVectorOfStructs(black_rooks_ar, 2);
  // black king
  Serializer::Coord black_kings_ar[] = { Serializer::Coord(Collumn::D, 7)};
  auto black_kings = builder.CreateVectorOfStructs(black_kings_ar, 1);
  // black queens
  Serializer::Coord black_queens_ar[] = { Serializer::Coord(Collumn::E, 7)};
  auto black_queens = builder.CreateVectorOfStructs(black_queens_ar, 1);
  // black pawns
  Serializer::Coord black_pawns_ar[] = { Serializer::Coord(Collumn::A, 6), Serializer::Coord(Collumn::B, 6), Serializer::Coord(Collumn::C, 6), Serializer::Coord(Collumn::D, 6), Serializer::Coord(Collumn::E, 6), Serializer::Coord(Collumn::F, 6), Serializer::Coord(Collumn::G, 6), Serializer::Coord(Collumn::H, 6) };
  auto black_pawns = builder.CreateVectorOfStructs(black_pawns_ar, 8);
  // white bishops
  Serializer::Coord white_bishops_ar[] = { Serializer::Coord(Collumn::C, 0), Serializer::Coord(Collumn::F, 0) };
  auto white_bishops = builder.CreateVectorOfStructs(white_bishops_ar, 2);
  // white knights
  Serializer::Coord white_knights_ar[] = { Serializer::Coord(Collumn::B, 0), Serializer::Coord(Collumn::G, 0) };
  auto white_knights = builder.CreateVectorOfStructs(white_knights_ar, 2);
  // white rooks
  Serializer::Coord white_rooks_ar[] = { Serializer::Coord(Collumn::A, 0), Serializer::Coord(Collumn::H, 0) };
  auto white_rooks = builder.CreateVectorOfStructs(white_rooks_ar, 2);
  // white king
  Serializer::Coord white_kings_ar[] = { Serializer::Coord(Collumn::E, 0)};
  auto white_kings = builder.CreateVectorOfStructs(white_kings_ar, 0);
  // white queens
  Serializer::Coord white_queens_ar[] = { Serializer::Coord(Collumn::D, 0)};
  auto white_queens = builder.CreateVectorOfStructs(white_queens_ar, 0);
  // white pawns
  Serializer::Coord white_pawns_ar[] = { Serializer::Coord(Collumn::A, 1), Serializer::Coord(Collumn::B, 1), Serializer::Coord(Collumn::C, 1), Serializer::Coord(Collumn::D, 1), Serializer::Coord(Collumn::E, 1), Serializer::Coord(Collumn::F, 1), Serializer::Coord(Collumn::G, 1), Serializer::Coord(Collumn::H, 1) };
  auto white_pawns = builder.CreateVectorOfStructs(white_pawns_ar, 8);
  // moves
  Serializer::Move moves_ar[] = {};
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
  return std::make_shared<char>((char*)builder.GetBufferPointer());
}

int main() {
   // Test reading
   flatbuffers::FlatBufferBuilder builder(1024);
   std::shared_ptr<char> buffer_ptr = CreateNewChessGameData(&builder);
  readBoard(buffer_ptr.get());

  // TODO: test a few moves

  // Test Serialize
  std::ofstream outfile;
  outfile.open("chessboard_test.zo", std::ios_base::binary | std::ios_base::out ); // clears contents first
  outfile.write(buffer_ptr.get(), builder.GetSize());
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
