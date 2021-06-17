//
// Created by HaoTianXue on 2021/4/26.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_BOOLEANQUERYNODE_H
#define DATABASE_MANAGEMENT_SYSTEM_BOOLEANQUERYNODE_H

// construct a query tree of operations "AND" and "OR"
// grammar: (use recursive descent parsing)
// s = expression
// expression = term | term {or term}
// term = factor | factor {and factor}
// factor = statement(type::string) | '('expression')'
class QueryNode {
public:
public:
    string op;
    BaseQuery* query;
    QueryNode* left;
    QueryNode* right;

    QueryNode() {}

    QueryNode(BaseQuery* x) : op(""), query(x), left(nullptr), right(nullptr) {}

    QueryNode(string x, QueryNode* left, QueryNode* right) : op(x), query(nullptr), left(left), right(right) {}

    ~QueryNode() {
        op.clear();
        // delete query;
        delete left;
        delete right;
        query = nullptr;
        left = nullptr;
        right = nullptr;
    }
};


#endif //DATABASE_MANAGEMENT_SYSTEM_BOOLEANQUERYNODE_H
