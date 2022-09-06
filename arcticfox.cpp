#include <iostream>

#include "uci.cpp"


/*

  -v --version      print version and exit
  -h --help         print help and exit

*/


int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-v" || arg == "--version") {
      std::cout << ENGINE_NAME << " v" << VERSION << " by " << AUTHOR << std::endl;
      return 0;
    } else if (arg == "-h" || arg == "--help") {
      std::cout << "Usage: " << argv[0] << " [OPTION]" << std::endl;
      std::cout << "  -v --version\tprint version and exit" << std::endl;
      std::cout << "  -h --help   \tprint help and exit" << std::endl;
      return 0;
    } else {
      std::cout << "Unknown option: " << arg << std::endl;
      return 1;
    }
  }
  uci();
  return 0;
};