cmake_minimum_required(VERSION 3.7)
project(Chess)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Bitboard.cpp Bitboard.h Mask.cpp Mask.h Eval.cpp Eval.h)
add_executable(Chess ${SOURCE_FILES})