#include <bits/stdc++.h>
#include <dirent.h>
#include <fstream>
using namespace std;

int THRESHOLD = 40;
int WINDOW_SIZE = 5;
int GRAMS = 3;
int PRIME = 31;  

double GetSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2);
vector<long long> GetFingerPrints(vector<long long> hashs);
vector<long long> Hash_n_Grams(vector<string> grams);
vector<string> Generate_n_grams(string code);
void TextProcessing(string& code);
std::vector<std::string> split(const std::string &str, char delimiter);
void Compare();


struct submission{
    string verdict;
    string username;
    string problem;
    string code;

    submission(string code, string verdict, string username, string problem){
        this->verdict = verdict;
        this->username = username;
        this->problem = problem;
        this->code = code;
    }
};

vector<shared_ptr<ifstream>> files;
vector<submission> submissions;
vector<pair<pair<submission, submission>, double>> similarSubmissions;

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

vector<long long> GetFingerPrints(vector<long long> hashs) {
    vector<long long> fingerPrints;
    set<pair<long long, int>> window;  // value, index
    map<pair<long long, int>, int> count;
    int i = 0;
    for (; i < WINDOW_SIZE; i++) {
        window.insert({hashs[i], i});
    }
    fingerPrints.push_back(window.begin()->first);
    count[*window.begin()]++;
    for (; i < hashs.size(); i++) {
        window.insert({hashs[i], i});
        window.erase({hashs[i - WINDOW_SIZE], i - WINDOW_SIZE});
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

        for (int j = 0; j < grams[i].size(); j++) {
            hash += (grams[i][j] - '!' + 1) * pow(PRIME, j);
        }
        hashes.push_back(hash);
    }
    return hashes;
}

vector<string> Generate_n_grams(string code) {
    vector<string> grams;
    for (int i = 0; i + GRAMS <= code.size(); i++) {
        grams.push_back(code.substr(i, GRAMS));
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

    if(tokens.size() == 5){
        tokens[2] = tokens[2] + "_" + tokens[3];
        tokens.erase(tokens.begin() + 3);
    }
    
    return tokens;
}

void Compare() {
    int count = 0;
    for (int i = 0; i < files.size(); i++) {
        for (int j = i + 1; j < files.size(); j++) {
            string code1, code2;

            if(submissions[i].verdict != "AC" || submissions[j].verdict != "AC") continue;
            if(submissions[i].username == submissions[j].username) continue;    
            if(submissions[i].problem != submissions[j].problem) continue;

            
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
            
            vector<string> grams1 = Generate_n_grams(code1);
            vector<string> grams2 = Generate_n_grams(code2);

            vector<long long> hashes1 = Hash_n_Grams(grams1);
            vector<long long> hashes2 = Hash_n_Grams(grams2);

            vector<long long> fingerPrints1 = GetFingerPrints(hashes1);
            vector<long long> fingerPrints2 = GetFingerPrints(hashes2);

            double similarity = GetSimilarity(fingerPrints1, fingerPrints2) * 100;


            if (similarity >= THRESHOLD && submissions[i].verdict == "AC" && submissions[j].verdict == "AC") {
                similarSubmissions.push_back({{submissions[i], submissions[j]}, similarity});
            }

        }
    }
    sort(similarSubmissions.begin(), similarSubmissions.end(), [](pair<pair<submission, submission>, double> a, pair<pair<submission, submission>, double> b){
        return a.second > b.second;
    });
}

void exportCSV(){
    if(similarSubmissions.size() == 0){
        ofstream file("result.csv");
        file << "No Similar Submissions Found\n";
        file.close();
        return;
    }
    ofstream file("result.csv");
    file << "Username1, Username2, Problem, CodeId1, CodeId2, Similarity\n";
    for(auto i : similarSubmissions){
        file << i.first.first.username << "," << i.first.second.username << "," << i.first.first.problem << "," << i.first.first.code << "," << i.first.second.code << "," << i.second << '\n';
    }
    file.close();
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
                vector<string> temp = split(entry->d_name, '_');
                submissions.push_back({temp[0], temp[1], temp[2], temp[3]});
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
    exportCSV();
    } catch(const exception& e){
        cout << "Error: " << e.what() << '\n';
    }
    return 0;
}
