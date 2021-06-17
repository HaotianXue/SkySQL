//
// Created by HaoTianXue on 2021/4/22.
//
#include "Parser.h"
#include "../Query.h"
#include "../base_element.h"
#include "../stdafx.h"
#include "../API/API.h"

void Parser::parseFile(string& fileName) {
    ifstream sqlFileStream(fileName);
    if (sqlFileStream) {
        string fileContent((std::istreambuf_iterator<char>(sqlFileStream)),(std::istreambuf_iterator<char>()));
        fileContent = regex_replace(fileContent, regex("\\s*$"), "");
        vector<string> SQLcommandVector = split<string>(fileContent, ";");

        for(auto& sqlCommand: SQLcommandVector) {
            sqlCommand = regex_replace(sqlCommand, regex("^\\s+"), "");
            parseSQLCommand(sqlCommand);
        }
        api.writeBackAllToDisk();
        cout << "finished executing file " << fileName << endl;
    } else {
        cout << "could not open the file " << fileName << '\n';
    }
    sqlFileStream.close();
}

void Parser::parseCommand() {
    string sqlCommand = "";
    string commandAppended = "";
    cout << "sql>";
    while (getline(cin, commandAppended)) {
        commandAppended = regex_replace(commandAppended, regex("\\s$"), "");
        sqlCommand += commandAppended + ' ';
        if (commandAppended.back() == ';') {
            sqlCommand.pop_back();
            sqlCommand.pop_back();
            if (sqlCommand == "exit") {
                break;
            }
            cout << '\n';
            parseSQLCommand(sqlCommand);
            sqlCommand = "";
            commandAppended = "";
            cout << "sql>";
            continue;
        }
        cout << "\n->";
    }
    api.writeBackAllToDisk();
}

void Parser::parseSQLCommand(string& sqlStatement) {
    try {
        sqlStatement = regex_replace(sqlStatement, regex("[\\(\\),]"), " $& ");

        istringstream inputSQLStream(sqlStatement);
        string token0, token1;
        inputSQLStream >> token0 >> token1;
        if (strToLower(token0) == "create" && strToLower(token1) == "table") {
            parseCreateTable(sqlStatement);
            return;
        }
        if (strToLower(token0) == "drop" && strToLower(token1) == "table") {
            parseDropTable(sqlStatement);
            return;
        }
        if (strToLower(token0) == "create" && strToLower(token1) == "index") {
            parseCreateIndex(sqlStatement);
            return;
        }
        if (strToLower(token0) == "drop" && strToLower(token1) == "index") {
            parseDropIndex(sqlStatement);
            return;
        }
        if (strToLower(token0) == "select") {
            if (sqlStatement.find("JOIN") == string::npos) {
                parseSelect(sqlStatement);
            } else {
                //TODO: support JOIN: SELECT row_list from left_table <JOIN> right_table ON <join condition> WHERE <where condition>
                parseSelectWithJoin(sqlStatement);
            }
            return;
        }
        if (strToLower(token0) == "insert" && strToLower(token1) == "into") {
            parseInsert(sqlStatement);
            return;
        }
        if (strToLower(token0) == "delete" && strToLower(token1) == "from") {
            parseDelete(sqlStatement);
            return;
        }
        if (strToLower(token0) == "source" || strToLower(token0) == "exec") {
            parseFile(token1);
            return;
        }
        if (strToLower(token0) == "show" && strToLower(token1) == "tables") {
            parseShowTables();
            return;
        }
        cout << "Error at parsing SQL statement : No operation: " << token0 << ". Aborted." <<  endl;
        return;
    } catch(exception & e) {
        cout << "Error at tokenizing SQL statement. Execution Aborted." << endl;
        return;
    }
}

void Parser::parseShowTables() {
    api.showTables();
}

