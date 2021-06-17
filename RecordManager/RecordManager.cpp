//
// Created by HaoTianXue on 2021/4/22.
//

#include "../stdafx.h"
#include "RecordManager.h"

void Record::addCell(TableCell* tableCell) {
    cells.push_back(tableCell);
}

ostream& operator<< (ostream& os, Record& record) {
    os << "| ";

    for_each(record.cells.begin(), record.cells.end(), [&](auto e) {
        os << *e << "  | ";
    });

    os << endl;
    return os;
}


void RecordManager::dropTable(string& tableName) {
    bufferManager.dropTableBuffer(tableName);
}


void RecordManager::initTable(Table& table) {
    bufferManager.createBufferForTable(table);
}


int RecordManager::insert(Record& record) {
    char recordBuffer[record.recordSize];
    int bufferPointer = 0;

    for (size_t attributeIter = 0;
         attributeIter < record.table.attributes.size();
         ++attributeIter) {
        char* dataBuffer =  record.cells[attributeIter] -> serialize();
        for (int dataPointer = 0;
             dataPointer < record.table.attributes[attributeIter].size;
             dataPointer++) {
            recordBuffer[bufferPointer++] = dataBuffer[dataPointer];
        }
    }

    return bufferManager.insertIntoTable(record.table.name, (char *)recordBuffer);
}


vector<Record> RecordManager::queryWithOffset(Table& table, vector<int>& offsets) {
    vector<Record> results;
    char* binaryResult;
    for (auto& offset : offsets) {
        binaryResult = bufferManager.queryTableWithOffset(table.name, offset);
        if (binaryResult) {
            Record retrivedRecord(table);
            int bufferPtr = 0;
            for(auto & attribute : table.attributes) {
                retrivedRecord.addCell(new TableCell(attribute.type, attribute.size, binaryResult + bufferPtr));
                bufferPtr += attribute.size;
            }
            results.push_back(retrivedRecord);
        }
    }
    return results;
}


vector<Record> RecordManager::queryWithCondition(Table& table, vector<BaseQuery*>& querys) {
    vector<Record> result;
    char* totalBuffer = bufferManager.queryCompleteTable(table.name);
    int totalBufferSize = bufferManager.getTableBufferSize(table.name);
    for (int bufferIter = 0;  bufferIter < totalBufferSize;  bufferIter += table.recordSize) {
        int elementPtr = 0;
        vector<TableCell*> elementsInRecord;
        for_each(table.attributes.begin(), table.attributes.end(), [&](auto attr) {
            // TODO: attr has index => direct return result on B+ tree
            elementsInRecord.push_back(new TableCell(attr.type, attr.size, totalBuffer + bufferIter + elementPtr));
            elementPtr += attr.size;
        });
        Record thisRecord(table, elementsInRecord);

        // clear out memory
        for (auto& ptr: elementsInRecord) {
            ptr = nullptr;
            delete ptr;
        }
        elementsInRecord.clear();
        elementsInRecord.shrink_to_fit();

        // handle all "and"
        bool matched = accumulate(querys.begin(), querys.end(), true, [&](bool matched, auto query) {
            return matched && query -> match(thisRecord);
        });
        if (matched) {
            result.push_back(thisRecord);
        }

    }
//    totalBuffer = nullptr;
//    delete totalBuffer;
    return result;
}


bool RecordManager::deleteWithOffset(string table, vector<int> offsets) {
    return accumulate(offsets.begin(), offsets.end(), true, [=](bool success, int offset) {
        return success && bufferManager.deleteFromTableWithOffset(table, offset);
    });
}


