# ArcticFox

A chess engine written in C++.

Compile with ```g++ arcticfox.cpp -o bin/arcticfox -std=c++23 -fconstexpr-ops-limit=100000000000 -Ofast -march=native -flto -fno-signed-zeros -funroll-loops -mcmodel=medium -pthread```