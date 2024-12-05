#include <bits/stdc++.h>
#include <filesystem>
#include "../include/fileUtils.h"
#include "../include/submissions.h"
#include "../include/similarity.h"
#include "../include/exportReports.h"
#include "../include/config.h"
#include "../include/textProcessing.h"
#include "../include/filters.h"
#include "../include/helpers.h"
#include "../include/textProcessing.h"
#include "../include/fileManager.h"

namespace fs = std::filesystem;

using namespace std;

int main(int argc, char *argv[]) {
    string dirPath;
    
    try {
        // Validate arguments
        if (argc < 2) {
            throw invalid_argument("Directory path is required");
        }
        dirPath = argv[1];

        // Parse command line arguments
        for (int i = 2; i < argc; i++) {
            string arg = argv[i];
            if (arg == "--threshold" || arg == "-t") {
                if (i + 1 >= argc) {
                    throw invalid_argument("Missing value for --threshold");
                }
                THRESHOLD = stoi(argv[++i]);
                if (THRESHOLD < 1 || THRESHOLD > 100) {
                    throw out_of_range("Threshold must be between 1 and 100");
                }
            } else if (arg == "--exclude-problems" || arg == "-e") {
                if (i + 1 >= argc) throw invalid_argument("Missing value for --exclude-problems");
                string problems = argv[++i];
                istringstream iss(problems);
                string problem;
                while (getline(iss, problem, ',')) {
                    if (problem.empty()) throw invalid_argument("Invalid format in problem list");
                    excludedProblems.push_back(problem);
                }
            } else if (arg == "--include-problems" || arg == "-i") {
                if (i + 1 >= argc) throw invalid_argument("Missing value for --include-problems");
                string problems = argv[++i];
                istringstream iss(problems);
                string problem;
                while (getline(iss, problem, ',')) {
                    if (problem.empty()) throw invalid_argument("Invalid format in problem list");
                    includedProblems.push_back(problem);
                }
            } else if (arg == "--include-users" || arg == "-u") {
                if (i + 1 >= argc) throw invalid_argument("Missing value for --include-users");
                string users = argv[++i];
                istringstream iss(users);
                string user;
                while (getline(iss, user, ',')) {
                    if (user.empty()) throw invalid_argument("Invalid format in user list");
                    includedUsers.push_back(user);
                }
            } else if (arg == "--window-size" || arg == "-w") {
                if (i + 1 >= argc) {
                    throw invalid_argument("Missing value for --window-size");
                }
                WINDOW_SIZE = stoi(argv[++i]);
                if (WINDOW_SIZE < 1) {
                    throw out_of_range("Window size must be greater than 0");
                }
            } else if (arg == "--grams" || arg == "-g") {
                if (i + 1 >= argc) {
                    throw invalid_argument("Missing value for --grams");
                }
                GRAMS = stoi(argv[++i]);
                if (GRAMS < 1) {
                    throw out_of_range("Grams must be greater than 0");
                }
            } else if (arg == "--prime" || arg == "-p") {
                if (i + 1 >= argc) {
                    throw invalid_argument("Missing value for --prime");
                }
                PRIME = stoi(argv[++i]);
                if (PRIME < 1) {
                    throw out_of_range("Prime must be greater than 0");
                }
            } else if (arg == "--help" || arg == "-h") {
                showUsage();
                return 0;
            } else {
                throw invalid_argument("Unknown option: " + arg);
            }
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n\n";
        showUsage();
        return 1;
    }

    try {
        if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
            cerr << "Invalid directory: " << dirPath << '\n';
            return 1;
        }

        string parentPath = "reports";

        if (removeDirectory(parentPath)) {  
            cout << "Removed old report directory: " << parentPath << '\n';  
        }
        if (createDirectory(parentPath)) {
            cout << "Created new report directory: " << parentPath << '\n';
        }


        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.is_directory()) {
                auto childDirPath = entry.path();

                clear();

                for (const auto& childEntry : fs::directory_iterator(childDirPath)) {
                    if (childEntry.is_regular_file()) {
                        auto filePath = childEntry.path();
                        auto fileName = filePath.filename().string();
                        vector<string> temp = splitStringByDelimiter(fileName, '_');

                        auto file = make_shared<ifstream>(filePath);
                        if (!file->is_open()) {
                            cerr << "Failed to open file: " << filePath << '\n';
                            continue;
                        }

                        string code((istreambuf_iterator<char>(*file)), istreambuf_iterator<char>());
                        if (!code.empty()) {
                            code.erase(find_if(code.rbegin(), code.rend(), [](unsigned char ch) {
                                return !isspace(ch);
                            }).base(), code.end());
                        }

                        if (temp[0][0] == '[') {
                            if (temp.size() < 5) {
                                cerr << "Invalid file name format: " << fileName << '\n';
                                continue;
                            }

                            if (temp[2] != "AC") continue; // Skip non-accepted submissions
                            files.push_back(file);

                            while (temp.size() > 5) {
                                temp[3] += "_" + temp[4];
                                temp.erase(temp.begin() + 4);
                            }

                            temp[4].erase(temp[4].find('.'));
                            temp[0].erase(0, 1);
                            temp[0].pop_back();

                            submissions.push_back({temp[1], temp[2], temp[3], temp[4], code, temp[0]});
                        } else {
                            if (temp.size() < 4) {
                                cerr << "Invalid file name format: " << fileName << '\n';
                                continue;
                            }

                            if (temp[1] != "AC") continue;
                            files.push_back(file);

                            while (temp.size() > 4) {
                                temp[2] += "_" + temp[3];
                                temp.erase(temp.begin() + 3);
                            }

                            temp[3].erase(temp[3].find('.'));

                            submissions.push_back({temp[0], temp[1], temp[2], temp[3], code});
                        }
                    }
                }

                // Process submissions for this subdirectory
                cout << "Processing submissions for directory: " << childDirPath << "...\n";

                try {

                    

                    string childPath = "reports/" + childDirPath.filename().string();


                    if (createDirectory(childPath)) {
                        cout << "Created new report directory: " << childPath << '\n';
                    }


                    // Generate reports
                    Compare(childPath);
                    exportCSV(childPath);
                    ExportHTML(childPath);
                    ExportParticipantsCSV(childPath);
                    ExportPairsOccurences(childPath);
                } catch (const exception& e) {
                    cerr << "Error processing reports for directory " << childDirPath << ": " << e.what() << '\n';
                }
            }
        }

        ExportIndex();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
    }

    return 0;
}
