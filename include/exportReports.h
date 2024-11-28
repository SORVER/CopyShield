#pragma once 

#include <iostream>
#include <string>

using namespace std;

void exportCSV(string reportDir);
void ExportHTML(string reportDir);
void ExportParticipantsCSV(string reportDir);  // contains handle and int for number of occurences
void ExportPairsOccurences(string reportDir);  // contains username1, username2, # of occurences of the pair
void ExportIndex();  // contains links to all problems' reports :)