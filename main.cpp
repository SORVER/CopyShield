#include <bits/stdc++.h>
#include <dirent.h>
#include <fstream>
using namespace std;

int THRESHOLD = 40;
int WINDOW_SIZE = 5;
int GRAMS = 3;
int PRIME = 31;  
int CODEBLOCK_SIZE = 30;

double GetSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2);
vector<long long> GetFingerPrints(vector<long long> hashs);
vector<long long> Hash_n_Grams(vector<string> grams);
vector<string> Generate_n_grams(string code);
void TextProcessing(string& code);
std::vector<std::string> split(const std::string &str, char delimiter);
void Compare();
void exportCSV();
void ExportHTML();
pair<string,string> HighlightSimilarBlocks(const std::string &code1, const std::string &code2);



struct submission{
    string verdict;
    string username;
    string problem;
    string code;
    string SubmissionId;

    submission(string SubmissionId, string verdict, string username, string problem, string code){
        this->verdict = verdict;
        this->username = username;
        this->problem = problem;
        this->SubmissionId = SubmissionId;
        this->code = code;
    }
};

vector<shared_ptr<ifstream>> files;
vector<submission> submissions;
vector<pair<pair<submission, submission>, double>> similarSubmissions;
vector<string> excludedProblems;
vector<string> includedProblems;

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
            if(find(excludedProblems.begin(), excludedProblems.end(), submissions[i].problem) != excludedProblems.end()) continue;
            if(includedProblems.size() > 0 && find(includedProblems.begin(), includedProblems.end(), submissions[i].problem) == includedProblems.end()) continue;

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


void ExportHTML() {
    std::ofstream htmlFile("report.html");
    htmlFile << "<!DOCTYPE html>\n<html>\n<head>\n";
    htmlFile << "<style>\n";
    htmlFile << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
    htmlFile << "h1 { text-align: center; }\n";
    htmlFile << "h2 { color: #444; margin-top: 40px; }\n";
    htmlFile << ".submission-table { width: 100%; border-collapse: collapse; margin: 20px 0; }\n";
    htmlFile << ".submission-table td { vertical-align: top; padding: 10px; border: 1px solid #ddd; width: 50%; }\n";
    htmlFile << ".header { font-size: 1.1em; font-weight: bold; margin-bottom: 10px; }\n";
    htmlFile << ".code { white-space: pre-wrap; font-family: monospace; background-color: #f9f9f9; padding: 10px; ";
    htmlFile << "border-radius: 5px; word-wrap: break-word; width: 100%; box-sizing: border-box; }\n";
    htmlFile << ".highlight { background-color: yellow; }\n";
    htmlFile << "</style>\n";
    htmlFile << "</head>\n<body>\n";
    htmlFile << "<h1>Similar Submissions</h1>\n";

    for (const auto &pair : similarSubmissions) {
        const auto &sub1 = pair.first.first;
        const auto &sub2 = pair.first.second;
        double similarity = pair.second;

        // Add similarity header
        htmlFile << "<h2>Similarity: " << similarity << "%</h2>\n";

        // Create table for side-by-side display
        htmlFile << "<table class='submission-table'>\n";
        htmlFile << "<tr>\n";

        // Submission 1
        htmlFile << "<td>\n";
        htmlFile << "<div class='header'>Submission 1 (" << sub1.username << ")<br>Id: " << sub1.SubmissionId << "</div>\n";
        htmlFile << "<div class='code'>" << HighlightSimilarBlocks(sub1.code, sub2.code).first << "</div>\n";
        htmlFile << "</td>\n";

        // Submission 2
        htmlFile << "<td>\n";
        htmlFile << "<div class='header'>Submission 2 (" << sub2.username << ")<br>Id: " << sub2.SubmissionId << "</div>\n";
        htmlFile << "<div class='code'>" << HighlightSimilarBlocks(sub1.code, sub2.code).second << "</div>\n";
        htmlFile << "</td>\n";

        htmlFile << "</tr>\n";
        htmlFile << "</table>\n";
    }

    htmlFile << "</body>\n</html>";
    htmlFile.close();
}




pair<string,string> HighlightSimilarBlocks(const string &code1, const string &code2) {
    vector<pair<int, int>> ranges1; 
    vector<pair<int, int>> ranges2; 

    for (int i = 0; i + CODEBLOCK_SIZE <= code1.size(); ++i) {
        for (int j = 0; j + CODEBLOCK_SIZE <= code2.size(); ++j) {
            if (code1.substr(i, CODEBLOCK_SIZE) == code2.substr(j, CODEBLOCK_SIZE)) {
                int k = CODEBLOCK_SIZE;

                while (i + k < code1.size() && j + k < code2.size() && 
                       code1[i + k] == code2[j + k]) {
                    ++k;
                }

                ranges1.push_back(make_pair(i, k));
                ranges2.push_back(make_pair(j, k));

                i += k - 1;
                break;
            }
        }
    }

    auto applyHighlights = [](const string &code, const vector<pair<int, int>> &ranges) {
        stringstream highlighted;
        size_t idx = 0;

        for (size_t r = 0; r < ranges.size(); ++r) {
            int start = ranges[r].first;
            int len = ranges[r].second;

            highlighted << code.substr(idx, start - idx);

            highlighted << "<span class='highlight'>" << code.substr(start, len) << "</span>";

            idx = start + len;
        }

        highlighted << code.substr(idx);

        return highlighted.str();
    };

    string highlightedCode1 = applyHighlights(code1, ranges1);
    string highlightedCode2 = applyHighlights(code2, ranges2);

    return make_pair(highlightedCode1, highlightedCode2);
}



