//
// Created by HaoTianXue on 2021/4/22.
//
#include "base_element.h"

ostream& operator << (ostream& os, const TableCell & cell) {
    switch (cell.type) {
        case INT:    os << cell.intValue;     return os;
        case FLOAT:  os << cell.floatValue;   return os;
        case STRING: os << cell.stringValue;  return os;
    }
    return os;
}

bool operator < (const TableCell& cell1, const TableCell& cell2) {
    switch (3 * cell1.type + cell2.type) {
        case 0 : return cell1.intValue < cell2.intValue;
        case 1 : return cell1.intValue < cell2.floatValue;
        case 2 : return false;
        case 3 : return cell1.floatValue < cell2.intValue;
        case 4 : return cell1.floatValue < cell2.floatValue;
        case 5 : return false;
        case 6 : return false;
        case 7 : return false;
        case 8 : return cell1.stringValue < cell2.stringValue;
    }
    return false;
}

bool operator > (const TableCell& cell1, const TableCell& cell2) {
    return cell2 < cell1;
}

bool operator <= (TableCell& cell1, TableCell& cell2) {
    return !(cell1 > cell2);
}

bool operator >= (TableCell& cell1, TableCell& cell2) {
    return !(cell1 < cell2);
}

bool operator == (const TableCell& cell1, const TableCell& cell2) {
    return (!(cell1 > cell2)) && (!(cell1 < cell2));
}

bool operator != (TableCell& cell1, TableCell& cell2) {
    return !(cell1 == cell2);
}