void Parser::parseCreateTable(string& statement) {
    istringstream inputSQLStream(statement);
    string currentToken;

    // ignore first two tokens.
    inputSQLStream >> currentToken >> currentToken >> currentToken;
    string tableName = currentToken;
    string primaryKey = "";

    inputSQLStream >> currentToken;
    if (currentToken != "(") {
        cout << "Error : Expected '(' after the table name. Aborted." << endl;
        return;
    }

    // parse attributes
    vector<TableAttribute> attributes;
    while (true) {
        if (currentToken == ")") {
            break;
        }
        // read info of each attribute
        string attrToken, typeToken;
        inputSQLStream >> attrToken >> typeToken;

        // case int
        if (typeToken == "int") {
            inputSQLStream >> currentToken;
            if (currentToken == "unique") {
                attributes.push_back(TableAttribute(attrToken, INT, sizeof(int), true));
                inputSQLStream >> currentToken;
            }
            else {
                attributes.push_back(TableAttribute(attrToken, INT, sizeof(int), false));
            }
            continue;
        }
        if (typeToken == "float") {
            inputSQLStream >> currentToken;
            if (currentToken == "unique") {
                attributes.push_back(TableAttribute(attrToken, FLOAT, sizeof(float), true));
                inputSQLStream >> currentToken;
            } else {
                attributes.push_back(TableAttribute(attrToken, FLOAT, sizeof(float), false));
            }
            continue;
        }
        if (typeToken == "char" || typeToken == "varchar" || typeToken == "string") {
            int size;
            inputSQLStream >> currentToken >> size >> currentToken >> currentToken;
            if (currentToken == "unique") {
                attributes.push_back(TableAttribute(attrToken, STRING, size, true));
                inputSQLStream >> currentToken;
            } else {
                attributes.push_back(TableAttribute(attrToken, STRING, size, false));
            }
            continue;
        }
        if (attrToken == "primary" && typeToken == "key") {
            inputSQLStream >> currentToken >> primaryKey >> currentToken >> currentToken;
            continue;
        }
    }
    Table newTable(tableName, primaryKey, attributes);
    cout << "creating table" << endl;
    api.createTable(newTable);
    inputSQLStream.clear();
    inputSQLStream.str("");
}

void Parser::parseDropTable(string& statement) {
    istringstream inputSQLStream(statement);
    string tempToken, tableName;
    inputSQLStream >> tempToken >> tempToken >> tableName;
    api.deleteTable(tableName);
    inputSQLStream.clear();
    inputSQLStream.str("");
}

void Parser::parseCreateIndex(string& statement) {
    istringstream inputSQLStream(statement);
    string thisToken, indexName, tableName, AttributeName;
    //                create       index       <INDEX_NAME>  on           <TABLE>      (            <ATTRIBUTE>      )
    inputSQLStream >> thisToken >> thisToken >> indexName >> thisToken >> tableName >> thisToken >> AttributeName;
    Index newIndex(tableName, AttributeName, indexName);
    api.createIndex(newIndex);
    inputSQLStream.clear();
    inputSQLStream.str("");
}

void Parser::parseDropIndex(string& statement) {
    istringstream inputSQLStream(statement);
    string tempToken, indexName;
    inputSQLStream >> tempToken >> tempToken >> indexName;
    api.deleteIndex(indexName);
    inputSQLStream.clear();
    inputSQLStream.str("");
}

