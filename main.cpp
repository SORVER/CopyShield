#include <bits/stdc++.h>
#include <dirent.h>
#include <fstream>
using namespace std;

#ifdef _WIN32
#define CREATE_DIR(name) _mkdir(name)
#define REMOVE_DIR(name) _rmdir(name)
#else
#define CREATE_DIR(name) mkdir(name, 0777)
#define REMOVE_DIR(name) rmdir(name)
#endif


int THRESHOLD = 40;
int WINDOW_SIZE = 5;
int GRAMS = 3;
int PRIME = 31;  
int CODEBLOCK_SIZE = 10;

double GetSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2);
vector<long long> GetFingerPrints(vector<long long> hashs);
vector<long long> Hash_n_Grams(vector<string> grams);
vector<string> Generate_n_grams(string code);
void TextProcessing(string& code);
std::vector<std::string> split(const std::string &str, char delimiter);
void Compare();
void exportCSV();
void ExportHTML();
string escapeHTML(string str);
void showUsage();



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
vector<string> includedUsers;


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
    for (int i = 0; i < files.size(); i++) {
        for (int j = i + 1; j < files.size(); j++) {
            string code1, code2;

            if(submissions[i].verdict != "AC" || submissions[j].verdict != "AC") continue;
            if(submissions[i].username == submissions[j].username) continue;    
            if(submissions[i].problem != submissions[j].problem) continue;
            if(find(excludedProblems.begin(), excludedProblems.end(), submissions[i].problem) != excludedProblems.end()) continue;
            if(includedProblems.size() > 0 && find(includedProblems.begin(), includedProblems.end(), submissions[i].problem) == includedProblems.end()) continue;
            if(includedUsers.size() > 0 && (find(includedUsers.begin(), includedUsers.end(), submissions[i].username) == includedUsers.end() && find(includedUsers.begin(), includedUsers.end(), submissions[j].username) == includedUsers.end())) continue;

            // getline(*files[i], code1, '\0');
            // files[i]->clear();               
            // files[i]->seekg(0, ios::beg);     
            
            // getline(*files[j], code2, '\0');
            // files[j]->clear();
            // files[j]->seekg(0, ios::beg);


            code1 = submissions[i].code;
            code2 = submissions[j].code;


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
        file << i.first.first.username << "," << i.first.second.username << "," << i.first.first.problem << "," << i.first.first.SubmissionId << "," << i.first.second.SubmissionId << "," << i.second << '\n';
    }
    file.close();
}

string escapeHTML(string str) {
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
    return newStr;   
}



