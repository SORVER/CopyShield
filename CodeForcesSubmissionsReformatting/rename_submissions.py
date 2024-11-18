import os
import json

def rename_files():
    submissions_folder = 'submissions'

    with open('submissions.json') as f:
        data = json.load(f)

    for submission in data['result']:
        id = submission['id']
        verdict = submission['verdict']
        authorHandle = submission['author']['members'][0]['handle']
        problemIndex = submission['problem']['index']

        if verdict == 'OK':
            verdict = 'AC'

        old_file = os.path.join(submissions_folder, f'{id}.cpp')
        new_file = os.path.join(submissions_folder, f'{id}_{verdict}_{authorHandle}_{problemIndex}.cpp')

        try:
            print(f'Renamed: {old_file} -> {new_file}')

            os.rename(old_file, new_file)
            print(f'Renamed: {old_file} -> {new_file}')
        except FileNotFoundError:
            print(f'File not found: {old_file}')
        except Exception as e:
            print(f'Error renaming {old_file}: {e}')

rename_files()
