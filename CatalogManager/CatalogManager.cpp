//
// Created by HaoTianXue on 2021/4/22.
//
#include "../stdafx.h"
#include "CatalogManager.h"


string CatalogManager::getPrimaryKeyIndexName(Table& table) {
    return "___" + table.name + "__PRIK_INDX____";
}

bool CatalogManager::createTableCatalog(Table& tableInfo) {
    ofstream catalogFile;
    catalogFile.open(tableCatalogDir + tableInfo.name);
    catalogFile << tableInfo.name << endl;
    catalogFile << tableInfo.primaryKey << endl;
    catalogFile << tableInfo.attributes.size() << endl;
    cout << tableInfo.name << " " << tableInfo.primaryKey << " " << tableInfo.attributes.size() << endl;
    for (auto& attr: tableInfo.attributes ) {
        catalogFile << attr.name << '\t'
                    << attr.type << '\t'
                    << attr.size << '\t'
                    << (attr.isUnique ? '1' : '0') << endl;
    }
    catalogFile.close();

    ofstream tableInfoFile;
    tableInfoFile.open(tableInfoFilename, std::ios_base::app);
    tableInfoFile << tableInfo.name << endl;
    tableInfoFile.close();

    Index primaryIndexForTable(tableInfo.name, tableInfo.primaryKey, getPrimaryKeyIndexName(tableInfo));

    return catalogFile.good() && tableInfoFile.good() && createIndexCatalog(primaryIndexForTable);
}


bool CatalogManager::dropLineFromFile(string& filename, string& element, int elementCount) {
    string line;
    string tempFileName = filename + ".tmp";
    ifstream mainFile;
    ofstream tempFile;

    mainFile.open(filename);
    tempFile.open(tempFileName);

    while (getline(mainFile, line)) {
        istringstream lineStringStream(line);
        string StrToCmp;
        for (int i = 0; i < elementCount; ++i) {
            lineStringStream >> StrToCmp;
        }
        if (StrToCmp != element) {
            tempFile << line << endl;
        }
    }
    tempFile.close();
    mainFile.close();

    remove(filename.c_str());
    rename(tempFileName.c_str(), filename.c_str());

    return (tempFile.good());
}

bool CatalogManager::dropTableCatalog(string& tableName) {
    return dropLineFromFile(tableInfoFilename, tableName, 1)
           && !remove((tableCatalogDir + tableName).c_str())
           && dropIndexOfTable(tableName);
}

bool CatalogManager::createIndexCatalog(Index& indexInfo) {
    ofstream indexInfoCatalogFile;
    indexInfoCatalogFile.open(indexInfoCatalog, std::ios_base::app);
    indexInfoCatalogFile << indexInfo.indexName << " "
                         << indexInfo.tableName << " "
                         << indexInfo.attributeName << endl;
    indexInfoCatalogFile.close();
    return !indexInfoCatalogFile.fail();
}

bool CatalogManager::dropIndexCatalog(string& indexName) {
    return dropLineFromFile(indexInfoCatalog, indexName, 1);
}

bool CatalogManager::dropIndexOfTable(string& tableName) {
    return dropLineFromFile(indexInfoCatalog, tableName, 2);
}

bool CatalogManager::checkLineExistance(string& filename, string& lineToCheck) {
    ifstream fileToCheck;
    fileToCheck.open(filename);
    string line;
    while(getline(fileToCheck, line)) {
        cout << line << "  " << lineToCheck << '\n';
        if (line == lineToCheck) {
            fileToCheck.close();
            return true;
        }
    }
    fileToCheck.close();
    return false;
}

bool CatalogManager::checkTableExistance(string tableName) {
    return checkLineExistance(tableInfoFilename, tableName);
}


Index CatalogManager::getIndexInfo(string& indexFileLine) {
    string indexLine;
    string _tableName, _indexName, _attrName;
    ifstream indexInfoCatalogFile;
    indexInfoCatalogFile.open(indexInfoCatalog);
    while(getline(indexInfoCatalogFile, indexLine)) {
        istringstream indexLineStream(indexLine);
        indexLineStream >> _indexName >> _tableName >> _attrName;
        // if (indexName == _indexName) {
        if (indexFileLine == indexLine) {
            indexInfoCatalogFile.close();
            return Index(_tableName, _attrName, _indexName);
        }
    }
    indexInfoCatalogFile.close();
    return Index(""); // not available index info
}

Index CatalogManager::getIndexInfo(string& tableName, string& attributeName) {
    string indexLine;
    string _tableName, _indexName, _attrName;
    ifstream indexInfoCatalogFile;
    indexInfoCatalogFile.open(indexInfoCatalog);
    while(getline(indexInfoCatalogFile, indexLine)) {
        istringstream indexLineStream(indexLine);
        indexLineStream >> _indexName >> _tableName >> _attrName;
        if (tableName == _tableName && attributeName == _attrName) {
            return Index(_tableName, _attrName, _indexName);
        }
    }
    return Index(""); // not available index obj
}

bool CatalogManager::checkIndexExistanceWithName(string indexName) {
    return getIndexInfo(indexName).isValid();
}

bool CatalogManager::checkIndexExistanceWithAttr(string tableName, string attributeName) {
    return getIndexInfo(tableName, attributeName).isValid();
}

bool CatalogManager::checkAttributeUnique(string tableName, string attributeName) {
    Table tableInfo = getTableInfo(tableName);
    for (auto& attr : tableInfo.attributes ) {
        if (attr.name == attributeName) {
            return attr.isUnique;
        }
    }
    return false;
}


Table CatalogManager::getTableInfo(string& tableName) {
    string _table, _primaryKey, _attr;
    int attrCount, _type, _size;
    bool _unique;
    vector<TableAttribute> _attributes;

    ifstream catalogFile;
    catalogFile.open(tableCatalogDir + tableName);
    catalogFile >> _table >> _primaryKey >> attrCount;
    for (int i = 0; i < attrCount; ++i) {
        catalogFile >> _attr >> _type >> _size >> _unique;
        _attributes.push_back(TableAttribute(_attr, _type, _size, _unique));
    }
    catalogFile.close();
    Table returnTable(_table, _primaryKey, _attributes);
    return returnTable;
}


map<string, Table> CatalogManager::initializeTables() {
    map<string, Table> result;

    ifstream tableListFile;
    tableListFile.open(tableInfoFilename);
    string tableFileLine;
    while(getline(tableListFile, tableFileLine)) {
        if (tableFileLine.empty()) continue;
        Table table = getTableInfo(tableFileLine);
        result.insert(pair<string, Table> (table.name, table));
    }
    tableListFile.close();
    return result;
}

map<string, Index> CatalogManager::initializeIndexes() {
    map<string, Index> result;

    ifstream indexListFile;
    indexListFile.open(indexInfoCatalog);
    string indexFileLine;
    while(getline(indexListFile, indexFileLine)) {
        if (indexFileLine.empty()) continue;
        Index index = getIndexInfo(indexFileLine);
        result.insert(pair<string, Index> (index.indexName, index));
    }
    indexListFile.close();
    return result;
}

