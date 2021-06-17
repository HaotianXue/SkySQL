//
// Created by HaoTianXue on 2021/4/22.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_PARSER_H
#define DATABASE_MANAGEMENT_SYSTEM_PARSER_H

#include "../stdafx.h"
#include "../API/API.h"
#include "../Query.h"
#include "BooleanQueryNode.h"


class Parser {

private:

    API api;

    void parseSQLCommand(string& sqlStatement);

    void parseShowTables();

    void parseCreateTable(string& statement);
    void parseDropTable(string& statement);

    void parseCreateIndex(string& statement);
    void parseDropIndex(string& statement);

    void parseSelect(string& statement);
    void parseSelectWithJoin(string& statement);
    void parseInsert(string& statement);
    void parseDelete(string& statement);

    QueryNode* parseBooleanStatement(string& statement, string& tableName);
    QueryNode* parseBooleanExpression(vector<string>& tokens, int& pos, string& tableName);
    QueryNode* parseBooleanTerm(vector<string>& tokens, int& pos, string& tableName);
    QueryNode* parseBooleanFactor(vector<string>& tokens, int& pos, string& tableName);

public:

    Parser() {};

    void parseFile(string& fileName);

    void parseCommand();
};


#endif //DATABASE_MANAGEMENT_SYSTEM_PARSER_H
