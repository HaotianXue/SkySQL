//
// Created by HaoTianXue on 2021/4/22.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_BUFFERMANAGER_H
#define DATABASE_MANAGEMENT_SYSTEM_BUFFERMANAGER_H

#include "../stdafx.h"
#include "../base_element.h"

#define PAGE_SIZE (int)(4096 - sizeof(int))  // 4kb - size(int)


// class for single buffer page
class BufferPage {

private:
    string getPageFileName();

public:
    bool isDirty;
    int recordLength;
    int usedLength;
    int pageId;
    string table;
    string pageFileName;
    char data[PAGE_SIZE];

    BufferPage(string table, int recordLength, int pageId):
        isDirty(true),
        table(table),
        pageId(pageId),
        pageFileName(getPageFileName()),
        recordLength(recordLength),
        usedLength(0) {};

    inline bool isFull();

    bool reloadPage();

    bool writeBackPage();

    void dropPage();

    char* readRecordWithOffset(int offset);

    bool deleteWithOffset(int offset);

    bool deleteWithCheckFunc(function<bool(char*)> checkDeletionFunction);

    //return offset
    int insertDataToPage(char* newData);
};


// one table <=> one buffer pool
class BufferPool {

private:
    inline string getPageFileName(int id);

public:
    string table;
    int activePageId;
    int validPageLength;
    int recordLength;
    bool isDirty;
    vector<BufferPage*> pages;
    vector<bool> pageFull;

    BufferPool (Table& table);

//    ~BufferPool() {
//        for (auto ptr: pages) {
//            delete ptr;
//            ptr = nullptr;
//        }
//        pages.clear();
//        pageFull.clear();
//        table = "";
//    }

    void addNewBufferToPool();
    void dropBufferPool();

    bool reloadBuffer();
    bool writeBackBuffers();

    void findActivePageId();
    int insertData(char* newData);

    int getTotalBufferSize();

    char* queryAllData();
    char* queryWithOffset(int offset);

    bool deleteWithOffset(int offset);
    bool deleteWithCheckFunc(function<bool(char*)> checkDeletionFunction);
};


class BufferManager {

public:
    // key : table name
    // value : buffer pool for that table
    map<string, BufferPool*> bufferMap;

//    ~BufferManager() {
//        for (auto& [tableName, ptr]: bufferMap) {
//            delete ptr;
//        }
//        bufferMap.clear();
//    }

    void createBufferForTable(Table& table);
    void dropTableBuffer(string tableName);

    bool writeBackAll();
    bool writeBackTable(string table);
    bool reloadTable(string table);

    // return offset of the inserted data
    int insertIntoTable(string table, char* newData);
    char* queryTableWithOffset(string table, int offset);
    char* queryCompleteTable(string table);
    int getTableBufferSize(string table);

    bool deleteFromTableWithOffset(string table, int offset);
    bool deleteFromTableWithCheckFunc(string table, function<bool(char*)> checkDeletionFunction);
};


#endif //DATABASE_MANAGEMENT_SYSTEM_BUFFERMANAGER_H
