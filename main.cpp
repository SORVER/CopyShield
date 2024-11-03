#include <bits/stdc++.h>
using namespace std;

double GetSimilarity(vector<long long> fingerPrints1, vector<long long> fingerPrints2){
    set<long long> intersection;
    set<long long> unionSet;
    for(long long i : fingerPrints1){
        unionSet.insert(i);
    }
    for(long long i : fingerPrints2){
        unionSet.insert(i);
    }
    for(long long i : fingerPrints1){
        if(find(fingerPrints2.begin(), fingerPrints2.end(), i) != fingerPrints2.end()){
            intersection.insert(i);
        }
    }
    return (double)intersection.size() / unionSet.size();
}

vector<long long> GetFingerPrints(vector<long long>hashs, int windowSize){
    vector<long long> fingerPrints;
    set<pair<long long , int>> window;  // value, index
    map<pair<long long, int>, int> count;
    int i = 0;
    for(; i<windowSize; i++){
        window.insert({hashs[i], i});
    }
    fingerPrints.push_back(window.begin()->first);
    count[*window.begin()]++;
    for(; i<hashs.size(); i++){
        window.insert({hashs[i], i});
        window.erase({hashs[i-windowSize], i-windowSize});
        count[*window.begin()]++;
        if(count[*window.begin()] == 1){
            fingerPrints.push_back(window.begin()->first);
        }
    }
    return fingerPrints;
}

vector<long long> Hash_n_Grams(vector<string> grams){
    vector<long long> hashes;
    for(int i=0; i<grams.size(); i++){
        long long hash = 0;
        int p = 31;

        for(int j=0; j<grams[i].size(); j++){
            hash += (grams[i][j] - '!' + 1) * pow(p, j);
        }
        hashes.push_back(hash);
    }
    // for(long long k : hashes){
    //     cout << k << '\n';
    // }
    return hashes;
}

vector<string> Generate_n_grams(string code, int n){
    vector<string> grams;
    for(int i=0; i+n<code.size(); i++){
        grams.push_back(code.substr(i, n));
    }
    // for(string i : grams){
    //     cout << i << '\n';
    // }
    return grams;
}

void TextProccessing(string& code){
    // remove all comments
    code = regex_replace(code, regex("//.*"), "");
    code = regex_replace(code, regex("/\\*.*?\\*/"), "");
    // code = regex_replace(code, regex("[^a-zA-Z0-9]"), " ");

    // remove all withespace
    code.erase(remove(code.begin(), code.end(), ' '), code.end());
    code.erase(remove(code.begin(), code.end(), '\n'), code.end());
    code.erase(remove(code.begin(), code.end(), '\t'), code.end());

    // transform the code to lowercase 
    transform(code.begin(), code.end(), code.begin(), ::tolower);
}

