#include <bits/stdc++.h>
#include <dirent.h>
#include <fstream>
using namespace std;

vector<shared_ptr<ifstream>> files;
vector<string> Names;

double GetSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2) {  // Jaccard Similarity
    set<long long> intersection;
    set<long long> unionSet;
    for (long long i : fingerPrints1) {
        unionSet.insert(i);
    }
    for (long long i : fingerPrints2) {
        unionSet.insert(i);
    }
    for (long long i : fingerPrints1) {
        if (find(fingerPrints2.begin(), fingerPrints2.end(), i) != fingerPrints2.end()) {
            intersection.insert(i);
        }
    }
    if(unionSet.size() == 0) return 0;
    return (double)intersection.size() / unionSet.size();
}

vector<long long> GetFingerPrints(vector<long long> hashs, int windowSize) {
    vector<long long> fingerPrints;
    set<pair<long long, int>> window;  // value, index
    map<pair<long long, int>, int> count;
    int i = 0;
    for (; i < windowSize; i++) {
        window.insert({hashs[i], i});
    }
    fingerPrints.push_back(window.begin()->first);
    count[*window.begin()]++;
    for (; i < hashs.size(); i++) {
        window.insert({hashs[i], i});
        window.erase({hashs[i - windowSize], i - windowSize});
        count[*window.begin()]++;
        if (count[*window.begin()] == 1) {
            fingerPrints.push_back(window.begin()->first);
        }
    }
    return fingerPrints;
}

vector<long long> Hash_n_Grams(vector<string> grams) {
    vector<long long> hashes;
    for (int i = 0; i < grams.size(); i++) {
        long long hash = 0;
        int p = 31;

        for (int j = 0; j < grams[i].size(); j++) {
            hash += (grams[i][j] - '!' + 1) * pow(p, j);
        }
        hashes.push_back(hash);
    }
    return hashes;
}

vector<string> Generate_n_grams(string code, int n) {
    vector<string> grams;
    for (int i = 0; i + n <= code.size(); i++) {
        grams.push_back(code.substr(i, n));
    }
    return grams;
}

void TextProcessing(string& code) {
    // remove all comments
    code = regex_replace(code, regex("//.*"), "");
    code = regex_replace(code, regex("/\\*.*?\\*/"), "");

    // remove all whitespace
    code.erase(remove(code.begin(), code.end(), ' '), code.end());
    code.erase(remove(code.begin(), code.end(), '\n'), code.end());
    code.erase(remove(code.begin(), code.end(), '\t'), code.end());

    // transform the code to lowercase
    transform(code.begin(), code.end(), code.begin(), ::tolower);
}

std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void Compare() {
    for (int i = 0; i < files.size(); i++) {
        for (int j = i + 1; j < files.size(); j++) {
            string code1, code2;

            // will be in this format 55643280_AC_SH3_H.cpp 
            vector<string> firstSubInfo = split(Names[i], '_');
            vector<string> secondSubInfo = split(Names[j], '_');

            // for(int k = 0; k < firstSubInfo.size(); k++){
            //     cout << firstSubInfo[k] << " ";
            // }

            if(firstSubInfo[1] != "AC" || secondSubInfo[1] != "AC") continue;
            if(firstSubInfo[2] == secondSubInfo[2]) continue;
            // if(firstSubInfo[3] != secondSubInfo[3]) continue;

            
            getline(*files[i], code1, '\0');
            files[i]->clear();               
            files[i]->seekg(0, ios::beg);     
            
            getline(*files[j], code2, '\0');
            files[j]->clear();
            files[j]->seekg(0, ios::beg);

            if (code1.empty() || code2.empty()) {
                continue;
            }

            TextProcessing(code1);
            TextProcessing(code2);
            
            vector<string> grams1 = Generate_n_grams(code1, 3);
            vector<string> grams2 = Generate_n_grams(code2, 3);

            vector<long long> hashes1 = Hash_n_Grams(grams1);
            vector<long long> hashes2 = Hash_n_Grams(grams2);

            vector<long long> fingerPrints1 = GetFingerPrints(hashes1, 5);
            vector<long long> fingerPrints2 = GetFingerPrints(hashes2, 5);

            double similarity = GetSimilarity(fingerPrints1, fingerPrints2) * 100;

            int lowerBound = 70 , upperBound = 80;

            if (similarity > lowerBound && similarity < upperBound && Names[i].find("AC") != string::npos) {
                cout << firstSubInfo.back() << ' ' << firstSubInfo[2] << ' ' <<  firstSubInfo[0] << ' ' << secondSubInfo[2] << ' ' << secondSubInfo[0] << " " << similarity << "\n";
            }

            // cout << similarity << "% similarity between file " << i << " and file " << j << '\n';
        }
    }
    cout << "Done\n";
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Please provide the dir path\n";
        return 0;
    }
    string dirPath = argv[1];
    DIR* dir = opendir(dirPath.c_str());
    try {
        if (dir == NULL) {
            cerr << "Failed to open the Directory\n";
            return 0;
        }
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) {
                string filePath = dirPath + "/" + entry->d_name;
                Names.push_back(entry->d_name);
                shared_ptr<ifstream> file = make_shared<ifstream>(filePath);
                if (!file->is_open()) {
                    cerr << "Failed to open the file " << filePath << '\n';
                    continue;
                }
                files.push_back(file);
            }
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << '\n';
    }

    closedir(dir);
    try{
    Compare();
    } catch(const exception& e){
        cout << "Error: " << e.what() << '\n';
    }
    return 0;
}