void ExportHTML() {

    CREATE_DIR("reports");

    // Create index.html file in the root directory
    std::ofstream htmlFile("index.html");
    htmlFile << "<!DOCTYPE html>\n<html>\n<head>\n";
    htmlFile << "<meta charset='UTF-8'>\n";
    htmlFile << "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
    htmlFile << "<title>Similar Submissions</title>\n";
    htmlFile << "<style>\n";
    htmlFile << "body { font-family: 'Arial', sans-serif; background: #f7f9fb; color: #333; margin: 0; padding: 0; }\n";
    htmlFile << "header { background-color: #28a745; color: white; padding: 20px; text-align: center; font-size: 2.5em; border-bottom: 4px solid #218838; box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1); }\n";
    htmlFile << "main { padding: 40px 20px; max-width: 1200px; margin: auto; background-color: white; box-shadow: 0 6px 20px rgba(0, 0, 0, 0.1); border-radius: 10px; overflow: hidden; }\n";
    htmlFile << "h2 { color: #28a745; text-align: center; font-size: 2.2em; margin-bottom: 20px; text-transform: uppercase; }\n";
    htmlFile << "ul { list-style-type: none; padding: 0; display: flex; flex-wrap: wrap; justify-content: center; gap: 30px; }\n";
    htmlFile << "li { background: #ffffff; border-radius: 15px; padding: 20px; width: 300px; box-sizing: border-box; display: flex; justify-content: space-between; align-items: center; transition: transform 0.3s ease-in-out, background-color 0.3s ease; cursor: pointer; box-shadow: 0 5px 15px rgba(0, 0, 0, 0.05); }\n";
    htmlFile << "li:hover { transform: translateY(-10px); background-color: #f1f3f7; box-shadow: 0 10px 25px rgba(0, 0, 0, 0.1); }\n";
    htmlFile << "button { background-color: #28a745; color: white; border: none; padding: 12px 25px; border-radius: 25px; font-size: 1.1em; transition: background-color 0.3s ease, transform 0.2s ease; cursor: pointer; font-weight: 600; }\n";
    htmlFile << "button:hover { background-color: #218838; transform: scale(1.05); }\n";
    htmlFile << ".similarity { font-weight: bold; font-size: 1.4em; color: #17a2b8; }\n";
    htmlFile << "@media (max-width: 768px) {\n";
    htmlFile << "li { width: 100%; max-width: 400px; }\n";
    htmlFile << "}\n";
    htmlFile << "</style>\n";
    htmlFile << "</head>\n<body>\n";
    htmlFile << "<header>Similar Submissions</header>\n";
    htmlFile << "<main>\n";

    htmlFile << "<h2>All Similar Pairs</h2>\n";
    htmlFile << "<ul>\n";
    for (const auto &pair : similarSubmissions) {
        const auto &sub1 = pair.first.first;
        const auto &sub2 = pair.first.second;
        double similarity = pair.second;

        // List item with similarity percentage and button
        htmlFile << "<li>\n";
        htmlFile << "<span class='similarity'>Similarity: " << similarity << "%</span>\n";
        htmlFile << "<button onclick=\"window.location.href='reports/report_" << sub1.SubmissionId << "_" << sub2.SubmissionId << ".html';\">View Report</button>\n";
        htmlFile << "</li>\n";
    }
    htmlFile << "</ul>\n";

    // Generate detailed reports for each pair
    for (const auto &pair : similarSubmissions) {
        const auto &sub1 = pair.first.first;
        const auto &sub2 = pair.first.second;
        double similarity = pair.second;

        // Create individual detailed report
        std::ofstream detailFile("reports/report_" + sub1.SubmissionId + "_" + sub2.SubmissionId + ".html");

        detailFile << "<!DOCTYPE html>\n<html>\n<head>\n";
        detailFile << "<meta charset='UTF-8'>\n";
        detailFile << "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
        detailFile << "<title>Submission Pair Report</title>\n";
        detailFile << "<style>\n";
        detailFile << "body { font-family: 'Arial', sans-serif; background: #f7f9fb; color: #333; margin: 0; padding: 0; }\n";
        detailFile << "header { background-color: #28a745; color: white; padding: 20px; text-align: center; font-size: 2.5em; border-bottom: 4px solid #218838; box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1); }\n";
        detailFile << "main { padding: 40px 20px; max-width: 1200px; margin: auto; background-color: white; box-shadow: 0 6px 20px rgba(0, 0, 0, 0.1); border-radius: 10px; overflow: hidden; }\n";
        detailFile << "h2 { color: #28a745; text-align: center; font-size: 2.5em; margin-bottom: 20px; text-transform: uppercase; }\n";
        detailFile << "table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n";
        detailFile << "td { padding: 20px; border: 1px solid #ddd; text-align: left; background-color: #f9f9f9; }\n";
        detailFile << "td:first-child { border-right: 2px solid #28a745; }\n";
        detailFile << ".code { font-family: 'Courier New', monospace; background-color: #f4f4f4; padding: 20px; border-radius: 5px; border: 1px solid #ddd; white-space: pre-wrap; word-wrap: break-word; color: #333; font-size: 1.1em; }\n";
        detailFile << ".similarity { font-weight: bold; font-size: 1.6em; color: #218838; margin-bottom: 30px; text-align: center; }\n";
        detailFile << "@media (max-width: 768px) {\n";
        detailFile << "table { display: block; }\n";
        detailFile << "td { display: block; width: 100%; margin-bottom: 20px; }\n";
        detailFile << "td:first-child { border-right: none; }\n";
        detailFile << "}\n";
        detailFile << "</style>\n";
        detailFile << "</head>\n<body>\n";
        detailFile << "<header>Detailed Report</header>\n";
        detailFile << "<main>\n";
        detailFile << "<h2>Similarity: " << similarity << "%</h2>\n";

        // Create table for side-by-side code comparison
        detailFile << "<table>\n";
        detailFile << "<tr>\n";

        detailFile << "<td>\n";
        detailFile << "<h3>Submission 1</h3>\n";
        detailFile << "<p><strong>SubmissionId:</strong> " << sub1.SubmissionId << "</p>\n";
        // add name and problem
        detailFile << "<p><strong>Username:</strong> " << sub1.username << "</p>\n";
        detailFile << "<p><strong>Problem:</strong> " << sub1.problem << "</p>\n";
        detailFile << "<div class='code'>" << escapeHTML(sub1.code) << "</div>\n";
        detailFile << "</td>\n";

        detailFile << "<td>\n";
        detailFile << "<h3>Submission 2</h3>\n";
        detailFile << "<p><strong>SubmissionId:</strong> " << sub2.SubmissionId << "</p>\n";
        detailFile << "<p><strong>Username:</strong> " << sub2.username << "</p>\n";
        detailFile << "<p><strong>Problem:</strong> " << sub2.problem << "</p>\n";
        detailFile << "<div class='code'>" << escapeHTML(sub2.code) << "</div>\n";
        detailFile << "</td>\n";

        detailFile << "</tr>\n";
        detailFile << "</table>\n";
        detailFile << "</main>\n</body>\n</html>";
    }

    htmlFile << "</main>\n</body>\n</html>";
    htmlFile.close();
}


void showUsage() {
    cout << "Usage: ./main <directory_path> [options]\n";
    cout << "Options:\n";
    cout << "  --threshold, -t <value>                           Set the threshold value for similarity (default: 40)\n";
    cout << "  --exclude-problems, -e <problem1,problem2,...>    Exclude problems from comparison\n"; 
    cout << "  --include-problems, -i <problem1,problem2,...>    Include only these problems in comparison\n";   
    cout << "  --include-users, -u <user1,user2,...>             Include only these users in comparison\n";
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
            } else if(arg == "--include-users" || arg == "-u"){
                if (i + 1 >= argc) throw invalid_argument("Missing value for --include-users");
                string users = argv[++i];
                istringstream iss(users);
                string user;
                while (getline(iss, user, ',')) {
                    if (user.empty()) throw invalid_argument("Invalid format in user list");
                    includedUsers.push_back(user);
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
    exportCSV();
    ExportHTML();
    } catch(const exception& e){
        cout << "Error: " << e.what() << '\n';
    }
    return 0;
}