int main(){
    string code1 = "#include <iostream>\n#include <vector>\n\nvoid Merge(std::vector<int>& inputArray, int leftIndex, int middleIndex, int rightIndex) {\n    // Sizes of the two subarrays\n    int sizeLeft = middleIndex - leftIndex + 1;\n    int sizeRight = rightIndex - middleIndex;\n\n    // Create temporary arrays\n    std::vector<int> leftArray(sizeLeft);\n    std::vector<int> rightArray(sizeRight);\n\n    // Copy data to temporary arrays\n    for (int index = 0; index < sizeLeft; index++)\n        leftArray[index] = inputArray[leftIndex + index];\n    for (int indexRight = 0; indexRight < sizeRight; indexRight++)\n        rightArray[indexRight] = inputArray[middleIndex + 1 + indexRight];\n\n    // Merge the temporary arrays back into inputArray[leftIndex..rightIndex]\n    int i = 0; // Initial index of first subarray\n    int j = 0; // Initial index of second subarray\n    int k = leftIndex; // Initial index of merged subarray\n\n    while (i < sizeLeft && j < sizeRight) {\n        if (leftArray[i] <= rightArray[j]) {\n            inputArray[k] = leftArray[i];\n            i++;\n        } else {\n            inputArray[k] = rightArray[j];\n            j++;\n        }\n        k++;\n    }\n\n    // Copy the remaining elements of leftArray[], if there are any\n    while (i < sizeLeft) {\n        inputArray[k] = leftArray[i];\n        i++;\n        k++;\n    }\n\n    // Copy the remaining elements of rightArray[], if there are any\n    while (j < sizeRight) {\n        inputArray[k] = rightArray[j];\n        j++;\n        k++;\n    }\n}\n\nvoid MergeSort(std::vector<int>& inputArray, int leftIndex, int rightIndex) {\n    if (leftIndex < rightIndex) {\n        // Find the middle point\n        int middleIndex = leftIndex + (rightIndex - leftIndex) / 2;\n\n        // Sort first and second halves\n        MergeSort(inputArray, leftIndex, middleIndex);\n        MergeSort(inputArray, middleIndex + 1, rightIndex);\n\n        // Merge the sorted halves\n        Merge(inputArray, leftIndex, middleIndex, rightIndex);\n    }\n}\n\nint main() {\n    std::vector<int> inputArray = {38, 27, 43, 3, 9, 82, 10};\n\n    std::cout << \"Original array: \";\n    for (int number : inputArray) {\n        std::cout << number << \" \";\n    }\n    std::cout << std::endl;\n\n    MergeSort(inputArray, 0, inputArray.size() - 1);\n\n    std::cout << \"Sorted array: \";\n    for (int number : inputArray) {\n        std::cout << number << \" \";\n    }\n    std::cout << std::endl;\n\n    return 0;\n}";
    // string code2 = "#include <iostream>\n#include <vector>\n\n// Function to merge two halves of the array\nvoid Combine(std::vector<int>& inputArray, int startIndex, int midPointIndex, int endIndex) {\n    // Sizes of the two subarrays\n    int sizeLeft = midPointIndex - startIndex + 1;\n    int sizeRight = endIndex - midPointIndex;\n\n    // Create temporary arrays\n    std::vector<int> leftArray(sizeLeft);\n    std::vector<int> rightArray(sizeRight);\n\n    // Copy data to temporary arrays\n    for (int indexLeft = 0; indexLeft < sizeLeft; indexLeft++)\n        leftArray[indexLeft] = inputArray[startIndex + indexLeft];\n    for (int indexRight = 0; indexRight < sizeRight; indexRight++)\n        rightArray[indexRight] = inputArray[midPointIndex + 1 + indexRight];\n\n    // Merge the temporary arrays back into inputArray[startIndex..endIndex]\n    int indexLeft = 0;  // Initial index of the left subarray\n    int indexRight = 0; // Initial index of the right subarray\n    int indexMerged = startIndex; // Initial index of merged subarray\n\n    while (indexLeft < sizeLeft && indexRight < sizeRight) {\n        if (leftArray[indexLeft] <= rightArray[indexRight]) {\n            inputArray[indexMerged] = leftArray[indexLeft];\n            indexLeft++;\n        } else {\n            inputArray[indexMerged] = rightArray[indexRight];\n            indexRight++;\n        }\n        indexMerged++;\n    }\n\n    // Copy the remaining elements of leftArray[], if there are any\n    while (indexLeft < sizeLeft) {\n        inputArray[indexMerged] = leftArray[indexLeft];\n        indexLeft++;\n        indexMerged++;\n    }\n\n    // Copy the remaining elements of rightArray[], if there are any\n    while (indexRight < sizeRight) {\n        inputArray[indexMerged] = rightArray[indexRight];\n        indexRight++;\n        indexMerged++;\n    }\n}\n\n// Function to implement merge sort\nvoid PerformMergeSort(std::vector<int>& inputArray, int startIndex, int endIndex) {\n    if (startIndex < endIndex) {\n        // Find the middle point\n        int midPointIndex = startIndex + (endIndex - startIndex) / 2;\n\n        // Sort first and second halves\n        PerformMergeSort(inputArray, startIndex, midPointIndex);\n        PerformMergeSort(inputArray, midPointIndex + 1, endIndex);\n\n        // Merge the sorted halves\n        Combine(inputArray, startIndex, midPointIndex, endIndex);\n    }\n}\n\nint main() {\n    std::vector<int> inputArray = {38, 27, 43, 3, 9, 82, 10};\n\n    std::cout << \"Original array: \";\n    for (int number : inputArray) {\n        std::cout << number << \" \";\n    }\n    std::cout << std::endl;\n\n    PerformMergeSort(inputArray, 0, inputArray.size() - 1);\n\n    std::cout << \"Sorted array: \";\n    for (int number : inputArray) {\n        std::cout << number << \" \";\n    }\n    std::cout << std::endl;\n\n    return 0;\n}";
    // string code2 = "#include <iostream>\n#include <vector>\n\n// Function to perform merge sort\nvoid PerformMergeSort(std::vector<int>& inputArray, int startIndex, int endIndex);\n\n// Function to merge two halves of the array\nvoid Combine(std::vector<int>& inputArray, int startIndex, int midPointIndex, int endIndex) {\n    // Sizes of the two subarrays\n    int sizeLeft = midPointIndex - startIndex + 1;\n    int sizeRight = endIndex - midPointIndex;\n\n    // Create temporary arrays\n    std::vector<int> leftArray(sizeLeft);\n    std::vector<int> rightArray(sizeRight);\n\n    // Copy data to temporary arrays\n    for (int indexLeft = 0; indexLeft < sizeLeft; indexLeft++)\n        leftArray[indexLeft] = inputArray[startIndex + indexLeft];\n    for (int indexRight = 0; indexRight < sizeRight; indexRight++)\n        rightArray[indexRight] = inputArray[midPointIndex + 1 + indexRight];\n\n    // Merge the temporary arrays back into inputArray[startIndex..endIndex]\n    int indexLeft = 0;  // Initial index of the left subarray\n    int indexRight = 0; // Initial index of the right subarray\n    int indexMerged = startIndex; // Initial index of merged subarray\n\n    while (indexLeft < sizeLeft && indexRight < sizeRight) {\n        if (leftArray[indexLeft] <= rightArray[indexRight]) {\n            inputArray[indexMerged] = leftArray[indexLeft];\n            indexLeft++;\n        } else {\n            inputArray[indexMerged] = rightArray[indexRight];\n            indexRight++;\n        }\n        indexMerged++;\n    }\n\n    // Copy the remaining elements of leftArray[], if there are any\n    while (indexLeft < sizeLeft) {\n        inputArray[indexMerged] = leftArray[indexLeft];\n        indexLeft++;\n        indexMerged++;\n    }\n\n    // Copy the remaining elements of rightArray[], if there are any\n    while (indexRight < sizeRight) {\n        inputArray[indexMerged] = rightArray[indexRight];\n        indexRight++;\n        indexMerged++;\n    }\n}\n\nint main() {\n    std::vector<int> inputArray = {38, 27, 43, 3, 9, 82, 10};\n\n    std::cout << \"Original array: \";\n    for (int number : inputArray) {\n        std::cout << number << \" \";\n    }\n    std::cout << std::endl;\n\n    PerformMergeSort(inputArray, 0, inputArray.size() - 1);\n\n    std::cout << \"Sorted array: \";\n    for (int number : inputArray) {\n        std::cout << number << \" \";\n    }\n    std::cout << std::endl;\n\n    return 0;\n}\n\nvoid PerformMergeSort(std::vector<int>& inputArray, int startIndex, int endIndex) {\n    if (startIndex < endIndex) {\n        // Find the middle point\n        int midPointIndex = startIndex + (endIndex - startIndex) / 2;\n\n        // Sort first and second halves\n        PerformMergeSort(inputArray, startIndex, midPointIndex);\n        PerformMergeSort(inputArray, midPointIndex + 1, endIndex);\n\n        // Merge the sorted halves\n        Combine(inputArray, startIndex, midPointIndex, endIndex);\n    }\n}";


    // string code1 = "using System;\n\nclass Program\n{\n    static void Main(string[] args)\n    {\n        string strOne = \"AGGTAB\";\n        string strTwo = \"GXTXAYB\";\n        \n        int lcsLength = calculateLCS(strOne, strTwo);\n        Console.WriteLine(\"Length of Longest Common Subsequence: \" + lcsLength);\n    }\n\n    static int calculateLCS(string strOne, string strTwo)\n    {\n        int lengthStrOne = strOne.Length;\n        int lengthStrTwo = strTwo.Length;\n        \n        // Create a 2D array to store lengths of longest common subsequence.\n        int[,] dpArray = new int[lengthStrOne + 1, lengthStrTwo + 1];\n\n        // Build the dpArray in a bottom-up fashion\n        for (int indexStrOne = 1; indexStrOne <= lengthStrOne; indexStrOne++)\n        {\n            for (int indexStrTwo = 1; indexStrTwo <= lengthStrTwo; indexStrTwo++)\n            {\n                // If characters match, add 1 to the diagonal value\n                if (strOne[indexStrOne - 1] == strTwo[indexStrTwo - 1])\n                {\n                    dpArray[indexStrOne, indexStrTwo] = dpArray[indexStrOne - 1, indexStrTwo - 1] + 1;\n                }\n                else\n                {\n                    // Otherwise, take the maximum of left or top cell\n                    dpArray[indexStrOne, indexStrTwo] = Math.Max(dpArray[indexStrOne - 1, indexStrTwo], dpArray[indexStrOne, indexStrTwo - 1]);\n                }\n            }\n        }\n\n        // The length of the longest common subsequence is in the bottom-right cell\n        return dpArray[lengthStrOne, lengthStrTwo];\n    }\n}";
    string code2 = "using System;\n\npublic class LCS\n{\n    // Method to compute LCS using recursion and memoization\n    public int LongestCommonSubsequence(string textOne, string textTwo)\n    {\n        // Create a memoization table to store results of subproblems\n        int[,] memo = new int[textOne.Length + 1, textTwo.Length + 1];\n        for (int indexOne = 0; indexOne <= textOne.Length; indexOne++)\n            for (int indexTwo = 0; indexTwo <= textTwo.Length; indexTwo++)\n                memo[indexOne, indexTwo] = -1; // Initialize memo table with -1\n\n        return LCSRecursive(textOne, textTwo, textOne.Length, textTwo.Length, memo);\n    }\n\n    // Recursive helper method for LCS\n    private int LCSRecursive(string textOne, string textTwo, int lengthOne, int lengthTwo, int[,] memo)\n    {\n        // Base case: If either string is empty, LCS is 0\n        if (lengthOne == 0 || lengthTwo == 0)\n            return 0;\n\n        // Check if result is already computed\n        if (memo[lengthOne, lengthTwo] != -1)\n            return memo[lengthOne, lengthTwo];\n\n        // If characters match, move to the next characters in both strings\n        if (textOne[lengthOne - 1] == textTwo[lengthTwo - 1])\n        {\n            memo[lengthOne, lengthTwo] = 1 + LCSRecursive(textOne, textTwo, lengthOne - 1, lengthTwo - 1, memo);\n        }\n        else\n        {\n            // If characters do not match, take the maximum of excluding one character\n            memo[lengthOne, lengthTwo] = Math.Max(\n                LCSRecursive(textOne, textTwo, lengthOne - 1, lengthTwo, memo),\n                LCSRecursive(textOne, textTwo, lengthOne, lengthTwo - 1, memo)\n            );\n        }\n\n        return memo[lengthOne, lengthTwo]; // Return the computed value\n    }\n}\n\n// Example usage\npublic class Program\n{\n    public static void Main(string[] args)\n    {\n        LCS lcs = new LCS();\n        string textOne = \"AGGTAB\";\n        string textTwo = \"GXTXAYB\";\n        int length = lcs.LongestCommonSubsequence(textOne, textTwo);\n        Console.WriteLine(\"Length of LCS is: \" + length);\n    }\n}";


    
    TextProccessing(code1);
    TextProccessing(code2);

    vector<long long> hashes1 = Hash_n_Grams(Generate_n_grams(code1, 5));
    vector<long long> fingerPrints1 = GetFingerPrints(hashes1, 5);

    vector<long long> hashes2 = Hash_n_Grams(Generate_n_grams(code2, 5));
    vector<long long> fingerPrints2 = GetFingerPrints(hashes2, 5);


    double similarity = GetSimilarity(fingerPrints1, fingerPrints2);
    cout << fixed << setprecision(2) << "Similarity: " << similarity * 100 << "%" << endl;
    
    // cout << code << endl;
    return 0;
}