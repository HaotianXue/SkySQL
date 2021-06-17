//
// Created by HaoTianXue on 2021/4/22.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_API_H
#define DATABASE_MANAGEMENT_SYSTEM_API_H

#include "../stdafx.h"
#include "../base_element.h"
#include "../Record.h"
#include "../Query.h"
#include "../RecordManager/RecordManager.h"
#include "../IndexManager/IndexManager.h"
#include "../BufferManager/BufferManager.h"
#include "../CatalogManager/CatalogManager.h"
#include "../InterpreterManager/BooleanQueryNode.h"


class API {
private:
    map<string, Table> tableMap;
    map<string, Index> indexMap;

    RecordManager recordManager;
    CatalogManager catalogManager;
    IndexManager indexManager;

public:
    API();

    void writeBackAllToDisk();

    void showTables();

    void createTable(Table newTable);
    void deleteTable(string tableName);

    void createIndex(Index newIndex);
    void deleteIndex(string indexName);

    void insertInto(string& table, vector<TableCell*>& cells);
    void deleteFrom(string& table, vector<BaseQuery*>& Querys);
    void selectFrom(string& table, vector<pair<string, string>>& columns, vector<BaseQuery*>& Querys);

    void deleteFrom(string& table, QueryNode* root);
    void selectFrom(string& table, vector<pair<string, string>>& columns, QueryNode* root);

    void selectFromWithOrderBy(string& table, vector<pair<string, string>>& columns, vector<BaseQuery*>& Querys, vector<pair<string, string>>& columnsToSort);
    void selectFromWithOrderBy(string& table, vector<pair<string, string>>& columns, QueryNode* root, vector<pair<string, string>>& columnsToSort);

    int getAttributeType(string& table, string& attr);
    int getAttributeSize(string& table, string& attr);

    pair<Table, vector<Record>> joinTable(string& leftTableName, string& rightTableName, string& joinType, vector<pair<string, string>>& onConditions);

    void selectJoin(pair<Table, vector<Record>>& tableInfo, vector<pair<string, string>>& op_columns, QueryNode* root, vector<pair<string, string>>& columnsToSort);
};


#endif //DATABASE_MANAGEMENT_SYSTEM_API_H
