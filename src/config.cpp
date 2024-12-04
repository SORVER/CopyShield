#include "../include/config.h"

int THRESHOLD = 40;
int WINDOW_SIZE = 5;
int GRAMS = 3;
int PRIME = 7;
int CODEBLOCK_SIZE = 10;
string SHEETNAME = "";

map<string, string> diff;  // the diff of the two codes in the pair SubmissionId1_SubmissionId2
map<string, string> similar; 