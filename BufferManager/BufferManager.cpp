//
// Created by HaoTianXue on 2021/4/22.
//
#include "../stdafx.h"
#include "BufferManager.h"

// =============== BufferPage Implementation ==============

inline string BufferPage::getPageFileName() {
    return "../data/record/_TBL_" + table + "__PageID_" + to_string(pageId) + "___";
}

inline bool BufferPage::isFull() {
    return (usedLength + recordLength) >= PAGE_SIZE;
}

void BufferPage::dropPage() {
    remove(pageFileName.c_str()); // delete file
}

bool BufferPage::writeBackPage() {
    if (isDirty) { // page is changed
        ofstream pageWriteStream;
        pageWriteStream.open(pageFileName, ios::out | ios::binary);
        // write the used page first.
        pageWriteStream.write(reinterpret_cast<const char *> (&usedLength), sizeof(int));
        pageWriteStream.write(data, PAGE_SIZE);
        pageWriteStream.close();
        isDirty = pageWriteStream.bad();
        return pageWriteStream.good();
    }
    // else the page is not changed
    return true;
}

bool BufferPage::reloadPage() {
    isDirty = false;
    ifstream pageReadStream;
    pageReadStream.open(pageFileName, ios::in | ios::binary);
    pageReadStream.read(reinterpret_cast<char *> (&usedLength), sizeof(int));
    pageReadStream.read(data, PAGE_SIZE);
    pageReadStream.close();
    return pageReadStream.good();
}

int BufferPage::insertDataToPage(char* newData) { // return offset
    if (isFull()) {
        return -1;
    } else {
        for (int i = 0; i < recordLength; ++i) {
            data[usedLength + i] = newData[i];
        }
        usedLength += recordLength;
        isDirty = true;
        return usedLength - recordLength;
    }
}

char* BufferPage::readRecordWithOffset(int offset) {
    return data + offset;
}

bool BufferPage::deleteWithOffset(int offset) {
    if (recordLength + offset > PAGE_SIZE) {
        return false;
    } else {
        for (int i = 0; i < recordLength; ++i) {
            data[offset + i] = INVALID_CHAR_DATA;
        }
    }
    return true;
}

bool BufferPage::deleteWithCheckFunc(function<bool(char* )> checkDeletionFunction) {
    for (int offset = 0; offset < usedLength; offset += recordLength) {
        if (checkDeletionFunction(data + offset)) {
            deleteWithOffset(offset);
        }
    }
    return true;
}


// =============== BufferPool Implementation ====================

BufferPool::BufferPool(Table& table):
    table(table.name), recordLength(table.recordSize) {

    activePageId = 0;
    validPageLength = PAGE_SIZE - (PAGE_SIZE % recordLength);
    reloadBuffer();
    if (pages.size() == 0) {
        addNewBufferToPool();
    }
    findActivePageId();
}

inline string BufferPool::getPageFileName(int id) {
    return "../data/record/_TBL_" + table + "__PageID_" + to_string(id) + "___";
}

void BufferPool::addNewBufferToPool() {
    pages.push_back(new BufferPage(table, recordLength, pages.size()));
    pageFull.push_back(false);
    isDirty = true;
}

void BufferPool::dropBufferPool() {
    for_each(pages.begin(), pages.end(), [](auto page) {
        page -> dropPage();
        delete page;
    });
}

bool BufferPool::reloadBuffer() {

    pages.clear();
    pageFull.clear();
    int curId = 0;

    while(checkFileExistance(getPageFileName(curId))) {
        BufferPage* curPage = new BufferPage(table, recordLength, curId);
        if(curPage -> reloadPage()) {
            pages.push_back(curPage);
            pageFull.push_back(curPage -> isFull());
            if (activePageId < 0 && (!(curPage -> isFull()))) {
                activePageId = curId;
            }
            ++curId;
        } else {
            return false;
        }
    }
    isDirty = false;
    return true;
}

bool BufferPool::writeBackBuffers() {
    if (isDirty) {
        bool noError = all_of(pages.begin(), pages.end(),
                              [](auto page)
                              {
                                  return page -> writeBackPage();
                              });
        isDirty = !noError;
        return noError;
    }
    return true;
}

void BufferPool::findActivePageId() {
    if (pageFull[activePageId]) {
        for (int i = 0; i < pageFull.size(); ++i) {
            if (!pageFull[i]) {
                activePageId = i;
                return;
            }
        }
        addNewBufferToPool();
        activePageId = pages.size() - 1;
    }
}

