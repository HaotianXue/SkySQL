//
// Created by HaoTianXue on 2021/4/22.
//
#include "IndexManager.h"


void IndexManager::createIndex(Index& indexInfo) {
    // if (trees.find(pair<string, string>(indexInfo.tableName, indexInfo.attributeName)) == trees.end()) {
    if (!trees.count(pair<string, string>(indexInfo.tableName, indexInfo.attributeName))) {
        trees[pair<string, string>(indexInfo.tableName, indexInfo.attributeName)] = BTree<TableCell, int>();
    }
}

void IndexManager::dropIndex(Index& indexInfo) {
    // if (trees.find(pair<string, string>(indexInfo.tableName, indexInfo.attributeName)) != trees.end()) {
    if (trees.count(pair<string, string>(indexInfo.tableName, indexInfo.attributeName))) {
        trees.erase(pair<string, string>(indexInfo.tableName, indexInfo.attributeName));
    }
}

void IndexManager::dropAllIndexFromTable(string& tableName) {
    for (auto treeIter : trees) {
        if (treeIter.first.first == tableName) {
            trees.erase(treeIter.first);
        }
    }
}

void IndexManager::insertIntoIndex(Index& index, TableCell value, int offset) {
    // if (trees.find(pair<string, string>(index.tableName, index.attributeName)) != trees.end()) {
    if (trees.count(pair<string, string>(index.tableName, index.attributeName))) {
        trees[pair<string, string>(index.tableName, index.attributeName)].Insert(value, offset);
    }
}

int IndexManager::queryFromIndex(string& table, string& attr, TableCell* cellPtr) {
    // if (trees.find(pair<string, string>(table, attr)) != trees.end()) {
    if (trees.count(pair<string, string>(table, attr))) {
        return trees[pair<string, string>(table, attr)][*cellPtr];
    }
    return -1;
}

void IndexManager::deleteFromIndex(Index& index, TableCell value) {
    // if (trees.find(pair<string, string>(index.tableName, index.attributeName)) != trees.end()) {
    if (trees.count(pair<string, string>(index.tableName, index.attributeName))) {
        trees[pair<string, string>(index.tableName, index.attributeName)].Delete(value);
    }
}

