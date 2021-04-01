cmake_minimum_required(VERSION 3.17)
project(keshe3_18)

set(CMAKE_CXX_STANDARD 14)

add_executable(keshe main.cpp pre_process.cpp pre_process.h lexer.cpp lexer.h parsing.cpp parsing.h Traverse.cpp Traverse.h AST.cpp AST.h Queue.cpp Queue.h Stack.cpp Stack.h print_file.cpp print_file.h)