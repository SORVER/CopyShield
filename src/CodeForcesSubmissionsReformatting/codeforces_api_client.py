import requests
import hashlib
import json
import time
import secrets
import os
from dotenv import load_dotenv

load_dotenv()

def get_env_variable(name):
    value = os.getenv(name)
    if value is None:
        raise EnvironmentError(f"Environment variable {name} is missing.") 
    return value

def generate_random_string():
    rand = secrets.randbelow(1000000)
    return str(rand).zfill(6)

def create_api_signature(rand, api_key, contest_id, group_code, current_time, api_secret):
    api_sig = rand + '/contest.status?apiKey=' + api_key + '&contestId=' + contest_id + '&groupCode=' + group_code + '&time=' + current_time + '#' + api_secret
    return hashlib.sha512(api_sig.encode()).hexdigest()

def fetch_contest_data(api_key, contest_id, group_code, current_time, api_secret, rand):
    signature = create_api_signature(rand, api_key, contest_id, group_code, current_time, api_secret)
    url = f'https://codeforces.com/api/contest.status?groupCode={group_code}&contestId={contest_id}&apiKey={api_key}&time={current_time}&apiSig={rand + signature}'
    
    response = requests.get(url)
    
    if response.status_code == 200:
        return response.json()
    else:
        raise Exception(f"Failed to fetch contest data. Status code: {response.status_code}, Response: {response.text}")

def save_to_json(data, file_path):
    with open(file_path, 'w') as f:
        json.dump(data, f, indent=4)
    print(f"Data successfully saved to {file_path}")

def main():
    try:
        api_key = get_env_variable('CODEFORCES_API_KEY')
        api_secret = get_env_variable('CODEFORCES_API_SECRET')
        group_code = get_env_variable('CODEFORCES_GROUP_CODE')
        contest_id = get_env_variable('CODEFORCES_CONTEST_ID')

        current_time = str(int(time.time()))
        rand = generate_random_string()

        data = fetch_contest_data(api_key, contest_id, group_code, current_time, api_secret, rand)

        path2 = os.path.join(os.path.dirname(__file__), "submissions.json")
        save_to_json(data, path2)

    except Exception as e:
        print(f"Error occurred: {e}")

if __name__ == "__main__":
    main()
