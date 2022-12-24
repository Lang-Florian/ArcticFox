# pragma once

#include <fstream>
#include <string>
#include "time.cpp"

/***********************************************************************
 * 
 *  Module for logging.
 * 
***********************************************************************/

static std::string _log_file_path = "log.txt";

void setup_log_path(std::string path) {
  _log_file_path = path;
};

template<typename... Args>
void log(Args... args) {
  std::ofstream log_file(_log_file_path, std::ios::app);
  log_file << "[" << timestamp() << " DEBUG]   ";
  bool first = true;
  for (auto arg : {args...}) {
    if (!first)
      log_file << "                              ";
    log_file << arg << "\n";
    first = false;
  };
  log_file.close();
};

template<typename... Args>
void warning(Args... args) {
  std::ofstream log_file(_log_file_path, std::ios::app);
  log_file << "[" << timestamp() << " WARNING] ";
  bool first = true;
  for (auto arg : {args...}) {
    if (!first)
      log_file << "                              ";
    log_file << arg << "\n";
    first = false;
  };
  log_file.close();
};

template<typename... Args>
void error(Args... args) {
  std::ofstream log_file(_log_file_path, std::ios::app);
  log_file << "[" << timestamp() << " ERROR]   ";
  bool first = true;
  for (auto arg : {args...}) {
    if (!first)
      log_file << "                              ";
    log_file << arg << "\n";
    first = false;
  };
  log_file.close();
};