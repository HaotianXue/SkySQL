//
// Created by HaoTianXue on 2021/4/22.
//
#include "API.h"

API::API() {
    cout << "Initializing... ";
    tableMap = catalogManager.initializeTables();
    indexMap = catalogManager.initializeIndexes();

    for (auto& tableIter: tableMap) {
        recordManager.initTable(tableIter.second);
    }

    for (auto& indexIter : indexMap) {
        indexManager.createIndex(indexIter.second);
        // vector<Record> allRecords = recordManager.queryWithCondition(tableMap[indexIter.second.indexName], {});
        vector<BaseQuery*> emptyVec;
        vector<Record> allRecords = recordManager.queryWithCondition(tableMap[indexIter.second.tableName], emptyVec);
        emptyVec.clear();
        emptyVec.shrink_to_fit();
        int offset = 0;
        for_each(allRecords.begin(), allRecords.end(), [&](auto record) {
            indexManager.insertIntoIndex(indexIter.second, record[indexIter.second.attributeName], offset);
            // offset += tableMap[indexIter.second.indexName].recordSize;
            offset += tableMap[indexIter.second.tableName].recordSize;
        });
    }
    cout << "Done. " << endl;
}

void API::writeBackAllToDisk() {
    recordManager.writeBackAllToDisk();
}

void API::showTables() {
    for (auto& [tableName, table]: tableMap) {
        cout << "table: " << tableName << endl;
    }
}

void API::createTable(Table newTable) {
    // if (tableMap.find(newTable.name) != tableMap.end()) {
    if (tableMap.count(newTable.name)) {
        cout << "Failed to create table: Table " << newTable.name << " already existed! " << endl;
        return;
    }
    if(catalogManager.createTableCatalog(newTable)) {
        // to do : create index for primary key.
        tableMap.insert(pair<string, Table>(newTable.name, newTable));
        recordManager.initTable(newTable);
        Index curIndex(newTable.name, newTable.primaryKey, catalogManager.getPrimaryKeyIndexName(newTable));
        indexManager.createIndex(curIndex);
        cout << "Successfully created table " << newTable.name << endl;
        return;
    }
    cout << "Creating table " << newTable.name << " failed. " << endl;
    return;
}

void API::deleteTable(string tableName) {
    // if (tableMap.find(tableName) == tableMap.end()) {
    if (!tableMap.count(tableName)) {
        cout << "Failed to drop table: Table " << tableName << " does not exist! " << endl;
        return;
    }
    if (catalogManager.dropTableCatalog(tableName)) {
        // to do : drop index for primary key.
        recordManager.dropTable(tableName);
        tableMap.erase(tableName);
        cout << "Successfully dropped table " << tableName << endl;
        return;
    }
    cout << "Failed to drop table " << tableName << " failed. " << endl;
    return;
}


void API::createIndex(Index newIndex) {
    // if (tableMap.find(newIndex.tableName) == tableMap.end()) {
    if (!tableMap.count(newIndex.tableName)) {
        cout << "Failed to create index: Table " << newIndex.tableName << " does not exist! " << endl;
        return;
    }
    // if (indexMap.find(newIndex.indexName) != indexMap.end()) {
    if (indexMap.count(newIndex.indexName)) {
        cout << "Failed to create index: Index " << newIndex.indexName << " already existed! " << endl;
        return;
    }
    if (catalogManager.createIndexCatalog(newIndex)) {
        // to do : finish index build.
        indexMap.insert(pair<string, Index>(newIndex.indexName, newIndex));
        cout << "Successfully created index " << newIndex.indexName << endl;
        return;
    }
    cout << "Creating index " << newIndex.indexName << " failed. " << endl;
    return;
}

void API::deleteIndex(string indexName) {
    // if (indexMap.find(indexName) == indexMap.end()) {
    if (!indexMap.count(indexName)) {
        cout << "Failed to drop index: Index " << indexName << " does not exist! " << endl;
        return;
    }
    if(catalogManager.dropIndexCatalog(indexName)) {
        // to do : drop index.
        indexMap.erase(indexName);
        cout << "Successfully dropped index " << indexName << endl;
        return;
    }
    cout << "Failed to drop index " << indexName << ". " << endl;
    return;
}