void showUsage() {
    cout << "Usage: ./main <directory_path> [options]\n";
    cout << "Options:\n";
    cout << "  --threshold, -t <value>                           Set the threshold value for similarity (default: 40)\n";
    cout << "  --exclude-problems, -e <problem1,problem2,...>    Exclude problems from comparison\n"; 
    cout << "  --include-problems, -i <problem1,problem2,...>    Include only these problems in comparison\n";   
    cout << "  --window-size, -w <value>                         Set the window size for fingerprinting (default: 5)\n";
    cout << "  --grams, -g <value>                               Set the n-grams value for hashing (default: 3)\n";
    cout << "  --prime, -p <value>                               Set the prime value for hashing (default: 31)\n";
    cout << "  --help, -h                                        Show this help message\n";

}

int main(int argc, char *argv[]) {
    string dirPath;
    try{
        if(argc < 2) {
            throw invalid_argument("Directory path is required");
        }
        dirPath = argv[1];
        for(int i = 2; i < argc; i++){
            string arg = argv[i];
            if(arg == "--threshold" || arg == "-t"){
                if (i + 1 >= argc){
                    throw invalid_argument("Missing value for --threshold");
                }
                THRESHOLD = stoi(argv[++i]);
                if(THRESHOLD < 1 || THRESHOLD > 100){
                    throw out_of_range("Threshold must be between 1 and 100");
                }
            } else if(arg == "--exclude-problems" || arg == "-e"){
                if (i + 1 >= argc) throw invalid_argument("Missing value for --exclude-problems");
                string problems = argv[++i];
                istringstream iss(problems);
                string problem;
                while (getline(iss, problem, ',')) {
                    if (problem.empty()) throw invalid_argument("Invalid format in problem list");
                    excludedProblems.push_back(problem);
                }
            } else if(arg == "--include-problems" || arg == "-i"){
                if (i + 1 >= argc) throw invalid_argument("Missing value for --include-problems");
                string problems = argv[++i];
                istringstream iss(problems);
                string problem;
                while (getline(iss, problem, ',')) {
                    if (problem.empty()) throw invalid_argument("Invalid format in problem list");
                    includedProblems.push_back(problem);
                }
            } else if(arg == "--window-size" || arg == "-w"){
                if (i + 1 >= argc){
                    throw invalid_argument("Missing value for --window-size");
                }
                WINDOW_SIZE = stoi(argv[++i]);
                if(WINDOW_SIZE < 1){
                    throw out_of_range("Window size must be greater than 0");
                }
            } else if(arg == "--grams" || arg == "-g"){
                if (i + 1 >= argc){
                    throw invalid_argument("Missing value for --grams");
                }
                GRAMS = stoi(argv[++i]);
                if(GRAMS < 1){
                    throw out_of_range("Grams must be greater than 0");
                }
            } else if(arg == "--prime" || arg == "-p"){
                if (i + 1 >= argc){
                    throw invalid_argument("Missing value for --prime");
                }
                PRIME = stoi(argv[++i]);
                if(PRIME < 1){
                    throw out_of_range("Prime must be greater than 0");
                }
            } else if(arg == "--help" || arg == "-h"){
                showUsage();
                return 0;
            } else{
                throw invalid_argument("Unknown option: " + arg);
            }
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n\n";
        showUsage();
        return 1;
    }

    


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
                shared_ptr<ifstream> file = make_shared<ifstream>(filePath);
                if (!file->is_open()) {
                    cerr << "Failed to open the file " << filePath << '\n';
                    continue;
                }
                files.push_back(file);

                string code((istreambuf_iterator<char>(*file)), istreambuf_iterator<char>());

                // remove the spaces in the end of the file
                while (code[code.size() - 1] == ' ' || code[code.size() - 1] == '\n') {
                    code.pop_back();
                }

                submissions.push_back({temp[0], temp[1], temp[2], temp[3], code});
            }
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << '\n';
    }

    closedir(dir);
    try{
    Compare();
    // exportCSV();
    ExportHTML();
    } catch(const exception& e){
        cout << "Error: " << e.what() << '\n';
    }
    return 0;
}
