#include<bits/stdc++.h>
#include "../include/exportReports.h"
#include "../include/submissions.h"
#include "../include/fileUtils.h"
#include "../include/textProcessing.h"
#include "../include/config.h"

using namespace std;

void exportCSV(string reportDir) {
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

void ExportParticipantsCSV(string reportDir) {
    if(similarSubmissions.size() == 0){
        ofstream file("reports/participants.csv");
        file << "No Similar Submissions Found\n";
        file.close();
        return;
    }
    map<string, int> participants;
    ofstream file("reports/participants.csv");
    file << "Handle,Score,Flag\n";



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
        file << i.first << "," << i.second << ","<< "False" << '\n';
    }

    file.close();
}

void ExportPairsOccurences(string reportDir) {
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


void ExportHTML(string reportDir) {

    // CREATE_DIR("reports/reportsHTML");


    string path = reportDir + "/HTMLreports";


    // CREATE_DIR(path.c_str());

    createDirectory(path);


    // if (CREATE_DIR("reports/reportsHTML") == 0) {
    //     std::cout << "Directory 'reports/HTMLreports' created successfully.\n";
    // } else {
    //     std::cerr << "Failed to create directory 'reports/HTMLreports'.\n";
    // }


    // Create index.html file in the root directory
    std::ofstream htmlFile(reportDir + "/index.html");
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
        htmlFile << "<button onclick=\"window.location.href='" << "HTMLreports/report_" << sub1.SubmissionId << "_" << sub2.SubmissionId << ".html';\">View Report</button>\n";
        htmlFile << "</li>\n";
    }
    htmlFile << "</ul>\n";

    // Generate detailed reports for each pair
    for (const auto &pair : similarSubmissions) {
        const auto &sub1 = pair.first.first;
        const auto &sub2 = pair.first.second;
        double similarity = pair.second;

        // Create individual detailed report
        std::ofstream detailFile(reportDir + "/HTMLreports/report_" + sub1.SubmissionId + "_" + sub2.SubmissionId + ".html");

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
        auto Key = sub1.SubmissionId + "_" + sub2.SubmissionId;
        if (diff.find(Key) != diff.end()) {
            detailFile << "<div class='code'>\n";
            detailFile << "<h3>Code Differences:</h3>\n";
            detailFile << escapeHTML(diff[Key]); // Show the diff in a preformatted block
            detailFile << "</div>\n";
        }

        detailFile << "<div class='code'>\n";
        detailFile << "<h3>Code Similarity:</h3>\n";
        detailFile << escapeHTML(similar[Key] , true); 
        detailFile << "</div>\n";
                

        detailFile << "</main>\n</body>\n</html>\n";

        // Close the detailed report file
        detailFile.close();
    }

    // Close the main index file
    htmlFile.close();
}


void ExportIndex() {
    // Contains links to all problems' reports
    // Each problem has a directory, and in this directory, there are all the reports of the pairs for this problem.
    // We need to create a file that contains the links to all the problem directories
    // Example: A/index.html
    // So links in the index.html file will contain the links to all the problem directories

    std::string baseDir = "reports";

    std::ofstream indexFile("index.html");
    if (!indexFile.is_open()) {
        std::cerr << "Failed to create index.html file!" << std::endl;
        return;
    }

    // Writing the HTML structure with added styles
    indexFile << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
              << "<meta charset=\"UTF-8\">\n"
              << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
              << "<title>Problem Reports</title>\n"
              << "<style>\n"
              << "/* Reset styles for consistency across browsers */\n"
              << "body, h1, ul, li { margin: 0; padding: 0; }\n"
              << "html, body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: linear-gradient(135deg, #f7f7f7, #e3e3e3); color: #333; height: 100%; }\n"
              << "header { background-color: #28a745; color: white; text-align: center; padding: 20px 0; }\n"
              << "header h1 { font-size: 3em; text-transform: uppercase; letter-spacing: 1px; }\n"
              << "main { padding: 30px; display: flex; justify-content: center; }\n"
              << "ul { display: grid; grid-template-columns: repeat(auto-fill, minmax(250px, 1fr)); gap: 30px; list-style: none; padding: 0; }\n"
              << "li { position: relative; background: white; border-radius: 12px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); overflow: hidden; transition: transform 0.3s ease, box-shadow 0.3s ease, background-color 0.3s ease; }\n"
              << "a { display: block; padding: 20px; text-decoration: none; text-align: center; font-size: 1.2em; color: #3498db; font-weight: bold; transition: color 0.3s ease, background-color 0.3s ease; }\n"
              << "a:hover { background-color: #3498db; color: white; }\n"
              << "li:hover { transform: translateY(-5px); box-shadow: 0 10px 20px rgba(0, 0, 0, 0.2); background-color: #f1f1f1; }\n"
              << "footer { text-align: center; padding: 20px; background-color: #2c3e50; color: white; }\n"
              << "footer p { font-size: 0.9em; }\n"
              << "@media (max-width: 768px) { header h1 { font-size: 2.5em; } }\n"
              << "@media (max-width: 500px) { ul { grid-template-columns: 1fr; } }\n"
              << "</style>\n"
              << "</head>\n<body>\n";

    // Header section
    indexFile << "<header>\n<h1>Problem Reports</h1>\n</header>\n";

    // Main content section
    indexFile << "<main>\n<ul>\n";

    // Iterate through directories and generate links
    for (const auto& entry : fs::directory_iterator(baseDir)) {
        if (fs::is_directory(entry)) {
            std::string problemName = entry.path().filename().string();
            std::string link = "reports/" + problemName + "/index.html";
            indexFile << "<li><a href=\"" << link << "\">" << problemName << "</a></li>\n";
        }
    }

    indexFile << "</ul>\n</main>\n";

    indexFile << "<footer>\n<p>&copy; 2024 Problem Reports. All rights reserved.</p>\n</footer>\n";

    indexFile << "</body>\n</html>\n";
    indexFile.close();

    std::cout << "index.html has been created successfully!" << std::endl;
}