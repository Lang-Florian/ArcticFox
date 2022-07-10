# ChessFox

A chess engine written in C++.

Compile with ```g++ main.cpp -o main -fconstexpr-ops-limit=100000000000 -std=c++23 -Ofast -march=native -flto -fno-signed-zeros -frename-registers -funroll-loops```