int RecordManager::deleteWithCondition(Table& table, vector<BaseQuery*>& querys) {
    int totalBufferSize = bufferManager.getTableBufferSize(table.name);
    int deleteCount = 0;
    for (int bufferIter = 0;  bufferIter < totalBufferSize;  bufferIter += table.recordSize) {
        bufferManager.deleteFromTableWithCheckFunc(table.name, [&](char* recordBuffer) {
            int elementPtr = 0;
            vector<TableCell*> elementsInRecord;
            for_each(table.attributes.begin(), table.attributes.end(), [&](auto attr) {
                elementsInRecord.push_back(new TableCell(attr.type, attr.size, recordBuffer + elementPtr));
                elementPtr += attr.size;
            });
            Record tempRecord(table, elementsInRecord);

            // clear out memory
            for (auto& ptr: elementsInRecord) {
                ptr = nullptr;
                delete ptr;
            }
            elementsInRecord.clear();
            elementsInRecord.shrink_to_fit();

            if (accumulate(querys.begin(), querys.end(), true, [&](bool matched, auto query) {
                return matched && query -> match(tempRecord);
            })) {
                ++deleteCount;
                return true;
            }

            return false;
        });
    }
    return deleteCount;
}


void RecordManager::writeBackAllToDisk() {
    bufferManager.writeBackAll();
}

// deal with select _ from _ where a and b or c ...
vector<Record> RecordManager::queryWithCondition(Table& table, QueryNode* root) {
    vector<Record> result;
    char* totalBuffer = bufferManager.queryCompleteTable(table.name);
    int totalBufferSize = bufferManager.getTableBufferSize(table.name);
    for (int bufferIter = 0;  bufferIter < totalBufferSize;  bufferIter += table.recordSize) {
        int elementPtr = 0;
        vector<TableCell*> elementsInRecord;
        for_each(table.attributes.begin(), table.attributes.end(), [&](auto attr) {
            elementsInRecord.push_back(new TableCell(attr.type, attr.size, totalBuffer + bufferIter + elementPtr));
            elementPtr += attr.size;
        });
        Record thisRecord(table, elementsInRecord);
        // in-order traversal
        function<bool(QueryNode*)> eval = [&](QueryNode* node) {
            if (!node) return false;
            if (node->left == nullptr && node->right == nullptr) return node->query->match(thisRecord);
            string op = node->op;
            bool leftRes = eval(node->left);
            bool rightRes = eval(node->right);
            if (strToLower(op) == "and") return leftRes && rightRes;
            if (strToLower(op) == "or") return leftRes || rightRes;
            return false;
        };
        bool matched = eval(root);

        if (matched) {
            result.push_back(thisRecord);
        }

        // clear out memory
        for (auto& ptr: elementsInRecord) {
            ptr = nullptr;
            delete ptr;
        }
        elementsInRecord.clear();
        elementsInRecord.shrink_to_fit();
    }

    return result;
}


int RecordManager::deleteWithCondition(Table& table, QueryNode* root) {
    int totalBufferSize = bufferManager.getTableBufferSize(table.name);
    int deleteCount = 0;
    for (int bufferIter = 0;  bufferIter < totalBufferSize;  bufferIter += table.recordSize) {
        bufferManager.deleteFromTableWithCheckFunc(table.name, [&](char* recordBuffer) {
            int elementPtr = 0;
            vector<TableCell*> elementsInRecord;
            for_each(table.attributes.begin(), table.attributes.end(), [&](auto attr) {
                elementsInRecord.push_back(new TableCell(attr.type, attr.size, recordBuffer + elementPtr));
                elementPtr += attr.size;
            });
            Record tempRecord(table, elementsInRecord);

            // in-order traversal
            function<bool(QueryNode*)> eval = [&](QueryNode* node) {
                if (!node) return false;
                if (node->left == nullptr && node->right == nullptr) return node->query->match(tempRecord);
                string op = node->op;
                bool leftRes = eval(node->left);
                bool rightRes = eval(node->right);
                if (strToLower(op) == "and") return leftRes && rightRes;
                if (strToLower(op) == "or") return leftRes || rightRes;
                return false;
            };
            bool matched = eval(root);

            // clear out memory
            for (auto& ptr: elementsInRecord) {
                ptr = nullptr;
                delete ptr;
            }
            elementsInRecord.clear();
            elementsInRecord.shrink_to_fit();

            if (matched) {
                ++deleteCount;
                return true;
            }
            return false;
        });
    }
    return deleteCount;
}