void API::insertInto(string& table, vector<TableCell*>& cells) {
    // if (tableMap.find(table) == tableMap.end()) {
    if (!tableMap.count(table)) {
        cout << "Error at insert : Table " << table << " does not exist! " << endl;
        return;
    }
    auto attributes = tableMap[table].attributes;

    // generate querys for unique attributes
    int attrIter = 0;
    for(auto& attr : attributes) {
        cells[attrIter] -> reinterpret(attr.type);
        if (attr.isUnique) {
            BaseQuery * uniQuery = nullptr;
            switch (attr.type)
            {
                case INT:
                    uniQuery = new SingleQuery<int>(attr.name, cells[attrIter] -> intValue);
                    break;
                case FLOAT:
                    uniQuery = new SingleQuery<float>(attr.name, cells[attrIter] -> floatValue);
                    break;
                    // case STRING:
                default:
                    uniQuery = new SingleQuery<string>(attr.name, cells[attrIter] -> stringValue);
                    break;
            }

            vector<BaseQuery*> queryVec({uniQuery});
            if (recordManager.queryWithCondition(tableMap[table], queryVec).size() > 0) {
                cout << "Error at insert : value of unique attribute " << attr.name << " repeated. " << endl;

                uniQuery = nullptr;
                delete uniQuery;
                queryVec.clear();

                return;
            }

            uniQuery = nullptr;
            delete uniQuery;
            queryVec.clear();

        }
        ++attrIter;
    }

    Record newRecord(tableMap[table], cells);
    int offset = recordManager.insert(newRecord);
    if (offset >= 0) {
        for (auto& indexIter: indexMap) {
            if (indexIter.second.tableName == table) {
                indexManager.insertIntoIndex(indexIter.second, newRecord[indexIter.second.attributeName], offset);
            }
        }
        cout << "Successfully inserted to table " << table << endl;
        return ;
    }
    cout << "Failed to insert into table " << table << endl;

}

void API::deleteFrom(string& table, vector<BaseQuery*>& Querys) {
    // if (tableMap.find(table) == tableMap.end()) {
    if (!tableMap.count(table)) {
        cout << "Error at select : Table " << table << " does not exist! " << endl;
        return;
    }
    if (recordManager.deleteWithCondition(tableMap[table], Querys)) {
        cout << "Successfully performed deletion. " << '\n';
        return;
    }

    cout << "No record deleted.";

}


void API::deleteFrom(string& table, QueryNode *root) {
    if (!tableMap.count(table)) {
        cout << "Error at select : Table " << table << " does not exist! " << endl;
        return;
    }
    if (recordManager.deleteWithCondition(tableMap[table], root)) {
        cout << "Successfully performed deletion. " << '\n';
        return;
    }

    cout << "No record deleted.";
}


