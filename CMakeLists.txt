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

set(SOURCES
utils.h
utils.cpp
ClientInterface.h
FileClientInterface.h
FileClientInterface.cpp
ChessUI.h
ChessUI.cpp
ChessGame.h
ChessGame.cpp
)

#add_executable(chessSerializeTest SerializeTest.cpp ${SOURCES})
add_executable(chessTest ChessUITest.cpp ${SOURCES})
#add_executable(testFlat test/native_type_test_impl.h test/native_type_test_impl.cpp)

target_link_libraries(chessTest PRIVATE flatbuffers::flatbuffers)
#target_link_libraries(chessSerializeTest PRIVATE flatbuffers::flatbuffers)
#target_link_libraries(testFlat PRIVATE flatbuffers::flatbuffers)
