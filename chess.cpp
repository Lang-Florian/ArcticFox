#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "attack_rays.cpp"
#include "attacks.cpp"
#include "board.cpp"
#include "constants.cpp"
#include "correct_attacks.cpp"
#include "magic.cpp"
#include "moves.cpp"
#include "random.cpp"
#include "test.cpp"
#include "time.cpp"
#include "zobrist.cpp"

#include "iostream"


using namespace std;

int main() {
  while (true) {
    string_t action = "";
    cout << "tp (test perft), q (quit)> ";
    getline(cin, action);
    if (action == "q") {
      break;
    };
    if (action == "tp") {
      test_perft();
    };
  };
};