void API::selectFrom(string& table, vector<pair<string, string>>& op_columns, vector<BaseQuery*>& Querys) {
    // if (tableMap.find(table) == tableMap.end()) {
    if (!tableMap.count(table)) {
        cout << "Error at select : Table " << table << " does not exist! " << endl;
        return;
    }

    // === print columns and get print vector ====
    // process * situation
    vector<int> printIndex;
    if (op_columns[0].second == "*") {
        op_columns.clear();
        for (auto & attr : tableMap[table].attributes) {
            op_columns.push_back({"NONE", attr.name});
        }
    }
    for (auto &op_col: op_columns) {
        size_t i;
        for (i = 0; i < tableMap[table].attributes.size(); ++i) {
            if (tableMap[table].attributes[i].name == op_col.second) {
                printIndex.push_back(i);
                break;
            }
        }
        if (i == tableMap[table].attributes.size()) {
            cout << "Error : Attribute " << op_col.second << " does not exist in table " << table << endl;
            return;
        }
    }
    cout << "-------------------------------------------------------" << endl;
    cout << "| ";
    for(auto& op_col: op_columns) {
        cout << op_col.second << " | ";
    }
    cout << endl << "-------------------------------------------------------" << endl;

    // ==== query and print result ===
    vector<Record> queryResult = recordManager.queryWithCondition(tableMap[table], Querys);

    for (auto& op_col: op_columns) { // TODO: NOTE: only supports MAX on one column!
        if (op_col.first == "MAX") { // print max value in this col
            auto comp_func = [&](Record& r1, Record& r2) { return r1[op_col.second] < r2[op_col.second]; };
            Record maxRecord = *std::max_element(queryResult.begin(), queryResult.end(), comp_func);
            cout << "| ";
            cout << maxRecord[op_col.second] << endl;
            cout << "-------------------------------------------------------" << endl << endl;
            return;
        } else break;
    }

    vector<string> distinct_cols;
    for (auto& op_col: op_columns) {
        if (op_col.first == "DISTINCT") { // print max value in this col
            distinct_cols.push_back(op_col.second);
        }
    }

    if (distinct_cols.size() > 0) {
        auto notSameFunc = [distinct_cols](Record r1, Record r2)->bool {
            for (auto& col: distinct_cols) {
                if (r1[col] == r2[col]) return false;
            }
            return true;
        };

        set<Record, decltype(notSameFunc)> mySet(notSameFunc);

        for (auto record: queryResult) {
            mySet.insert(record);
        }

        for (auto record: mySet) {
            if (record.validate()) {
                cout << "| ";
                for (auto& index: printIndex) {
                    cout << *record.cells[index] << " | ";
                }
                cout << endl;
            }
        }
        cout << "-------------------------------------------------------" << endl << endl;

        return;
    }

    for (auto& record: queryResult) {
        if (record.validate()) {
            cout << "| ";
            for (auto& index: printIndex) {
                cout << *record.cells[index] << " | ";
            }
            cout << endl;
        }
    }
    cout << "-------------------------------------------------------" << endl << endl;
}

void API::selectFrom(string& table, vector<pair<string, string>>& op_columns, QueryNode* root) {
    if (!tableMap.count(table)) {
        cout << "Error at select : Table " << table << " does not exist! " << endl;
        return;
    }

    // === print columns and get print vector ====
    // process * situation
    vector<int> printIndex;
    if (op_columns[0].second == "*") {
        op_columns.clear();
        for (auto & attr : tableMap[table].attributes) {
            op_columns.push_back({"NONE", attr.name});
        }
    }

    // TODO: add DISTINCT() and MAX()
    for (auto &op_col: op_columns) {
        size_t i;
        for (i = 0; i < tableMap[table].attributes.size(); ++i) {
            if (tableMap[table].attributes[i].name == op_col.second) {
                printIndex.push_back(i);
                break;
            }
        }
        if (i == tableMap[table].attributes.size()) {
            cout << "Error : Attribute " << op_col.second << " does not exist in table " << table << endl;
            return;
        }
    }
    cout << "-------------------------------------------------------" << endl;
    cout << "| ";
    for(auto& op_col: op_columns) {
        cout << op_col.second << " | ";
    }
    cout << endl << "-------------------------------------------------------" << endl;
    // ==== query and print result ===
    vector<Record> queryResult = recordManager.queryWithCondition(tableMap[table], root);

    for (auto& op_col: op_columns) { // TODO: NOTE: only supports MAX on one column!
        if (op_col.first == "MAX") { // print max value in this col
            auto comp_func = [&](Record& r1, Record& r2) { return r1[op_col.second] < r2[op_col.second]; };
            Record maxRecord = *std::max_element(queryResult.begin(), queryResult.end(), comp_func);
            cout << "| ";
            cout << maxRecord[op_col.second] << endl;
            cout << "-------------------------------------------------------" << endl << endl;
            return;
        }
    }

    vector<string> distinct_cols;
    for (auto& op_col: op_columns) {
        if (op_col.first == "DISTINCT") { // print max value in this col
            distinct_cols.push_back(op_col.second);
        }
    }

    if (distinct_cols.size() > 0) {
        auto notSameFunc = [distinct_cols](Record r1, Record r2) {
            for (auto col: distinct_cols) {
                if (r1[col] == r2[col]) return false;
            }
            return true;
        };

        set<Record, decltype(notSameFunc)> mySet(notSameFunc);

        for (auto record: queryResult) {
            mySet.insert(record);
        }

        for (auto record: mySet) {
            if (record.validate()) {
                cout << "| ";
                for (auto& index: printIndex) {
                    cout << *record.cells[index] << " | ";
                }
                cout << endl;
            }
        }
        cout << "-------------------------------------------------------" << endl << endl;

        return;
    }


    // TODO: NOTE: here we return all columns as queryResult and we use printIndex to filter out
    for (auto & record: queryResult) {
        if (record.validate()) {
            cout << "| ";
            for (auto & index: printIndex) {
                cout << *record.cells[index] << " | ";
            }
            cout << endl;
        }
    }
    cout << "-------------------------------------------------------" << endl << endl;
}