void Parser::parseSelect(string& statement) {
    istringstream inputSQLStream(statement);
    string tempToken, tableName, attributeToken, nextToken;
    vector<string> columns;
    vector<pair<string, string>> op_column_pairs;
    inputSQLStream >> tempToken;
    do {
        inputSQLStream >> attributeToken >> nextToken; // MAX(col) or DISTINCT col1, col2, ...
        string col_op = "NONE";
        //TODO: add DISTINCT and MAX()
        if (attributeToken == "DISTINCT") {
            col_op = "DISTINCT";
            attributeToken = nextToken;
            inputSQLStream >> nextToken;
        } else if (attributeToken == "MAX") { // MAX ( col )
            col_op = "MAX";
            inputSQLStream >> attributeToken;
            inputSQLStream >> nextToken >> nextToken; // consume ")"
        }
        op_column_pairs.push_back({col_op, attributeToken});
        columns.push_back(attributeToken);
    } while(nextToken == ",");
    inputSQLStream >> tableName;

    string whereToken;
    inputSQLStream >> whereToken;
    if (whereToken[0] && whereToken == "where") { // TODO: add GROUP BY and ORDER BY (each col can choose either ASC or DESC)
        int startIndex = statement.find("where") + 5;
        if (startIndex == string::npos) startIndex = statement.find("WHERE") + 5;
        string whereStatement = statement.substr(startIndex);
        int endIndex = whereStatement.find("ORDER BY");
        QueryNode* root = nullptr;
        if (endIndex != string::npos) {
            int cutSize = whereStatement.size() - endIndex;
            string orderByStatement = whereStatement.substr(endIndex + 9); // col1 ASC, col2 DESC, col3 , ...
            whereStatement = whereStatement.substr(0, whereStatement.size() - cutSize);
            istringstream orderByStream(orderByStatement);
            string colName, commaToken, sortToken;
            vector<pair<string, string>> columnsToSort;
            do {
                orderByStream >> colName >> sortToken >> commaToken;
                columnsToSort.push_back({colName, sortToken});
                if (orderByStream.eof()) break;
            } while (commaToken == ",");
            root = parseBooleanStatement(whereStatement, tableName);
            api.selectFromWithOrderBy(tableName, op_column_pairs, root, columnsToSort);
        } else {
            root = parseBooleanStatement(whereStatement, tableName);
            api.selectFrom(tableName, op_column_pairs, root);
        }

        delete root;
        root = nullptr;
        // api.selectFrom(tableName, columns, queryVec);
    } else if (whereToken[0] && whereToken == "ORDER"){
        // TODO: ORDER BY col1 DESC, col2 ASC, ...,
        // TODO: how to combine the default sort(no DESC/ASC) and DESC/ASC together
        int endIndex = statement.find("ORDER BY");
        QueryNode* root = nullptr;
        if (endIndex != string::npos) {
            string orderByStatement = statement.substr(endIndex + 9); // col1 DESC, col2 ASC, col3 DESC, ...
            istringstream orderByStream(orderByStatement);
            string colName, commaToken, sortToken;
            vector<pair<string, string>> columnsToSort;
            do {
                orderByStream >> colName >> sortToken >> commaToken;
                columnsToSort.push_back({colName, sortToken});
                if (orderByStream.eof()) break;
            } while (commaToken == ",");
            vector<BaseQuery*> emptyVec;
            api.selectFromWithOrderBy(tableName, op_column_pairs, emptyVec, columnsToSort);
        } else {
            vector<BaseQuery*> emptyVec;
            api.selectFrom(tableName, op_column_pairs, emptyVec);

            emptyVec.clear();
            emptyVec.shrink_to_fit();
        }
    } else { // select _ from table;
        vector<BaseQuery*> emptyVec;
        api.selectFrom(tableName, op_column_pairs, emptyVec);

        emptyVec.clear();
        emptyVec.shrink_to_fit();
    }

    inputSQLStream.clear();
    inputSQLStream.str("");
}

