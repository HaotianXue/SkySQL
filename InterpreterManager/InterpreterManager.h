//
// Created by HaoTianXue on 2021/4/22.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_INTERPRETERMANAGER_H
#define DATABASE_MANAGEMENT_SYSTEM_INTERPRETERMANAGER_H

#include "Parser.h"

class InterpreterManager {
private:

    Parser parser;

public:

    InterpreterManager() {};

    void interpretFile(string fileName);

    void interpretCommand();

};

#endif //DATABASE_MANAGEMENT_SYSTEM_INTERPRETERMANAGER_H
