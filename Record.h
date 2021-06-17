//
// Created by HaoTianXue on 2021/4/22.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_RECORD_H
#define DATABASE_MANAGEMENT_SYSTEM_RECORD_H

# include "stdafx.h"
# include "base_element.h"

class Record {

public:

    Table table;
    vector<TableCell*> cells;
    int recordSize;

    Record(Table table):
        table(table), recordSize(table.recordSize) {};

    Record(Table table, vector<TableCell*> cells):
        table(table), cells(cells), recordSize(table.recordSize) {};

//    ~Record() {
//        for (auto& ptr: cells) {
//            delete ptr;
//            ptr = nullptr;
//        }
//        cells.clear();
//        cells.shrink_to_fit();
//    }

    void addCell(TableCell* tableCell);

    friend ostream& operator<< (ostream& os, Record& record);

    bool validate() { // check if one of cells are valid
        return accumulate(cells.begin(),
                          cells.end(),
                          false,
                          [](bool isValid, auto cellPtr) { return (isValid || cellPtr -> isValid); }
                          );
    };

    TableCell operator[] (string attributeName) {
        for (size_t i = 0; i < cells.size(); ++i) {
            if (table.attributes[i].name == attributeName) {
                return *cells[i];
            }
        }
        return TableCell();
    }
};

#endif //DATABASE_MANAGEMENT_SYSTEM_RECORD_H
