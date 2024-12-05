#include <bits/stdc++.h>
#include "../include/similarity.h"
#include "../include/filters.h"
#include "../include/submissions.h"
#include "../include/fileManager.h"
#include "../include/config.h"
#include "../include/textProcessing.h"
#include "../include/helpers.h"

using namespace std;


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

void Compare(string path) {
    cout << fixed << setprecision(0);
    int totalIterations = files.size() * (files.size() - 1) / 2;
    int currentIteration = 0;
    const int barWidth = 50; 
    for (int i = 0; i < files.size(); i++) {
        for (int j = i + 1; j < files.size(); j++) {

            string code1, code2;


            bool skip = false;

            if(submissions[i].verdict != "AC" || submissions[j].verdict != "AC") skip = true;
            if(submissions[i].username == submissions[j].username) skip = true;    
            if(submissions[i].problem != submissions[j].problem) skip = true;
            if(find(excludedProblems.begin(), excludedProblems.end(), submissions[i].problem) != excludedProblems.end()) skip = true;
            if(includedProblems.size() > 0 && find(includedProblems.begin(), includedProblems.end(), submissions[i].problem) == includedProblems.end()) skip = true;
            if(includedUsers.size() > 0 &&
             (find(includedUsers.begin(), includedUsers.end(), submissions[i].username) == includedUsers.end() && 
                find(includedUsers.begin(), includedUsers.end(), submissions[j].username) == includedUsers.end())) skip = true;


            if(skip) {
                currentIteration++;
                double progress = (double)currentIteration / totalIterations;
                cout << '\r' << "Progress: ";
                cout << progress * 100 << "%" << flush;
                continue;
            }


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
            string s = GetSimilarity(words1, words2, lcs);

            diff[submissions[i].SubmissionId + "_" + submissions[j].SubmissionId] = d;
            similar[submissions[i].SubmissionId + "_" + submissions[j].SubmissionId] = s;


            if (similarity >= THRESHOLD && submissions[i].verdict == "AC" && submissions[j].verdict == "AC") {
                if(submissions[i].relativeTime > submissions[j].relativeTime) 
                    similarSubmissions.push_back({{submissions[j], submissions[i]}, similarity});
                else
                    similarSubmissions.push_back({{submissions[i], submissions[j]}, similarity}); 
            }

            currentIteration++;
            double progress = (double)currentIteration / totalIterations;

            cout << '\r' << "Progress: ";
            cout << progress * 100 << "%" << flush;

        }
    }

    string problemName = path.substr(path.find_last_of('/') + 1);

    cout << "\n";
    cout << "=================================================================================\n";
    cout << "  Comparing " << (problemName == "Uncategorized" ? "problem" : "") << problemName << " submissions Finished Successfully! ^_^\n";
    cout << "=================================================================================\n";

    sort(similarSubmissions.begin(), similarSubmissions.end(), [](pair<pair<submission, submission>, double> a, pair<pair<submission, submission>, double> b){
        return a.second > b.second;
    });
}