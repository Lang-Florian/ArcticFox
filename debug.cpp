#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include "base.cpp"
#include "board.cpp"
#include "perft.cpp"

/*

  Module to generate debug output.

*/

void print_board(Board& board) {
  std::cout << " +---+---+---+---+---+---+---+---+\n";
  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 0; file < 8; file++) {
      std::cout << " | " << piece_to_string(board.pieces[8 * (7 - rank) + file]);
    };
    std::cout << " | " << rank + 1 << "\n";
    std::cout << " +---+---+---+---+---+---+---+---+\n";
  };
  std::cout << "   a   b   c   d   e   f   g   h\n\n";
  std::cout << "Fen: " << board.fen() << "\n";
};

void perft_test_suite(Board& board, std::string epd_file_path) {
  std::string original_fen = board.fen();
  std::ifstream edp_file(epd_file_path);
  if (!edp_file.is_open()) {
    edp_file.open("test/" + epd_file_path);
    if (!edp_file.is_open()) {
      return;
    };
  };
  std::string line;
  int correct_positions = 0;
  int total_positions = 0;
  float total_time = 0;
  u64_t total_nodes = 0;
  float max_mnps = 0;
  while (std::getline(edp_file, line)) {
    std::istringstream line_stream(line);
    std::string fen;
    std::getline(line_stream, fen, ';');
    board.set_fen(fen);
    std::cout << "\tFen: " << board.fen() << "\n";
    std::string depth_token;
    std::string nodes_token;
    while (line_stream >> depth_token) {
      line_stream >> nodes_token;
      int depth = std::stoi(depth_token.substr(1));
      u64_t nodes = std::stol(nodes_token);
      perft_result_t perft_result;
      perft_result = perft<legal>(board, depth, false);
      if (perft_result.nodes == nodes) {
        std::cout << "       ";
      } else {
        std::cout << "FAILED!";
      };
      std::cout << "\t  Depth:" << depth;
      std::cout << "\tResult: " << perft_result.nodes << "\n";
      correct_positions += perft_result.nodes == nodes;
      total_positions++;
      total_time += perft_result.time;
      total_nodes += perft_result.nodes;
      max_mnps = std::max(max_mnps, perft_result.mnps);
    };
    std::cout << "\n";
  };
  std::cout << "Correct: " << correct_positions << "/" << total_positions << "\n";
  std::cout << "Total time: " << total_time << " s" << "\n";
  std::cout << "Mean MNps: " << (total_nodes / total_time) * 1e-6 << "\n";
  std::cout << "Max MNps: " << max_mnps << "\n";
  board.set_fen(original_fen);
};

void print_bitboard(const bitboard_t& bitboard) {
  std::cout << " +---+---+---+---+---+---+---+---+\n";
  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 0; file < 8; file++) {
      std::cout << " | " << (bitboard & (1ULL << (8 * (7 - rank) + file)) ? "1" : "0");
    };
    std::cout << " | " << rank + 1 << "\n";
    std::cout << " +---+---+---+---+---+---+---+---+\n";
  };
  std::cout << "   a   b   c   d   e   f   g   h\n\n";
};
