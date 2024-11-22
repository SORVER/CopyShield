import os
import json

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
        elif(lang.find('Python') != -1 or lang.find('Py') != -1):
            old_file = os.path.join(submissions_folder, f'{id}.py')
            new_file = os.path.join(submissions_folder, f'[{relativeTimeMinutes}]_{id}_{verdict}_{authorHandle}_{problemIndex}.py')
        elif(lang.find('C') != -1):
            old_file = os.path.join(submissions_folder, f'{id}.c')
            new_file = os.path.join(submissions_folder, f'[{relativeTimeMinutes}]_{id}_{verdict}_{authorHandle}_{problemIndex}.c')

        try:
            os.rename(old_file, new_file)
            print(f'Renamed: {old_file} -> {new_file}')
        except FileNotFoundError:
            print(f'File not found: {old_file}')
        except Exception as e:
            print(f'Error renaming {old_file}: {e}')

rename_files()
