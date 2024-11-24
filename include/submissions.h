#pragma once

#include <string>
#include <vector>

using namespace std;


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

        int timeInMinutes = stoi(relativeTime);
        int days = timeInMinutes / 1440;
        int hours = (timeInMinutes % 1440) / 60;
        int minutes = timeInMinutes % 60;
        this->relativeTime = to_string(days) + ":" + (hours < 10 ? "0" : "") + to_string(hours) + ":" + (minutes < 10 ? "0" : "") + to_string(minutes);        
    }

    submission(string SubmissionId, string verdict, string username, string problem, string code){
        this->verdict = verdict;
        this->username = username;
        this->problem = problem;
        this->SubmissionId = SubmissionId;
        this->code = code;
    }
};

extern vector<submission> submissions;
extern vector<pair<pair<submission, submission>, double>> similarSubmissions;