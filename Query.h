//
// Created by HaoTianXue on 2021/4/22.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_QUERY_H
#define DATABASE_MANAGEMENT_SYSTEM_QUERY_H

# include "stdafx.h"
# include "base_element.h"
# include "Record.h"


class BaseQuery {

protected:
    string attributeName;

public:

    BaseQuery(string attributeName): attributeName(attributeName) {};

    // virtual ~BaseQuery() = 0;

    virtual bool match(Record record) = 0; // pure virtual function

};


template<class T>
class SingleQuery: public BaseQuery {

public:
    T targetData;
    bool matchUnEqual;

    SingleQuery(string attributeName, T data, bool unEqual=false):
        BaseQuery(attributeName),  targetData(data), matchUnEqual(unEqual) {};

    // ~SingleQuery() {};

    bool match(Record record);

};


template <class T>
class RangeQuery : public BaseQuery {
public:
    T minData;
    T maxData;
    bool minIncluded;
    bool maxIncluded;

    RangeQuery(string attrName, T minData, T maxData, bool minInc, bool maxInc):
        BaseQuery(attrName), minData(minData), maxData(maxData), minIncluded(minInc), maxIncluded(maxInc) {};

    // ~RangeQuery() {};

    bool match(Record record);

};


template <class T>
class infinityRangeQuery : public BaseQuery {
public:
    T seperateData;
    bool sepInlcluded;
    bool queryGreaterThan;

    infinityRangeQuery(string attrName, T seperateData, bool sepInlcluded, bool queryGreaterThan):
        BaseQuery(attrName), seperateData(seperateData), sepInlcluded(sepInlcluded), queryGreaterThan(queryGreaterThan) {};

    infinityRangeQuery(string attrName, T seperateData, string comparator):
        BaseQuery(attrName), seperateData(seperateData) {

        if (comparator == ">") {
            sepInlcluded = false;
            queryGreaterThan = true;
        }
        if (comparator == "<") {
            sepInlcluded = false;
            queryGreaterThan = false;
        }
        if (comparator == ">=") {
            sepInlcluded = true;
            queryGreaterThan = true;
        }
        if (comparator == "<=") {
            sepInlcluded = true;
            queryGreaterThan = false;
        }

    };

    // ~infinityRangeQuery() {};

    bool match(Record record);
};


#endif //DATABASE_MANAGEMENT_SYSTEM_QUERY_H
