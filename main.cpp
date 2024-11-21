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
int PRIME = 5;  
int CODEBLOCK_SIZE = 10;

double JaccardSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2);
double CosinSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2);
vector<long long> GetFingerPrints(vector<long long> hashs);
vector<long long> Hash_n_Grams(vector<string> grams);
vector<string> Generate_n_grams(string code);
void TextProcessing(string& code);
std::vector<std::string> split(const std::string &str, char delimiter);
void Compare();
void exportCSV();
void ExportHTML();
void ExportParticipantsCSV();  // contains handle and int for number of occurences
void ExportPairsOccurences();  // contains username1, username2, # of occurences of the pair
string escapeHTML(string str);
void showUsage();
string highlightAddedAndRemoved(string str);
string GetDiff(vector<string> a, vector<string> b, vector<string> lcs);
vector<string> LCS(vector<string> a, vector<string> b);
vector<std::string> splitCodeToWords(const string& input);






struct submission{
    string verdict;
    string username;
    string problem;
    string code;
    string SubmissionId;
    string relativeTime = "N/A";

    submission(string SubmissionId, string verdict, string username, string problem, string code, string relativeTime){
        this->verdict = verdict;
        this->username = username;
        this->problem = problem;
        this->SubmissionId = SubmissionId;
        this->code = code;
        this->relativeTime = relativeTime;
    }

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
map<string, string> diff;  // the diff of the two codes in the pair SubmissionId1_SubmissionId2


double CosinSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2) {  // Cosine Similarity:  https://en.wikipedia.org/wiki/Cosine_similarity
    long long dotProduct = 0;
    long long magnitude1 = 0;
    long long magnitude2 = 0;
    for (int i = 0; i < min(fingerPrints1.size(), fingerPrints2.size()); i++) {
        // cout << fingerPrints1[i] << " " << fingerPrints2[i] << '\n';
        dotProduct += fingerPrints1[i] * fingerPrints2[i];
        magnitude1 += fingerPrints1[i] * fingerPrints1[i];
        magnitude2 += fingerPrints2[i] * fingerPrints2[i];
    }
    if (magnitude1 == 0 || magnitude2 == 0) return 0;
    return dotProduct / (sqrt(magnitude1) * sqrt(magnitude2));
}

double JaccardSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2) {  // Jaccard Similarity
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

vector<string> LCS(vector<string> a, vector<string> b){
    int n = a.size();
    int m = b.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));
    for(int i = 1; i <= n; i++){
        for(int j = 1; j <= m; j++){
            if(a[i - 1] == b[j - 1]){
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else{
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    vector<string> lcs;
    int i = n, j = m;
    while(i > 0 && j > 0){
        if(a[i - 1] == b[j - 1]){
            lcs.push_back(a[i - 1]);
            i--;
            j--;
        } else if(dp[i - 1][j] > dp[i][j - 1]){
            i--;
        } else{
            j--;
        }
    }
    reverse(lcs.begin(), lcs.end());
    return lcs;
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

std::vector<std::string> split(const std::string &str, char delimiter) {
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

void Compare() {
    cout << submissions.size() << '\n';
    // for(auto i : submissions){
    //     cout << i.username << ' ' << i.problem << ' ' << i.verdict <<  ' ' << i.SubmissionId << '\n';
    // }
    for (int i = 0; i < files.size(); i++) {
        // cout << i << '\n';
        for (int j = i + 1; j < files.size(); j++) {
            // cout << j << ' ' << submissions[j].SubmissionId << '\n';
            // if(submissions[j].SubmissionId.empty()){
            //     cout << "Empty\n";
            // }
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




            string code1Temp = code1;
            string code2Temp = code2;

            RemoveComments(code1Temp);
            RemoveComments(code2Temp);

            // cout << code1 << '\n';

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

            // double similarity = CosinSimilarity(fingerPrints1, fingerPrints2) * 100;
            double similarity = JaccardSimilarity(fingerPrints1, fingerPrints2) * 100;


            // cout << code1Temp << '\n';

            auto words1 = splitCodeToWords(code1Temp);
            auto words2 = splitCodeToWords(code2Temp);
            auto lcs = LCS(words1, words2);

            string d = GetDiff(words1, words2, lcs);

            diff[submissions[i].SubmissionId + "_" + submissions[j].SubmissionId] = d;


            if (similarity >= THRESHOLD && submissions[i].verdict == "AC" && submissions[j].verdict == "AC") {
                similarSubmissions.push_back({{submissions[i], submissions[j]}, similarity});
            }

        }
    }
    cout << "Done\n";
    sort(similarSubmissions.begin(), similarSubmissions.end(), [](pair<pair<submission, submission>, double> a, pair<pair<submission, submission>, double> b){
        return a.second > b.second;
    });
}


void ExportPairsOccurences() {
    if(similarSubmissions.size() == 0){
        ofstream file("reports/pairs.csv");
        file << "No Similar Submissions Found\n";
        file.close();
        return;
    }
    map<pair<string, string>, vector<int>> pairs;
    ofstream file("reports/pairs.csv");
    file << "Username1, Username2, Occurences\n";
    for(auto i : similarSubmissions){
        pairs[{i.first.first.username, i.first.second.username}].push_back(i.second);
    }

    vector<pair<pair<string, string>, vector<int>>> pairsVec(pairs.begin(), pairs.end());
    sort(pairsVec.begin(), pairsVec.end(), [](pair<pair<string, string>, vector<int>> a, pair<pair<string, string>, vector<int>> b){
        if(a.second.size() == b.second.size()){
            int sumA = 0, sumB = 0;
            for(auto i : a.second){
                sumA += i;
            }
            for(auto i : b.second){
                sumB += i;
            }
            return sumA > sumB;
        }
        return a.second.size() > b.second.size();
    });

    for(auto i : pairsVec){
        file << i.first.first << "," << i.first.second << "," ;
        for(auto j : i.second){
            file << j << " ";
        }
        file << '\n';
    }

    file.close();
}


void ExportParticipantsCSV() {
    if(similarSubmissions.size() == 0){
        ofstream file("reports/participants.csv");
        file << "No Similar Submissions Found\n";
        file.close();
        return;
    }
    map<string, int> participants;
    ofstream file("reports/participants.csv");
    file << "Handle, # of Occurences\n";



    for (const auto &pair : similarSubmissions) {
        if(pair.second >= 90) {
            participants[pair.first.second.username] = 9999999;
            participants[pair.first.first.username] = 9999999;
        } else if (pair.second >= 70 && pair.second < 90){
            participants[pair.first.second.username] += 10000;
            participants[pair.first.first.username] += 10000;
        } else if (pair.second >= 50 && pair.second < 70){
            participants[pair.first.second.username] += 1000;
            participants[pair.first.first.username] += 1000;
        } else if (pair.second >= 40 && pair.second < 50){
            participants[pair.first.second.username] += 250;
            participants[pair.first.first.username] += 250;
        } else {
            participants[pair.first.second.username] += 100;
            participants[pair.first.first.username] += 100;
        }
    }

    vector<pair<string, int>> participantsVec(participants.begin(), participants.end());
    sort(participantsVec.begin(), participantsVec.end(), [](pair<string, int> a, pair<string, int> b){
        return a.second > b.second;
    });

    for(auto i : participantsVec){
        file << i.first << "," << i.second << '\n';
    }

    file.close();
}

void exportCSV(){
    if(similarSubmissions.size() == 0){
        ofstream file("reports/result.csv");
        file << "No Similar Submissions Found\n";
        file.close();
        return;
    }
    ofstream file("reports/result.csv");
    file << "Username1, Username2, Problem, CodeId1, CodeId2, Similarity\n";
    for(auto i : similarSubmissions){
        file << i.first.first.username << "," << i.first.second.username << "," << i.first.first.problem << "," << i.first.first.SubmissionId << "," << i.first.second.SubmissionId << "," << i.second << '\n';
    }
    file.close();
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
    return highlightAddedAndRemoved(newStr);   
}



void ExportHTML() {

    CREATE_DIR("reports/HTMLreports");

    // Create index.html file in the root directory
    std::ofstream htmlFile("reports/index.html");
    htmlFile << "<!DOCTYPE html>\n<html>\n<head>\n";
    htmlFile << "<meta charset='UTF-8'>\n";
    htmlFile << "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
    htmlFile << "<title>Similar Submissions</title>\n";
    htmlFile << "<style>\n";
    htmlFile << "body { font-family: 'Arial', sans-serif; background: #f7f9fb; color: #333; margin: 0; padding: 0; }\n";
    htmlFile << "header { background-color: #28a745; color: white; padding: 20px; text-align: center; font-size: 2.5em; border-bottom: 4px solid #218838; box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1); }\n";
    htmlFile << "main { padding: 40px 20px; max-width: 1200px; margin: auto; background-color: white; box-shadow: 0 6px 20px rgba(0, 0, 0, 0.1); border-radius: 10px; overflow: hidden; }\n";
    htmlFile << "h2 { color: #28a745; text-align: center; font-size: 2.2em; margin-bottom: 20px; }\n";
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

    htmlFile << "<h2>All Similar Pairs: ";
    htmlFile << similarSubmissions.size();
    htmlFile << "</h2>\n";
    htmlFile << "<ul>\n";
    for (const auto &pair : similarSubmissions) {
        const auto &sub1 = pair.first.first;
        const auto &sub2 = pair.first.second;
        double similarity = pair.second;

        // List item with similarity percentage and button
        htmlFile << "<li>\n";
        htmlFile << "<span class='similarity'>Similarity: " << int(similarity) << "%</span>\n";
        htmlFile << "<button onclick=\"window.location.href='HTMLreports/report_" << sub1.SubmissionId << "_" << sub2.SubmissionId << ".html';\">View Report</button>\n";
        htmlFile << "</li>\n";
    }
    htmlFile << "</ul>\n";

    // Generate detailed reports for each pair
    for (const auto &pair : similarSubmissions) {
        const auto &sub1 = pair.first.first;
        const auto &sub2 = pair.first.second;
        double similarity = pair.second;

        // Create individual detailed report
        std::ofstream detailFile("reports/HTMLreports/report_" + sub1.SubmissionId + "_" + sub2.SubmissionId + ".html");

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
        detailFile << ".diff { background-color: #fffbcc; padding: 15px; border: 1px solid #ddd; border-radius: 5px; font-family: 'Courier New', monospace; color: #333; white-space: pre-wrap; word-wrap: break-word; font-size: 1em; margin-top: 20px; }\n"; // Styling for diff
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

        // Submission 1
        detailFile << "<td>\n";
        detailFile << "<h3>Submission 1</h3>\n";
        detailFile << "<p><strong>SubmissionId:</strong> " << sub1.SubmissionId << "</p>\n";
        detailFile << "<p><strong>Username:</strong> " << sub1.username << "</p>\n";
        detailFile << "<p><strong>Problem:</strong> " << sub1.problem << "</p>\n";
        detailFile << "<p><strong>Time:</strong> " << sub1.relativeTime << "</p>\n";
        detailFile << "<div class='code'>" << escapeHTML(sub1.code) << "</div>\n";
        detailFile << "</td>\n";

        // Submission 2
        detailFile << "<td>\n";
        detailFile << "<h3>Submission 2</h3>\n";
        detailFile << "<p><strong>SubmissionId:</strong> " << sub2.SubmissionId << "</p>\n";
        detailFile << "<p><strong>Username:</strong> " << sub2.username << "</p>\n";
        detailFile << "<p><strong>Problem:</strong> " << sub2.problem << "</p>\n";
        detailFile << "<p><strong>Time:</strong> " << sub2.relativeTime << "</p>\n";
        detailFile << "<div class='code'>" << escapeHTML(sub2.code) << "</div>\n";
        detailFile << "</td>\n";

        detailFile << "</tr>\n";
        detailFile << "</table>\n";

        // Display the diff if it exists
        auto diffKey = sub1.SubmissionId + "_" + sub2.SubmissionId;
        if (diff.find(diffKey) != diff.end()) {
            detailFile << "<div class='code'>\n";
            detailFile << "<h3>Code Differences:</h3>\n";
            detailFile << escapeHTML(diff[diffKey]); // Show the diff in a preformatted block
            detailFile << "</div>\n";
        }

        detailFile << "</main>\n</body>\n</html>\n";

        // Close the detailed report file
        detailFile.close();
    }

    // Close the main index file
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

                string code((istreambuf_iterator<char>(*file)), istreambuf_iterator<char>());

                if (!code.empty()) {
                    while (!code.empty() && (code.back() == ' ' || code.back() == '\n')) {
                        code.pop_back();
                    }
                }


                if(temp[0][0] == '[') {
                    // cout << "PASSED\n";
                    // cout << temp.size() << '\n';
                    if (temp.size() < 5) {
                        cerr << "Invalid file name format: " << entry->d_name << '\n';
                        continue;
                    }

                    if(temp[2] != "AC") continue;
                    files.push_back(file);

                    // cout << temp[1] << '\n';

                    // if username contains _ 
                    while(temp.size() > 5){
                        temp[3] = temp[3] + "_" + temp[4];
                        temp.erase(temp.begin() + 4);
                    }


                    temp[4].erase(temp[4].find('.'), temp[4].size());
                    // cout << "Processing " << temp[0] << " " << temp[1] << " " << temp[2] << " " << temp[3] << '\n';

                    temp[0].erase(0, 1);
                    temp[0].erase(temp[0].size() - 1, 1);

                    submissions.push_back({temp[1], temp[2], temp[3], temp[4], code, temp[0]});
                } else {
                    if (temp.size() < 4) {
                        cerr << "Invalid file name format: " << entry->d_name << '\n';
                        continue;
                    }

                    if(temp[1] != "AC") continue;
                    files.push_back(file);

                    // cout << temp[1] << '\n';

                    // if username contains _ 
                    while(temp.size() > 4){
                        temp[2] = temp[2] + "_" + temp[3];
                        temp.erase(temp.begin() + 3);
                    }


                    temp[3].erase(temp[3].find('.'), temp[3].size());
                    // cout << "Processing " << temp[0] << " " << temp[1] << " " << temp[2] << " " << temp[3] << '\n';

                    submissions.push_back({temp[0], temp[1], temp[2], temp[3], code});
                }
            }
        }

        cout << "Processing submissions...\n";

    } catch (const exception& e) {
        cout << "Error: " << e.what() << '\n';
    }


    closedir(dir);


    try{
        REMOVE_DIR("reports");
        CREATE_DIR("reports");

        Compare();
        exportCSV();
        ExportHTML();
        ExportParticipantsCSV();
        ExportPairsOccurences();
    } catch(const exception& e){
        cout << "Error: " << e.what() << '\n';
    }
    return 0;
}