void Parser::parseInsert(string& statement) {
    istringstream inputSQLStream(statement);
    string thisToken, tableName;
    //                insert       into         <TABLE>      values       (
    inputSQLStream >> thisToken >> thisToken >> tableName >> thisToken >> thisToken;

    vector<TableCell*> elementsForRecord;
    string elementStartToken, elementEndToken, nextSignToken, finalElementToken;
    while(true) {
        inputSQLStream >> elementStartToken;
        if (elementStartToken[0] == '"') {
            elementEndToken = elementStartToken;
            while(elementEndToken.back() != '"') {
                inputSQLStream >> elementEndToken;
            }
            size_t startPos = statement.find(elementStartToken) + 1;
            size_t tokenLength = statement.find(elementEndToken) + elementEndToken.length() - 1 - startPos;
            finalElementToken = statement.substr(startPos, tokenLength);
        } else {
            finalElementToken = elementStartToken;
        }
        elementsForRecord.push_back(new TableCell(finalElementToken));
        inputSQLStream >> nextSignToken;
        if (nextSignToken == ")") {
            break;
        }
    }

    inputSQLStream.clear();
    inputSQLStream.str("");

    api.insertInto(tableName, elementsForRecord);

    // clear out memory
    for (auto& ptr: elementsForRecord) {
        ptr = nullptr;
        delete ptr;
    }
    elementsForRecord.clear();
    elementsForRecord.shrink_to_fit();
}

void Parser::parseDelete(string& statement) {
    istringstream inputSQLStream(statement);
    string tempToken, tableName, attributeToken;
    inputSQLStream >> tempToken >> tempToken >> tableName >> tempToken;
    if (tempToken[0]) {
        int startIndex = statement.find(tempToken) + 5;
        // if (startIndex == string::npos) startIndex = statement.find(tempToken) + 5;
        string subStatement = statement.substr(startIndex);
        QueryNode* root = parseBooleanStatement(subStatement, tableName);
        api.deleteFrom(tableName, root);

        delete root;
        root = nullptr;
    } else {
        vector<BaseQuery*> emptyVec;
        api.deleteFrom(tableName, emptyVec);

        emptyVec.clear();
        emptyVec.shrink_to_fit();
    }

    inputSQLStream.clear();
    inputSQLStream.str("");
}

QueryNode* Parser::parseBooleanStatement(string& statement, string& tableName) {
    int pos = 0;
    vector<string> tokens = splitByStr(statement, "AND", "OR");
    return parseBooleanExpression(tokens, pos, tableName);
}

QueryNode * Parser::parseBooleanExpression(vector<string>& tokens, int &pos, string& tableName) {
    QueryNode* lhs = parseBooleanTerm(tokens, pos, tableName);
    while (pos < tokens.size() && (tokens[pos] == "OR")) {
        string op = tokens[pos++];
        QueryNode* rhs = parseBooleanTerm(tokens, pos, tableName);
        lhs = new QueryNode(op, lhs, rhs);
    }
    return lhs;
}

QueryNode * Parser::parseBooleanTerm(vector<string>& tokens, int &pos, string& tableName) {
    QueryNode* lhs = parseBooleanFactor(tokens, pos, tableName);
    while (pos < tokens.size() && (tokens[pos] == "AND")) {
        string op = tokens[pos++];
        QueryNode* rhs = parseBooleanFactor(tokens, pos, tableName);
        lhs = new QueryNode(op, lhs, rhs);
    }
    return lhs;
}

