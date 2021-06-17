//
// Created by HaoTianXue on 2021/4/25.
//

# include "utils.h"
# include <sstream>
# include <regex>
using namespace std;


void tokenize(const string& str, const string& delims, vector<string>& tokens) {
    std::stringstream stringStream(str);
    std::string line;
    while(std::getline(stringStream, line)) {
        std::size_t prev = 0, pos;
        // only look for each char delimeter in delims
        while ((pos = line.find_first_of(delims, prev)) != std::string::npos) {
            if (pos > prev)
                tokens.push_back(line.substr(prev, pos-prev));
            tokens.push_back(line.substr(pos, 1));               // add delimiter
            prev = pos+1;
        }
        if (prev < line.length()) tokens.push_back(line.substr(prev, std::string::npos));
    }
}



vector<string> splitByStr(string s, string delimiter1, string delimiter2) {
    size_t pos = 0;
    string token;
    vector<string> res;

//    auto removeSpaces = [](string& token) {
//        token.erase(std::remove_if(token.begin(), token.end(), [](unsigned char x) { return std::isspace(x); }), token.end());
//    };

    while (true) {
        int pos1 = s.find(delimiter1);
        int pos2 = s.find(delimiter2);
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
            pos = min(pos1, pos2);
        } else if (pos1 != std::string::npos) {
            pos = pos1;
        } else if (pos2 != std::string::npos) {
            pos = pos2;
        } else {
            break;
        }
        token = s.substr(0, pos);
        // remove spaces
        // removeSpaces(token);
        token = regex_replace(token, std::regex("^ +| +$|( ) +"), "$1");
        vector<string> sub_tokens;
        tokenize(token, "()", sub_tokens); // "(a = 1" => ["(", "a = 1"]
        for (auto& t: sub_tokens) {
            if (t == " ") continue;
            res.push_back(t);
        }
        res.push_back((pos == pos1 ? delimiter1: delimiter2));
        s.erase(0, pos + (pos == pos1 ? delimiter1.length() : delimiter2.length()));
    }
    // removeSpaces(s);
    s = regex_replace(s, std::regex("^ +| +$|( ) +"), "$1");
    vector<string> sub_tokens;
    tokenize(s, "()", sub_tokens); // "(a=1" => ["(", "a=1"]
    for (auto& t: sub_tokens) {
        if (t == " ") continue;
        res.push_back(t);
    }
    return res;
}

