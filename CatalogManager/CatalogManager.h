//
// Created by HaoTianXue on 2021/4/22.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_CATALOGMANAGER_H
#define DATABASE_MANAGEMENT_SYSTEM_CATALOGMANAGER_H

#include "../stdafx.h"
#include "../base_element.h"

class CatalogManager {

private:

    string tableCatalogDir = "../data/catalog/";
    string tableInfoFilename = "../data/table.cat";
    string indexInfoCatalog = "../data/index.cat";
    string indexDir;

    bool dropLineFromFile(string& filename, string& element, int elementCount);
    bool checkLineExistance(string& filename, string& lineToCheck);

public:

    string getPrimaryKeyIndexName(Table& table);

    bool createTableCatalog(Table& tableInfo);
    bool dropTableCatalog(string& tableName);
    bool createIndexCatalog(Index& indexInfo);
    bool dropIndexCatalog(string& indexName);
    bool dropIndexOfTable(string& tableName);

    bool checkTableExistance(string tableName);
    bool checkIndexExistanceWithName(string indexName);
    bool checkIndexExistanceWithAttr(string tableName, string attributeName);
    bool checkAttributeUnique(string tableName, string attributeName);

    Table getTableInfo(string& tableName);
    Index getIndexInfo(string&indexName);
    Index getIndexInfo(string& tableName, string& attributeName);

    map<string, Table> initializeTables();
    map<string, Index> initializeIndexes();

};


#endif //DATABASE_MANAGEMENT_SYSTEM_CATALOGMANAGER_H
