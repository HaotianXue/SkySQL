//
// Created by HaoTianXue on 2021/4/22.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_INDEXMANAGER_H
#define DATABASE_MANAGEMENT_SYSTEM_INDEXMANAGER_H

#include "../stdafx.h"
#include "../base_element.h"
#include "BPlusTree.h"

class IndexManager {
public:

    // key: (index's table name, index's attribute/column name)
    // value: B+ tree
    map<pair<string, string> , BTree<TableCell, int> > trees;

    void createIndex(Index& indexInfo);
    void dropIndex(Index& indexInfo);
    void dropAllIndexFromTable(string& tableName);
    void insertIntoIndex(Index& index, TableCell value, int offset);
    int queryFromIndex(string& table, string &attr, TableCell* cellPtr);
    void deleteFromIndex(Index& index, TableCell cell);


};


#endif //DATABASE_MANAGEMENT_SYSTEM_INDEXMANAGER_H