void API::selectFromWithOrderBy(string &table, vector<pair<string, string>> &op_columns, QueryNode *root,
                                vector<pair<string, string>> &columnsToSort) {
    if (!tableMap.count(table)) {
        cout << "Error at select : Table " << table << " does not exist! " << endl;
        return;
    }

    // === print columns and get print vector ====
    // process * situation
    vector<int> printIndex;
    if (op_columns[0].second == "*") {
        op_columns.clear();
        for (auto & attr : tableMap[table].attributes) {
            op_columns.push_back({"NONE", attr.name});
        }
    }

    for (auto &op_col: op_columns) {
        size_t i;
        for (i = 0; i < tableMap[table].attributes.size(); ++i) {
            if (tableMap[table].attributes[i].name == op_col.second) {
                printIndex.push_back(i);
                break;
            }
        }
        if (i == tableMap[table].attributes.size()) {
            cout << "Error : Attribute " << op_col.second << " does not exist in table " << table << endl;
            return;
        }
    }
    cout << "-------------------------------------------------------" << endl;
    cout << "| ";
    for(auto& op_col: op_columns) {
        cout << op_col.second << " | ";
    }
    cout << endl << "-------------------------------------------------------" << endl;
    // ==== query and print result ===
    vector<Record> queryResult = recordManager.queryWithCondition(tableMap[table], root);

    auto compFunc = [&](Record& r1, Record& r2) {
//        auto c1 = make_tuple(r1[columnsToSort[0]]);
//        auto c2 = make_tuple(r2[columnsToSort[0]]);

        int i = 0;
        while (i < columnsToSort.size()) {
            if (r1[columnsToSort[i].first] == r2[columnsToSort[i].first]) {
                ++i;
                continue;
            }
            return columnsToSort[i].second == "ASC" ? r1[columnsToSort[i].first] < r2[columnsToSort[i].first] : r1[columnsToSort[i].first] > r2[columnsToSort[i].first];
        }

//        for (int i = 1; i < columnsToSort.size(); ++i) {
//            auto c3 = tuple_cat(c1, make_tuple(r1[columnsToSort[i]]));
//            auto c4 = tuple_cat(c2, make_tuple(r2[columnsToSort[i]]));
//            if (i == columnsToSort.size() - 1) return c3 < c4;
//        }

        return false;
    };

    sort(queryResult.begin(), queryResult.end(), compFunc);


    for (auto& op_col: op_columns) { // TODO: NOTE: only supports MAX on one column!
        if (op_col.first == "MAX") { // print max value in this col
            auto comp_func = [&](Record& r1, Record& r2) { return r1[op_col.second] < r2[op_col.second]; };
            Record maxRecord = *std::max_element(queryResult.begin(), queryResult.end(), comp_func);
            cout << "| ";
            cout << maxRecord[op_col.second] << endl;
            cout << "-------------------------------------------------------" << endl << endl;
            return;
        }
    }

    vector<string> distinct_cols;
    for (auto& op_col: op_columns) {
        if (op_col.first == "DISTINCT") { // print max value in this col
            distinct_cols.push_back(op_col.second);
        }
    }

    if (distinct_cols.size() > 0) {
        auto notSameFunc = [distinct_cols](Record r1, Record r2) {
            for (auto col: distinct_cols) {
                if (r1[col] == r2[col]) return false;
            }
            return true;
        };

        set<Record, decltype(notSameFunc)> mySet(notSameFunc);

        for (auto record: queryResult) {
            mySet.insert(record);
        }

        for (auto record: mySet) {
            if (record.validate()) {
                cout << "| ";
                for (auto& index: printIndex) {
                    cout << *record.cells[index] << " | ";
                }
                cout << endl;
            }
        }
        cout << "-------------------------------------------------------" << endl << endl;

        return;
    }

    for (auto & record: queryResult) {
        if (record.validate()) {
            cout << "| ";
            for (auto & index: printIndex) {
                cout << *record.cells[index] << " | ";
            }
            cout << endl;
        }
    }
    cout << "-------------------------------------------------------" << endl << endl;
}

