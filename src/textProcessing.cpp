#include<bits/stdc++.h>
#include "../include/textProcessing.h"

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


string escapeHTML(string str, bool similar) {
    string newStr;
    
    for (char c : str) {
        if (c == '<') {
            newStr += "&lt;";
        } else if (c == '>') {
            newStr += "&gt;";
        } else if (c == '&') {
            newStr += "&amp;";
        } else {
            newStr += c;
        }
    }
    return similar ? highlightSimilar(newStr) : highlightAddedAndRemoved(newStr);   

    
}



string highlightSimilar(string str){
    // if found SIMILARFLAGBEGIN then add <span style="background-color: #b6dcfa"> and close it with SIMILARFLAGEND
    string result;



    for(int i = 0; i < str.size(); i++){
        if(str.substr(i, 16) == "SIMILARFLAGBEGIN"){
            result += "<span style='background-color: #b6dcfa'>";
            i += 15;
        } else if(str.substr(i, 14) == "SIMILARFLAGEND"){
            result += "</span>";
            i += 13;
        } else{
            result += str[i];
        }
    }
    return result;
}

string highlightAddedAndRemoved(string str){
    // if found ADDEDFLAGBEGIN then add <span style="background-color: #ccffcc"> and close it with ADDEDFLAGEND
    // if found REMOVEDFLAGBEGIN then add <span style="background-color: #ffcccc"> and close it with REMOVEDFLAGEND
    string result;

    for(int i = 0; i < str.size(); i++){
        if(str.substr(i, 14) == "ADDEDFLAGBEGIN"){
            result += "<span style='background-color: #ccffcc'>";
            i += 13;
        } else if(str.substr(i, 12) == "ADDEDFLAGEND"){
            result += "</span>";
            i += 11;
        } else if(str.substr(i, 16) == "REMOVEDFLAGBEGIN"){
            result += "<span style='background-color: #ffcccc'>";
            i += 15;
        } else if(str.substr(i, 14) == "REMOVEDFLAGEND"){
            result += "</span>";
            i += 13;
        } else{
            result += str[i];
        }
    }
    return result;
}


string GetDiff(vector<string> a, vector<string> b, vector<string> lcs){
    int i=0, j=0, k=0;
    string result;
    while(i < a.size() || j < b.size()){
        if(k < lcs.size() && i < a.size() && j < b.size() && a[i] == lcs[k] && b[j] == lcs[k]){
            result += a[i];
            i++;
            j++;
            k++;
        } else{
            if(i < a.size() && (k >= lcs.size() || a[i] != lcs[k])){
                result += "REMOVEDFLAGBEGIN" + a[i] + "REMOVEDFLAGEND"; 
                i++;
            }
            if(j < b.size() && (k >= lcs.size() || b[j] != lcs[k])){
                result += "ADDEDFLAGBEGIN" + b[j] + "ADDEDFLAGEND"; 
                j++;
            }
        }
    }

    return result;
}

string GetSimilarity(vector<string> a, vector<string> b, vector<string> lcs){
    int i=0, j=0, k=0;
    string result;
    while(i < a.size() || j < b.size()){
        if(k < lcs.size() && i < a.size() && j < b.size() && a[i] == lcs[k] && b[j] == lcs[k]){
            result += "SIMILARFLAGBEGIN" + a[i] + "SIMILARFLAGEND"; 
            i++;
            j++;
            k++;
        } else{
            if(i < a.size() && (k >= lcs.size() || a[i] != lcs[k])){
                result += a[i];
                i++;
            }
            if(j < b.size() && (k >= lcs.size() || b[j] != lcs[k])){
                // result += b[j];
                j++;
            }
        }
    }
    return result;
}