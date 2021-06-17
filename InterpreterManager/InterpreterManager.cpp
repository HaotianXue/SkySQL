//
// Created by HaoTianXue on 2021/4/22.
//
#include "InterpreterManager.h"
#include "Parser.h"

void InterpreterManager::interpretFile(string fileName) {
    parser.parseFile(fileName);
}

void InterpreterManager::interpretCommand() {
    parser.parseCommand();
}