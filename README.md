# CopyShield

## Table of Contents

- [What is CopyShield ?](#what-is-copyshield-)
- [Report Generation](#report-generation)
  * [CSV Reports](#csv-reports)
  * [HTML Report](#html-report)
- [How it works ?](#how-it-works-)
- [Usage](#usage)
  * [Setting up the environment](#setting-up-the-environment)
  * [Fetching Submissions](#fetching-submissions)
    + [**Vjudge**](#--vjudge--)
    + [**CodeForces**](#--codeforces--)
  * [Compile cpp code](#compile-cpp-code)
  * [Getting the reports](#getting-the-reports)
- [Options](#options)
    * [Example](#example)
- [TODO](#todo)

## What is CopyShield ?

CopyShield is a simple Plagiarism Detection tool, which reads collection of documents and checks for similarity between them. It  can be used to detect plagiarism in documents or source codes.

## Report Generation

### CSV Reports

The report generation feature creates three separate files with detailed information as follows:

1. **Detected Plagiarism and Similarities**: This file contains the pairs of files that are flagged as likely duplicates along with the similarity percentage.

2. **Pairwise Similarities**: This file contains list of similarity percentages between each pair of participants.

3. **Participant Plagiarism Scores**: This file contains the plagiarism score of each participant.



### HTML Report

The program generates an HTML report containing the code snippets of the all pairs of files that are flagged as likely duplicates.

![HTML report](https://github.com/saifadin1/CopyShield/blob/main/res/image2.png)

![HTML report](https://github.com/saifadin1/CopyShield/blob/main/res/image4.png)

![HTML report](https://github.com/saifadin1/CopyShield/blob/main/res/image3.png)



## How it works ?

1. **Text Preprocessing**: The code from each file is preprocessed to remove comments and whitespace, and all characters are converted to lowercase.

2. **n-grams Generation**: Each processed code snippet is divided into n-grams 

3. **Hashing**: The n-grams are hashed to reduce the dimensionality of the feature space.

4. **Fingerprinting**: A sliding window approach is used to create fingerprints from the hashed n-grams, allowing efficient comparison.

5. **Similarity Calculation**: The program computes Jaccard Similarity between fingerprints of each pair of files. If similarity exceeds a threshold , it flags the files as likely duplicates.

## Usage

### Setting up the environment

1. Clone the repository

```bash
git clone https://github.com/saifadin1/CopyShield.git
```

2. Install the required packages

```bash
pip install -r requirements.txt
```

3. Create the `.env` file: Copy the contents of the `.env.example` file to create a new `.env` file in the project root directory and set the required environment variables if needed.




### Fetching Submissions

First, the submissions should be fetched from the online judge (Vjudge or CodeForces especially).

#### **Vjudge**

 Simply download the submissions from the contest page as a zip file and files names will be formatted correctly as: `<submission Id>_<Verdict>_<username>_<problem name>`

#### **CodeForces**

Similarly, download the submissions as a zip file from the contest page. However, there's a slight issue: the filenames are not formatted as needed. To fix this, we need to reformat them to match the required format:` <submission Id>_<Verdict>_<username>_<problem name>`.
[`CodeForcesSubmissionsReformatting`](https://github.com/saifadin1/CopyShield/tree/main/src/CodeForcesSubmissionsReformatting)
this directory contains two scripts to help you with that:
1. `codeforces_api_client.py` : this script will fetch the metadata of the submissions and save it in a json file.
2. `rename_submissions.py` : this script will rename the files in  `./src/CodeForcesSubmissionsReformatting/submissions` to be formatted so the fetched submissions should be in this path.



### Compile cpp code

```bash
g++ -std=c++17 main.cpp -o main
```

2. Run the compiled code using the following command:
```bash
.\src\main .\<path to the directory containing the files to be checked>
```

### Getting the reports

The reports will be generated in `./reports` directory. 


## Options 

* Set the threshold value for similarity
    ```bash
    --threshold, -t <value>
    ```

* Set the window size for fingerprinting
    ```bash
    --window-size, -w <value>
    ```

* Set the n-gram size
    ```bash
    --grams, -g <value>
    ```

* Exclude specific files (problem) 
    ```bash
    --exclude-problems, -e <problem1,problem2,...>
    ```

* Include only specific files (problem)
    ```bash
    --include-problems, -i <problem1,problem2,...>
    ```

* Include only specific users
    ```bash
    --include-users, -u <user1,user2,...>
    ```

* Display the help message showing the available options and their descriptions
    ```bash
    --help, -h
    ```

### Example

```bash
.\src\main .\problems -t 70 -w 5 -g 3 -p 101 -e problem1,problem2
```

## TODO

- [x] Add support for highlighting the similer blocks in the HTML report
- [ ] Add better hashing function
- [ ] Add more efficient similarity calculation algorithm







