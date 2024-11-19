cmake_minimum_required(VERSION 3.29)

project(BoostNetworkingPlayground LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Boost and flatbuffers
find_package(Boost 1.86 REQUIRED COMPONENTS system filesystem)
find_package(flatbuffers 3.30 REQUIRED)
# Don't think i need this.
# PATHS /opt/homebrew/Cellar/flatbuffers/24.3.25/include)

#add_executable(chess ChessGame.cpp)
add_executable(chessSerializeTest ChessGame.h ChessGame.cpp SerializeTest.cpp)
add_executable(chessTest ChessUITest.cpp ChessGame.h ChessUI.h ChessUI.cpp ChessGame.cpp SerializeTest.cpp)
#add_executable(testFlat test/native_type_test_impl.h test/native_type_test_impl.cpp)

#target_link_libraries(chess PRIVATE flatbuffers::flatbuffers)
target_link_libraries(chessTest PRIVATE flatbuffers::flatbuffers)
target_link_libraries(chessSerializeTest PRIVATE flatbuffers::flatbuffers)
#target_link_libraries(testFlat PRIVATE flatbuffers::flatbuffers)
