#include <bits/stdc++.h>
using namespace std;

void TextProccessing(string& code){
    // remove all comments
    code = regex_replace(code, regex("//.*"), "");
    code = regex_replace(code, regex("/\\*.*?\\*/"), "");

    // remove all withespace
    code.erase(remove(code.begin(), code.end(), ' '), code.end());
    code.erase(remove(code.begin(), code.end(), '\n'), code.end());
    code.erase(remove(code.begin(), code.end(), '\t'), code.end());

    // transform the code to lowercase 
    transform(code.begin(), code.end(), code.begin(), ::tolower);
}

int main(){
    string code = "int main(){\n\t// this is a comment\n\t/* this is a comment */\n\treturn 0;\n}";
    TextProccessing(code);
    cout << code << endl;
    return 0;
}