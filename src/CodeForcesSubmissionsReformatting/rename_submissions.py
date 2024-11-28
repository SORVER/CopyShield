import os
import json
import re
import shutil

def rename_files():
    submissions_folder = os.path.join(os.path.dirname(__file__), "submissions")

    jsonPath = os.path.join(os.path.dirname(__file__), "submissions.json")


    with open(jsonPath) as f:
        data = json.load(f)

    for submission in data['result']:
        id = submission['id']
        verdict = submission['verdict']
        authorHandle = submission['author']['members'][0]['handle']
        problemIndex = submission['problem']['index']
        relativeTimeMinutes = submission['relativeTimeSeconds'] // 60
        lang = submission['programmingLanguage']

        if verdict == 'OK':
            verdict = 'AC'


        if(lang.find('C++') != -1):
            old_file = os.path.join(submissions_folder, f'{id}.cpp')
            new_file = os.path.join(submissions_folder, f'[{relativeTimeMinutes}]_{id}_{verdict}_{authorHandle}_{problemIndex}.cpp')
        elif(lang.find('C#') != -1):
            old_file = os.path.join(submissions_folder, f'{id}.cs')
            new_file = os.path.join(submissions_folder, f'[{relativeTimeMinutes}]_{id}_{verdict}_{authorHandle}_{problemIndex}.cs')
        elif(lang.find('Py') != -1 or lang.find('Py') != -1):
            old_file = os.path.join(submissions_folder, f'{id}.py')
            new_file = os.path.join(submissions_folder, f'[{relativeTimeMinutes}]_{id}_{verdict}_{authorHandle}_{problemIndex}.py')
        elif(lang.find('Java') != -1):
            old_file = os.path.join(submissions_folder, f'{id}.java')
            new_file = os.path.join(submissions_folder, f'[{relativeTimeMinutes}]_{id}_{verdict}_{authorHandle}_{problemIndex}.java')
        elif(lang.find('C') != -1):
            old_file = os.path.join(submissions_folder, f'{id}.c')
            new_file = os.path.join(submissions_folder, f'[{relativeTimeMinutes}]_{id}_{verdict}_{authorHandle}_{problemIndex}.c')

        try:
            os.rename(old_file, new_file)
            # print(f'Renamed: {old_file} -> {new_file}')
        except FileNotFoundError:
            print(f'File not found: {old_file}')
        except Exception as e:
            print(f'Error renaming {old_file}: {e}')


def distribute_files_to_folders():
    submissions_folder = os.path.join(os.path.dirname(__file__), "submissions")

    
    for file in os.listdir(submissions_folder):
        if os.path.isfile(os.path.join(submissions_folder, file)):
            found = 0
            for letter in "ABCDEFGHIJKLMNOPQRSTUVWXYZ":
                match = re.search(rf"_({letter})\.", file)
                if match:
                    folder_path = os.path.join(submissions_folder, match.group(1))
                    os.makedirs(folder_path, exist_ok=True) # 
                    shutil.move(os.path.join(submissions_folder, file), os.path.join(folder_path, file))
                    found = 1
                    break
            if not found:
                folder_path = os.path.join(submissions_folder, "Uncategorized")
                os.makedirs(folder_path, exist_ok=True)
                shutil.move(os.path.join(submissions_folder, file), os.path.join(submissions_folder, "Uncategorized", file))


    print("File distribution complete.")

if __name__ == "__main__":
    rename_files()
    distribute_files_to_folders()
    
