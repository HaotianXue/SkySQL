//
// Created by HaoTianXue on 2021/4/22.
//
# include "stdafx.h"
# include "Record.h"
# include "Query.h"


template
bool SingleQuery<int>::match(Record record);
template
bool SingleQuery<float>::match(Record record);
template
bool SingleQuery<string>::match(Record record);

template
bool RangeQuery<int>::match(Record record);
template
bool RangeQuery<float>::match(Record record);
template
bool RangeQuery<string>::match(Record record);

template
bool infinityRangeQuery<int>::match(Record record);
template
bool infinityRangeQuery<float>::match(Record record);
template
bool infinityRangeQuery<string>::match(Record record);


template <class T>
bool SingleQuery<T>::match(Record record) {

    TableCell elementToCompare(targetData);

    for (size_t attrIter = 0; attrIter < record.cells.size(); ++attrIter) {
        if (record.table.attributes[attrIter].name == attributeName) {
            if (*(int*)(record.cells[attrIter]) == INVALID_INT_DATA) return false;

            return ((*record.cells[attrIter]) == elementToCompare) ^ matchUnEqual;
        }
    }
    return false;
}


template <class T>
bool RangeQuery<T>::match(Record record) {

    TableCell minElement(minData);
    TableCell maxElement(maxData);

    for (size_t attrIter = 0; attrIter < record.cells.size(); ++attrIter) {
        if (record.table.attributes[attrIter].name == attributeName) {
            if (*(int*)(record.cells[attrIter]) == INVALID_INT_DATA) return false;

            TableCell thisElement = *record.cells[attrIter];
            switch((minIncluded ? 2 : 0) + (maxIncluded ? 1 : 0)) {
                case 3 : return thisElement >= minElement && thisElement <= maxElement;
                case 2 : return thisElement >= minElement && thisElement <  maxElement;
                case 1 : return thisElement >  minElement && thisElement <= maxElement;
                case 0 : return thisElement >  minElement && thisElement <  maxElement;
            }
        }
    }
    return false;
}


template <class T>
bool infinityRangeQuery<T>::match(Record record) {

    TableCell elementToCompare(seperateData);

    for (size_t attrIter = 0; attrIter < record.cells.size(); ++attrIter) {
        if (record.table.attributes[attrIter].name == attributeName) {
            if ( *(int*)(record.cells[attrIter]) == INVALID_INT_DATA) return false;

            TableCell thisElement = *record.cells[attrIter];
            switch ((queryGreaterThan ? 2 : 0) + (sepInlcluded ? 1 : 0)) {
                case 3 : return thisElement >= elementToCompare;
                case 2 : return thisElement >  elementToCompare;
                case 1 : return thisElement <= elementToCompare;
                case 0 : return thisElement <  elementToCompare;
            }
        }
    }
    return false;
}
