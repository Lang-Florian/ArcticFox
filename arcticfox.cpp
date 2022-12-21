#include <iostream>
#include "uci.cpp"

/***********************************************************************
 * 
 * Main file.
 * 
 * Parses command line arguments and starts the UCI loop.
 * 
 * Usage: ./arcticfox [options]
 * 
 * Options:
 * 
 *  -h, --help
 *    Print this help message.
 *  -v, --version
 *    Print version information.
 * 
***********************************************************************/

int main(int argc, char *argv[]) {
  // parse command line arguments if any
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-v" || arg == "--version") {
      std::cout << ENGINE_NAME << " v" << VERSION << " by " << AUTHOR << "\n";
      return 0;
    } else if (arg == "-h" || arg == "--help") {
      std::cout << "Usage: " << argv[0] << " [options]\n";
      std::cout << "  -h --help   \n\tPrint this help message.\n";
      std::cout << "  -v --version\n\tPrint version information.\n";
      return 0;
    } else {
      std::cout << "Unknown option: " << arg << "\n";
      return 1;
    }
  }
  // start uci loop
  uci_loop();
  return 0;
};