int BufferPool::insertData(char* newData) {
    int pageOffset = pages[activePageId] -> insertDataToPage(newData);
    int totalOffset = activePageId * PAGE_SIZE + pageOffset;
    pageFull[activePageId] = pages[activePageId] -> isFull();
    findActivePageId();
    isDirty = true;
    return totalOffset;
}

char * BufferPool::queryWithOffset(int offset) {
    int id = offset / PAGE_SIZE;
    int pageOffset = offset % PAGE_SIZE;
    return pages[id] -> readRecordWithOffset(pageOffset);
}

int BufferPool::getTotalBufferSize() {
    return accumulate(pages.begin(), pages.end(), 0,
                      [](int sum, auto page)
                      {
                          return sum + page -> usedLength;
                      });
}

char* BufferPool::queryAllData() {
    char* returnData = new char[getTotalBufferSize()];
    int dataPtr = 0;
    int pagePtr;
    for (auto const & page : pages) {
        pagePtr = 0;
        while(pagePtr < page -> usedLength) {
            returnData[dataPtr++] = page -> data[pagePtr++];
        }
    }
    return returnData;
}

bool BufferPool::deleteWithOffset(int offset) {
    int id = offset / PAGE_SIZE;
    int pageOffset = offset % PAGE_SIZE;
    if (id < pages.size()) {
        return pages[id] -> deleteWithOffset(pageOffset);
    }
    return false;
}

bool BufferPool::deleteWithCheckFunc(function<bool(char* )> checkDeletionFunction) {
    for_each(pages.begin(), pages.end(), [=](auto page) {
        page -> deleteWithCheckFunc(checkDeletionFunction);
    });
    return true;
}


// ================= BufferManager Implementation ======================

void BufferManager::createBufferForTable(Table& table) {
    // if (bufferMap.find(table.name) == bufferMap.end()) {
    if (!bufferMap.count(table.name)) {
        auto newPool = new BufferPool(table);
        bufferMap[table.name] = newPool;
    }
}

void BufferManager::dropTableBuffer(string tableName) {
    // if (bufferMap.find(tableName) != bufferMap.end()) {
    if (bufferMap.count(tableName)) {
        bufferMap[tableName] -> dropBufferPool();
        delete bufferMap[tableName];
        bufferMap.erase(tableName);
    }
}

bool BufferManager::writeBackTable(string table) { // write back to disk
    // if (bufferMap.find(table) != bufferMap.end()) {
    if (bufferMap.count(table)) {
        return bufferMap[table] -> writeBackBuffers();
    }
    return false;
}

bool BufferManager::reloadTable(string table) {
    // if (bufferMap.find(table) != bufferMap.end()) {
    if (bufferMap.count(table)) {
        return bufferMap[table] -> reloadBuffer();
    }
    return false;
}

bool BufferManager::writeBackAll() {
    return all_of(bufferMap.begin(), bufferMap.end(),
                  [](auto bufferMapIter) {
                      return bufferMapIter.second -> writeBackBuffers();
                  });
}

int BufferManager::insertIntoTable(string table, char* newData) {
    // if (bufferMap.find(table) != bufferMap.end()) {
    if (bufferMap.count(table)) {
        return bufferMap[table] -> insertData(newData);
    }
    return -1;
}

char * BufferManager::queryTableWithOffset(string table, int offset) {
    // if (bufferMap.find(table) != bufferMap.end()) {
    if (bufferMap.count(table)) {
        return bufferMap[table] -> queryWithOffset(offset);
    }
    return nullptr;
}

char* BufferManager::queryCompleteTable(string table) {
    // if (bufferMap.find(table) != bufferMap.end()) {
    if (bufferMap.count(table)) {
        return bufferMap[table] -> queryAllData();
    }
    return nullptr;
}

int BufferManager::getTableBufferSize(string table) {
    // if (bufferMap.find(table) != bufferMap.end()) {
    if (bufferMap.count(table)) {
        return bufferMap[table] -> getTotalBufferSize();
    }
    return 0;
}

bool BufferManager::deleteFromTableWithOffset(string table, int offset) {
    // if (bufferMap.find(table) != bufferMap.end()) {
    if (bufferMap.count(table)) {
        return bufferMap[table] -> deleteWithOffset(offset);
    }
    return false;
}

bool BufferManager::deleteFromTableWithCheckFunc(string table, function<bool(char *)> checkDeletionFunction) {
    // if (bufferMap.find(table) != bufferMap.end()) {
    if (bufferMap.count(table)) {
        return bufferMap[table] -> deleteWithCheckFunc(checkDeletionFunction);
    }
    return false;
}


