//
// Created by HaoTianXue on 2021/4/22.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_RECORDMANAGER_H
#define DATABASE_MANAGEMENT_SYSTEM_RECORDMANAGER_H

#include "../stdafx.h"
#include "../base_element.h"
#include "../BufferManager/BufferManager.h"
#include "../Query.h"
#include "../Record.h"
#include "../InterpreterManager/BooleanQueryNode.h"


class RecordManager {

public:
    BufferManager bufferManager;

    RecordManager() {};

    void initTable(Table& table);

    void dropTable(string& tableName);

    int insert(Record& record);

    vector<Record> queryWithOffset(Table& table, vector<int>& offsets);

    vector<Record> queryWithCondition(Table& table, vector<BaseQuery*>& querys);

    vector<Record> queryWithCondition(Table& table, QueryNode* root);

    bool deleteWithOffset(string table, vector<int> offsets);

    int deleteWithCondition(Table& table, vector<BaseQuery*>& querys);

    int deleteWithCondition(Table& table, QueryNode* root);

    void writeBackAllToDisk();
};



#endif //DATABASE_MANAGEMENT_SYSTEM_RECORDMANAGER_H