QueryNode * Parser::parseBooleanFactor(vector<string>& tokens, int &pos, string& tableName) {
    if (tokens[pos] == "(") {
        string leftBracket = tokens[pos++];
        QueryNode* exp = parseBooleanExpression(tokens, pos, tableName);
        string rightBracket = tokens[pos++];
        return exp;
    } else { // construct BaseQuery
        string statement = tokens[pos++];
        istringstream inputSQLStream(statement);
        string compareAttribute, compratorToken, valueStartToken, valueEndToken, finalValueToken;
        inputSQLStream >> compareAttribute >> compratorToken >> valueStartToken;
        if (valueStartToken.front() == '"') {
            valueEndToken = valueStartToken;
            while(valueEndToken.back() != '"') {
                inputSQLStream >> valueEndToken;
            }
            size_t startPos = statement.find(valueStartToken) + 1;
            size_t tokenLength = statement.find(valueEndToken) + valueEndToken.length() - 1 - startPos;
            finalValueToken = statement.substr(startPos, tokenLength);
            finalValueToken.resize(api.getAttributeSize(tableName, compareAttribute));
        } else {
            finalValueToken = valueStartToken;
        }
        // to do : reinterpret value.
        BaseQuery * thisQuery = nullptr;
        if (compratorToken == "=" || compratorToken == "!=") {
            switch (api.getAttributeType(tableName, compareAttribute)) {
                case INT:
                    thisQuery = new SingleQuery<int>(compareAttribute, stoi(finalValueToken), compratorToken == "!=");
                    break;
                case FLOAT:
                    thisQuery = new SingleQuery<float>(compareAttribute, stof(finalValueToken), compratorToken == "!=");
                    break;
                case STRING:
                    thisQuery = new SingleQuery<string>(compareAttribute, finalValueToken, compratorToken == "!=");
                    break;
            }
        } else {
            switch (api.getAttributeType(tableName, compareAttribute)) {
                case INT:
                    thisQuery = new infinityRangeQuery<int>(compareAttribute, stoi(finalValueToken), compratorToken);
                    break;
                case FLOAT:
                    thisQuery = new infinityRangeQuery<float>(compareAttribute, stof(finalValueToken), compratorToken);
                    break;
                case STRING:
                    thisQuery = new infinityRangeQuery<string>(compareAttribute, finalValueToken, compratorToken);
                    break;
            }
        }
        return new QueryNode(thisQuery);
    }
}