void API::selectFromWithOrderBy(string &table, vector<pair<string, string>> &op_columns, vector<BaseQuery *> &Querys,
                                vector<pair<string, string>> &columnsToSort) {
    if (!tableMap.count(table)) {
        cout << "Error at select : Table " << table << " does not exist! " << endl;
        return;
    }

    // === print columns and get print vector ====
    // process * situation
    vector<int> printIndex;
    if (op_columns[0].second == "*") {
        op_columns.clear();
        for (auto & attr : tableMap[table].attributes) {
            op_columns.push_back({"NONE", attr.name});
        }
    }

    for (auto &op_col: op_columns) {
        size_t i;
        for (i = 0; i < tableMap[table].attributes.size(); ++i) {
            if (tableMap[table].attributes[i].name == op_col.second) {
                printIndex.push_back(i);
                break;
            }
        }
        if (i == tableMap[table].attributes.size()) {
            cout << "Error : Attribute " << op_col.second << " does not exist in table " << table << endl;
            return;
        }
    }
    cout << "-------------------------------------------------------" << endl;
    cout << "| ";
    for(auto& op_col: op_columns) {
        cout << op_col.second << " | ";
    }
    cout << endl << "-------------------------------------------------------" << endl;
    // ==== query and print result ===
    vector<Record> queryResult = recordManager.queryWithCondition(tableMap[table], Querys);

    auto compFunc = [&](Record& r1, Record& r2) {
//        auto c1 = make_tuple(r1[columnsToSort[0]]);
//        auto c2 = make_tuple(r2[columnsToSort[0]]);

        int i = 0;
        while (i < columnsToSort.size()) {
            if (r1[columnsToSort[i].first] == r2[columnsToSort[i].first]) {
                ++i;
                continue;
            }
            return columnsToSort[i].second == "ASC" ? r1[columnsToSort[i].first] < r2[columnsToSort[i].first] : r1[columnsToSort[i].first] > r2[columnsToSort[i].first];
        }

//        for (int i = 1; i < columnsToSort.size(); ++i) {
//            auto c3 = tuple_cat(c1, make_tuple(r1[columnsToSort[i]]));
//            auto c4 = tuple_cat(c2, make_tuple(r2[columnsToSort[i]]));
//            if (i == columnsToSort.size() - 1) return c3 < c4;
//        }

        return false;
    };

    sort(queryResult.begin(), queryResult.end(), compFunc);


    for (auto& op_col: op_columns) { // TODO: NOTE: only supports MAX on one column!
        if (op_col.first == "MAX") { // print max value in this col
            auto comp_func = [&](Record& r1, Record& r2) { return r1[op_col.second] < r2[op_col.second]; };
            Record maxRecord = *std::max_element(queryResult.begin(), queryResult.end(), comp_func);
            cout << "| ";
            cout << maxRecord[op_col.second] << endl;
            cout << "-------------------------------------------------------" << endl << endl;
            return;
        } else break;
    }

    vector<string> distinct_cols;
    for (auto& op_col: op_columns) {
        if (op_col.first == "DISTINCT") { // print max value in this col
            distinct_cols.push_back(op_col.second);
        } else break;
    }

    if (distinct_cols.size() > 0) {
        auto notSameFunc = [distinct_cols](Record r1, Record r2) {
            for (auto col: distinct_cols) {
                if (r1[col] == r2[col]) return false;
            }
            return true;
        };

        set<Record, decltype(notSameFunc)> mySet(notSameFunc);

        for (auto record: queryResult) {
            mySet.insert(record);
        }

        for (auto record: mySet) {
            if (record.validate()) {
                cout << "| ";
                for (auto& index: printIndex) {
                    cout << *record.cells[index] << " | ";
                }
                cout << endl;
            }
        }
        cout << "-------------------------------------------------------" << endl << endl;

        return;
    }

    for (auto & record: queryResult) {
        if (record.validate()) {
            cout << "| ";
            for (auto & index: printIndex) {
                cout << *record.cells[index] << " | ";
            }
            cout << endl;
        }
    }
    cout << "-------------------------------------------------------" << endl << endl;
}


