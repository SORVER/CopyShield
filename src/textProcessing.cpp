#include<bits/stdc++.h>
#include "textProcessing.h"

using namespace std;

void RemoveComments(string& code) {
    // remove all comments
    code = regex_replace(code, regex("//.*"), "");
    code = regex_replace(code, regex("/\\*.*?\\*/"), "");
}

void TextProcessing(string& code) {
    // remove all comments
    RemoveComments(code);

    // remove all whitespace
    code.erase(remove(code.begin(), code.end(), ' '), code.end());
    code.erase(remove(code.begin(), code.end(), '\n'), code.end());
    code.erase(remove(code.begin(), code.end(), '\t'), code.end());

    // transform the code to lowercase
    transform(code.begin(), code.end(), code.begin(), ::tolower);
}

vector<std::string> splitCodeToWords(const string& input) {


    // cout << input << '\n';

    std::regex word_regex("(\\S+|\\s+)");
    std::sregex_iterator words_begin = std::sregex_iterator(input.begin(), input.end(), word_regex);
    std::sregex_iterator words_end = std::sregex_iterator();
    
    std::vector<std::string> words;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        // cout << i->str();
        words.push_back(i->str());
    }
    
    // for(auto& word : words){
    //     cout << word << '\n';
    // }


    return words;
}

std::vector<std::string> splitStringByDelimiter(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    // if(tokens.size() == 5){
    //     tokens[2] = tokens[2] + "_" + tokens[3];
    //     tokens.erase(tokens.begin() + 3);
    // }
    
    return tokens;
}
