#pragma once 

#include <string>
#include <vector>


using namespace std;

void RemoveComments(string& code);
void TextProcessing(string& code);  // remove comments, whitespace, and transform to lowercase
vector<std::string> splitCodeToWords(const string& input);  
std::vector<std::string> splitStringByDelimiter(const std::string &str, char delimiter);  // split a string by a delimiter : used to split the file name
string escapeHTML(string str, bool similar = false);

string highlightSimilar(string str);
string highlightAddedAndRemoved(string str);

string GetDiff(vector<string> a, vector<string> b, vector<string> lcs);
string GetSimilarity(vector<string> a, vector<string> b, vector<string> lcs);

