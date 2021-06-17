#include "InterpreterManager/InterpreterManager.h"

int main(int argc, char const * argv[]) {

    cout << "Welcome to Zirong and Haotian's DBMS!" << endl;

    InterpreterManager interpreterManager;

    if (argc > 1) {
        interpreterManager.interpretFile(argv[1]);
    } else {
        interpreterManager.interpretCommand();
    }

    return 0;
}