int API::getAttributeType(string& table, string& attr) {
    auto attrs = tableMap[table].attributes;
    for (auto &curAttr : attrs ) {
        if (curAttr.name == attr) {
            return curAttr.type;
        }
    }
    return -1;
}


int API::getAttributeSize(string& table, string& attr) {
    auto attrs = tableMap[table].attributes;
    for (auto & curAttr : attrs ) {
        if (curAttr.name == attr) {
            return curAttr.size;
        }
    }
    return -1;
}

pair<Table, vector<Record>> API::joinTable(string &leftTableName, string &rightTableName, string &joinType,
                     vector<pair<string, string>> &onConditions) {
    vector<BaseQuery*> emptyVec;
    vector<Record> leftTableRecords = recordManager.queryWithCondition(tableMap[leftTableName], emptyVec);
    vector<Record> rightTableRecords = recordManager.queryWithCondition(tableMap[rightTableName], emptyVec);

    Table leftTable = tableMap[leftTableName];
    Table rightTable = tableMap[rightTableName];

    vector<TableAttribute> columns;

    for (auto& col: leftTable.attributes) {
        int size;
        switch (col.type) {
            case 0:
                size = sizeof(int);
                break;
            case 1:
                size = sizeof(float);
                break;
        }
        if (col.type <= 1) columns.push_back(TableAttribute(leftTableName + "." + col.name, col.type, size, col.isUnique));
        else columns.push_back(TableAttribute(leftTableName + "." + col.name, col.type, col.size, col.isUnique));
    }
    for (auto& col: rightTable.attributes) {
        int size;
        switch (col.type) {
            case 0:
                size = sizeof(int);
                break;
            case 1:
                size = sizeof(float);
                break;
        }
        if (col.type <= 1) columns.push_back(TableAttribute(rightTableName + "." + col.name, col.type, size, col.isUnique));
        else columns.push_back(TableAttribute(rightTableName + "." + col.name, col.type, col.size, col.isUnique));
    }

    string tableName = leftTableName + "-" + joinType + "-" + rightTableName;

    Table joinedTable(tableName, "", columns);

    tableMap[tableName] = joinedTable; // TODO: remeber to earse it after finish join querying!

    vector<Record> records;
    for (auto& leftRecord: leftTableRecords) {
        for (auto& rightRecord: rightTableRecords) {
            bool matched = true;
            for (auto& [col_l, col_r]: onConditions) {
                int leftIndex = col_l.find(".");
                string leftCol = col_l.substr(leftIndex + 1);
                int rightIndex = col_r.find(".");
                string rightCol = col_r.substr(rightIndex + 1);
                if (!(leftRecord[leftCol] == rightRecord[rightCol])) { // check "ON" conditions
                    matched = false;
                    break;
                }
            }
            if (!matched) continue;
            vector<TableCell*> cells = leftRecord.cells;
            cells.insert(cells.end(), rightRecord.cells.begin(), rightRecord.cells.end());
            Record curRecord(joinedTable, cells);
            records.push_back(curRecord);
        }
    }

    return make_pair(joinedTable, records);

}


