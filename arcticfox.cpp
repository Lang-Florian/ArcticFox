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
      std::cout << ENGINE_NAME << " v" << VERSION << " by " << AUTHOR << "\n";
      return 0;
    } else if (arg == "-h" || arg == "--help") {
      std::cout << "Usage: " << argv[0] << " [OPTION]\n";
      std::cout << "  -v --version\tprint version and exit\n";
      std::cout << "  -h --help   \tprint help and exit\n";
      return 0;
    } else {
      std::cout << "Unknown option: " << arg << "\n";
      return 1;
    }
  }
  loop();
  return 0;
};