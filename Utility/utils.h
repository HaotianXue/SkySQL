//
// Created by HaoTianXue on 2021/4/22.
//

#ifndef DATABASE_MANAGEMENT_SYSTEM_UTILS_H
#define DATABASE_MANAGEMENT_SYSTEM_UTILS_H

#include <vector>
#include <string>

#include <fstream>
#include <iostream>

#include <unistd.h>

using namespace std;

template<typename T>
vector<T> split(const T & str, const T & delimiters) {
    vector<T> v;
    typename T::size_type start = 0;
    auto pos = str.find_first_of(delimiters, start);
    while(pos != T::npos) {
        if(pos != start) // ignore empty tokens
            v.emplace_back(str, start, pos - start);
        start = pos + 1;
        pos = str.find_first_of(delimiters, start);
    }
    if(start < str.length()) // ignore trailing delimiter
        v.emplace_back(str, start, str.length() - start); // add what's left of the string
    return v;
}

inline string strToLower(string str) {
    transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return tolower(c); });
    return str;
}


inline bool checkFileExistance(string filename) {
    return (access(filename.c_str(), F_OK) != -1);
}

void tokenize(const string& str, const string& delims, vector<string>& tokens);

vector<string> splitByStr(string s, string delimiter1, string delimiter2);

template <typename NewElem, typename... TupleElem>
std::tuple<TupleElem..., NewElem> tuple_append(const std::tuple<TupleElem...> &tup, const NewElem &el) {
    return std::tuple_cat(tup, std::make_tuple(el));
}


#endif //DATABASE_MANAGEMENT_SYSTEM_UTILS_H
