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
            string s = GetSimilarity(words1, words2, lcs);

            diff[submissions[i].SubmissionId + "_" + submissions[j].SubmissionId] = d;
            similar[submissions[i].SubmissionId + "_" + submissions[j].SubmissionId] = s;


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