// SELECT row_list from left_table <JOIN> right_table ON <join condition> WHERE <where condition>
void Parser::parseSelectWithJoin(string &statement) {
    istringstream inputSQLStream(statement);
    string tempToken, attributeToken, nextToken;
    vector<string> columns;
    vector<pair<string, string>> op_column_pairs;
    inputSQLStream >> tempToken;
    do {
        inputSQLStream >> attributeToken >> nextToken; // MAX(col) or DISTINCT col1, col2, ...
        string col_op = "NONE";
        if (attributeToken == "DISTINCT") {
            col_op = "DISTINCT";
            attributeToken = nextToken;
            inputSQLStream >> nextToken;
        } else if (attributeToken == "MAX") { // MAX ( col )
            col_op = "MAX";
            inputSQLStream >> attributeToken;
            inputSQLStream >> nextToken >> nextToken; // consume ")"
        }
        op_column_pairs.push_back({col_op, attributeToken});
        columns.push_back(attributeToken);
    } while(nextToken == ",");

    string leftTable, rightTable, joinToken, conditionToken, joinType;
    if (statement.find("INNER JOIN") != string::npos) {
        inputSQLStream >> leftTable >> joinType >> joinToken >> rightTable;
    } else if (statement.find("LEFT JOIN") != string::npos) {
        inputSQLStream >> leftTable >> joinType >> joinToken >> rightTable;
    } else if (statement.find("RIGHT JOIN") != string::npos) {
        inputSQLStream >> leftTable >> joinType >> joinToken >> rightTable;
    }else {
        inputSQLStream >> leftTable >> joinToken >> rightTable;
        joinType = "CROSS";
    }

    assert(joinToken == "JOIN");

    inputSQLStream >> conditionToken; // could be eof or WHERE or ON

    if (conditionToken[0] && conditionToken == "ON") { // ON table1.colx = table2.coly, .... (WHERE) ... / (ORDER BY) ...

        string leftCol, rightCol, equalToken, commaToken;

        vector<pair<string, string>> onConditions;

        do {
            inputSQLStream >> leftCol >> equalToken >> rightCol >> commaToken;
            onConditions.push_back({leftCol, rightCol});
            if (inputSQLStream.eof()) break;
        } while (commaToken == ",");

        pair<Table, vector<Record>> joinedTableInfo = api.joinTable(leftTable, rightTable, joinType, onConditions);

        if (commaToken == "WHERE") { // WHERE table1.colx > table2.coly AND .... OR ...
            int startIndex = statement.find("WHERE") + 6;
            string whereStatement = statement.substr(startIndex);
            int endIndex = whereStatement.find("ORDER BY");

            if (endIndex != string::npos) { // WHERE ... ORDER BY ...
                int cutSize = whereStatement.size() - endIndex;
                string orderByStatement = whereStatement.substr(endIndex + 9); // table1.col1 ASC, table1.col2 DESC, ...
                whereStatement = whereStatement.substr(0, whereStatement.size() - cutSize);
                istringstream orderByStream(orderByStatement);
                string colName, commaToken, sortToken;
                vector<pair<string, string>> columnsToSort;
                do {
                    orderByStream >> colName >> sortToken >> commaToken;
                    columnsToSort.push_back({colName, sortToken});
                    if (orderByStream.eof()) break;
                } while (commaToken == ",");
                QueryNode* root = parseBooleanStatement(whereStatement, joinedTableInfo.first.name);
                api.selectJoin(joinedTableInfo, op_column_pairs, root, columnsToSort);
            } else { // WHERE ...; no ORDER BY
                QueryNode* root = parseBooleanStatement(whereStatement, joinedTableInfo.first.name);
                vector<pair<string, string>> emptyVec;
                api.selectJoin(joinedTableInfo, op_column_pairs, root, emptyVec);
            }
        } else if (commaToken == "ORDER") { // ORDER BY table1.colx ASC, table2.coly DESC, ...
            string nextToken;
            inputSQLStream >> nextToken; //consume "BY"
//            string orderByStatement = statement.substr(endIndex + 9); // col1 DESC, col2 ASC, col3 DESC, ...
//            istringstream orderByStream(orderByStatement);
            string colName, commaToken, sortToken;
            vector<pair<string, string>> columnsToSort;
            do {
                inputSQLStream >> colName >> sortToken >> commaToken;
                columnsToSort.push_back({colName, sortToken});
                if (inputSQLStream.eof()) break;
            } while (commaToken == ",");
            api.selectJoin(joinedTableInfo, op_column_pairs, NULL, columnsToSort);
        } else { // no WHERE and ORDER BY, just SELECT col_list from table1 JOIN table2
            vector<pair<string, string>> emptyVec;
            api.selectJoin(joinedTableInfo, op_column_pairs, NULL, emptyVec);
        }
    } else {
        vector<pair<string, string>> onConditions;
        pair<Table, vector<Record>> joinedTableInfo = api.joinTable(leftTable, rightTable, joinType, onConditions);

        if (conditionToken[0] && conditionToken == "WHERE") { // no "ON", select _ from .. JOIN .. WHERE ...
            int startIndex = statement.find("WHERE") + 6;
            string whereStatement = statement.substr(startIndex);
            int endIndex = whereStatement.find("ORDER BY");

            QueryNode* root = parseBooleanStatement(whereStatement, joinedTableInfo.first.name);
            if (endIndex != string::npos) { // WHERE ... ORDER BY ...
                int cutSize = whereStatement.size() - endIndex;
                string orderByStatement = whereStatement.substr(endIndex + 9); // table1.col1 ASC, table1.col2 DESC, ...
                whereStatement = whereStatement.substr(0, whereStatement.size() - cutSize);
                istringstream orderByStream(orderByStatement);
                string colName, commaToken, sortToken;
                vector<pair<string, string>> columnsToSort;
                do {
                    orderByStream >> colName >> sortToken >> commaToken;
                    columnsToSort.push_back({colName, sortToken});
                    if (orderByStream.eof()) break;
                } while (commaToken == ",");
                api.selectJoin(joinedTableInfo, op_column_pairs, root, columnsToSort);
            } else { // WHERE ...; no ORDER BY
                vector<pair<string, string>> emptyVec;
                api.selectJoin(joinedTableInfo, op_column_pairs, root, emptyVec);
            }

        } else { // eof, no condition, just SELECT col_list from table1 JOIN table2
            vector<pair<string, string>> emptyVec;
            api.selectJoin(joinedTableInfo, op_column_pairs, NULL, emptyVec);
        }
    }

    inputSQLStream.clear();
    inputSQLStream.str("");
}
