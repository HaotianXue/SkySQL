//
// Created by HaoTianXue on 2021/4/22.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_BASE_ELEMENT_H
#define DATABASE_MANAGEMENT_SYSTEM_BASE_ELEMENT_H

#include "stdafx.h"

#define INT_SIZE    (int)sizeof(int)
#define FLOAT_SIZE  (int)sizeof(float)

#define INVALID_CHAR_DATA 127
#define INVALID_INT_DATA (int)0x7F7F7F7F

enum AttributeType { INT, FLOAT, STRING };


class TableAttribute { // column info

public:
    int type;
    int size;
    string name;
    bool isUnique;

    TableAttribute(string name, string typeStr, bool isUnique) {
        this->name = name;
        this->isUnique = isUnique;
        if (typeStr.size() == 3) {
            this->type = INT;
            this->size = sizeof(int);
        }
        if (typeStr.size() == 5) {
            this->type = FLOAT;
            this->size = sizeof(float);
        }
    }

    TableAttribute(string name, int type, int size, bool isUnique):
        type(type), name(name), size(size), isUnique(isUnique) {};

    TableAttribute(string name, int size, bool isUnique):
        type(STRING), name(name), size(size), isUnique(isUnique) {};

};


class Table {

public:
    string name;
    string primaryKey;
    vector<TableAttribute> attributes;
    int recordSize; // size of one single row

    Table() {};

    ~Table() {
        name.clear();
        primaryKey.clear();
        attributes.clear();
        attributes.shrink_to_fit();
    }

    Table(string name, string primaryKey, vector<TableAttribute> attributes):
        name(name),
        primaryKey(primaryKey),
        attributes(attributes),
        recordSize(accumulate(attributes.begin(),
                              attributes.end(),
                              0,
                              [](int size, TableAttribute attribute) { return size + attribute.size; })
                  ) {};
};


class Index {

public:
    string indexName;
    string tableName;
    string attributeName;
    int recordSize;

    Index(string indexName): indexName(indexName) {};

    Index(string tableName, string attributeName):
        tableName(tableName), attributeName(attributeName) {};

    Index(string tableName, string attributeName, string indexName):
        tableName(tableName), attributeName(attributeName), indexName(indexName) {};

    bool isValid() {
        return indexName != "";
    }
};


class TableCell {

public:
    int type;
    int size;
    int intValue;
    float floatValue;
    string stringValue;
    bool isValid = true;

    TableCell(int type, int size, char* sourceBit): type(type), size(size) {
        deserialize(sourceBit);
    };

    ~TableCell() {
        stringValue.clear();
        isValid = true;
    };

    TableCell() {};

    TableCell(int value): intValue(value), type(INT), size(sizeof(value)) {};

    TableCell(float value): floatValue(value), type(FLOAT), size(sizeof(value)) {};

    TableCell(string str): stringValue(str), type(STRING), size(str.size()) {};

    void resize(int newSize) {
        size = newSize;
    }

    void reinterpret(int newType) { // convert to newType
        switch(newType) {
            case INT:
                type = INT;
                intValue = stoi(stringValue);
                size = sizeof(int);
                return;
            case FLOAT:
                type = FLOAT;
                floatValue = stof(stringValue);
                size = sizeof(float);
                return;
        }
    }

    char* serialize() { // get bit to buffer
        switch (type) {
            case INT:
                return reinterpret_cast<char *> (&intValue);
            case FLOAT:
                return reinterpret_cast<char *> (&floatValue);
            case STRING:
                return (char *)stringValue.c_str();
        }
        return nullptr;
    }

    void deserialize(char* sourceBit) { // retrive from bit
        if (*(int *)sourceBit == INVALID_INT_DATA) {
            isValid = false;
            return;
        }
        switch(type) {
            case INT:
                copy(sourceBit, sourceBit + 4, reinterpret_cast<char *> (&intValue));
                break;
            case FLOAT:
                copy(sourceBit, sourceBit + 4, reinterpret_cast<char *> (&floatValue));
                break;
            case STRING:
                stringValue = string(sourceBit);
                stringValue.resize(size);
        }
    };

    friend ostream & operator << (ostream& os, const TableCell& cell);

    friend bool operator < (const TableCell& cell1, const TableCell& cell2);
    friend bool operator > (const TableCell& cell1, const TableCell& cell2);
    friend bool operator <= (TableCell& cell1, TableCell& cell2);
    friend bool operator >= (TableCell& cell1, TableCell& cell2);
    friend bool operator == (const TableCell& cell1, const TableCell& cell2);
    friend bool operator != (TableCell& cell1, TableCell& cell2);
};


#endif //DATABASE_MANAGEMENT_SYSTEM_BASE_ELEMENT_H