void API::selectJoin(pair<Table, vector<Record>>& tableInfo, vector<pair<string, string>> &op_columns, QueryNode *root,
                     vector<pair<string, string>> &columnsToSort) {

    Table table = tableInfo.first;

    // === print columns and get print vector ====
    // process * situation
    vector<int> printIndex;
    if (op_columns[0].second == "*") {
        op_columns.clear();
        for (auto & attr : table.attributes) {
            op_columns.push_back({"NONE", attr.name});
        }
    }

    for (auto &op_col: op_columns) {
        size_t i;
        for (i = 0; i < table.attributes.size(); ++i) {
            if (table.attributes[i].name == op_col.second) {
                printIndex.push_back(i);
                break;
            }
        }
        if (i == table.attributes.size()) {
            cout << "Error : Attribute " << op_col.second << " does not exist in table " << table.name << endl;
            return;
        }
    }
    cout << "-------------------------------------------------------" << endl;
    cout << "| ";
    for(auto& op_col: op_columns) {
        cout << op_col.second << " | ";
    }
    cout << endl << "-------------------------------------------------------" << endl;
    // ==== query and print result ===
    // vector<Record> queryResult = recordManager.queryWithCondition(table, root);
    vector<Record> records = tableInfo.second;
    vector<Record> queryResult;

    if (root == NULL) { // no WHERE conditions
        queryResult = records;
    } else {
        // in-order traversal
        function<bool(QueryNode*, Record&)> eval = [&](QueryNode* node, Record& record) {
            if (!node) return false;
            if (node->left == nullptr && node->right == nullptr) return node->query->match(record);
            string op = node->op;
            bool leftRes = eval(node->left, record);
            bool rightRes = eval(node->right, record);
            if (strToLower(op) == "and") return leftRes && rightRes;
            if (strToLower(op) == "or") return leftRes || rightRes;
            return false;
        };

        for (Record& record: records) {
            bool matched = eval(root, record);
            if (matched) {
                queryResult.push_back(record);
            }
        }
    }

    auto compFunc = [&](Record& r1, Record& r2) {

        int i = 0;
        while (i < columnsToSort.size()) {
            if (r1[columnsToSort[i].first] == r2[columnsToSort[i].first]) {
                ++i;
                continue;
            }
            return columnsToSort[i].second == "ASC" ? r1[columnsToSort[i].first] < r2[columnsToSort[i].first] : r1[columnsToSort[i].first] > r2[columnsToSort[i].first];
        }

        return false;
    };

    sort(queryResult.begin(), queryResult.end(), compFunc);


    for (auto& op_col: op_columns) { // TODO: NOTE: only supports MAX on one column!
        if (op_col.first == "MAX") { // print max value in this col
            auto comp_func = [&](Record& r1, Record& r2) { return r1[op_col.second] < r2[op_col.second]; };
            Record maxRecord = *std::max_element(queryResult.begin(), queryResult.end(), comp_func);
            cout << "| ";
            cout << maxRecord[op_col.second] << endl;
            cout << "-------------------------------------------------------" << endl << endl;
            return;
        } else break;
    }

    vector<string> distinct_cols;
    for (auto& op_col: op_columns) {
        if (op_col.first == "DISTINCT") { // print max value in this col
            distinct_cols.push_back(op_col.second);
        } else break;
    }

    if (distinct_cols.size() > 0) {
        auto notSameFunc = [distinct_cols](Record r1, Record r2) {
            for (auto col: distinct_cols) {
                if (r1[col] == r2[col]) return false;
            }
            return true;
        };

        set<Record, decltype(notSameFunc)> mySet(notSameFunc);

        for (auto record: queryResult) {
            mySet.insert(record);
        }

        for (auto record: mySet) {
            if (record.validate()) {
                cout << "| ";
                for (auto& index: printIndex) {
                    cout << *record.cells[index] << " | ";
                }
                cout << endl;
            }
        }
        cout << "-------------------------------------------------------" << endl << endl;

        return;
    }

    for (auto & record: queryResult) {
        if (record.validate()) {
            cout << "| ";
            for (auto & index: printIndex) {
                cout << *record.cells[index] << " | ";
            }
            cout << endl;
        }
    }
    cout << "-------------------------------------------------------" << endl << endl;

    tableMap.erase(tableInfo.first.name);
}
