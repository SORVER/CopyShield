import requests
import hashlib
import json
import time
import random

rand = random.randint(0, 100000)
rand = str(rand).zfill(6)
current_time = str(int(time.time()))

with open('settings.json', 'r') as f:
    settings = json.load(f)

api_key = settings['api_key']
api_secret = settings['api_secret']
group_code = settings['group_code']
contest_id = settings['contest_id']

api_sig = rand + '/contest.status?apiKey=' + api_key + '&contestId=' + contest_id + '&groupCode=' + group_code + '&time=' + current_time + '#' + api_secret
hash = hashlib.sha512(api_sig.encode()).hexdigest()
data = requests.get(f'https://codeforces.com/api/contest.status?groupCode={group_code}&contestId={contest_id}&apiKey={api_key}&time={current_time}&apiSig={rand+hash}').json()


# get the submissions in json file 
with open('submissions.json', 'w') as f:
    json.dump(data, f, indent=4)
    print("submissions.json created successfully")

