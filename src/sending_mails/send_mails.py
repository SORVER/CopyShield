import csv
from mailjet_rest import Client
import json
import os
from dotenv import load_dotenv


load_dotenv()

def read_csv(file_path):
    """Read data from a CSV file and return as a list of dictionaries."""
    with open(file_path, mode='r') as file:
        reader = csv.DictReader(file)
        return [row for row in reader]

def send_email(sender_email, recipient_email, subject, body, api_key, api_secret):
    """Send an email using Mailjet's API."""
    try:
       
        mailjet = Client(auth=(api_key, api_secret), version='v3.1')

        data = {
            'Messages': [
                {
                    "From": {
                        "Email": sender_email,
                        "Name": "SHAC ICPC Coaches"
                    },
                    "To": [
                        {
                            "Email": recipient_email
                        }
                    ],
                    "Subject": subject,
                    "TextPart": body
                }
            ]
        }

        response = mailjet.send.create(data=data)
        if response.status_code == 200:
            print(f"Email sent to {recipient_email}.")
        else:
            print(f"Failed to send email to {recipient_email}. Error: {response.status_code} - {response.text}")

    except Exception as e:
        print(f"Error sending email to {recipient_email}. Error: {e}")

def process_and_email(handles_file, flags_file, api_key, api_secret):
    """Process handles and send emails based on flags."""
    handles_data = read_csv(handles_file)
    flags_data = read_csv(flags_file)

    flags_dict = {row['Handle']: row['Flag'] == 'True' for row in flags_data}
    missing_handles = [] 
    for entry in handles_data:
        handle = entry['Handle']
        email = entry['Email']
        name = entry['Name']

        if handle not in flags_dict:
            missing_handles.append(handle)
            continue
        
        if flags_dict[handle]:
            subject = "SHAC ICPC Training: Cheating Alert"
            body = (
                f"Dear {name},\n\n"
                f"We have detected that you have been flagged for cheating or assisting others in cheating during the Codeforces Week Sheet. "
                f"This behavior violates the principles of fair competition and the ethics of SHAC ICPC training.\n\n"
                f"If you continue engaging in such actions, you will be permanently banned from the training program.\n\n"
                f"If you believe this is a mistake, contact the SHAC ICPC Coaches immediately.\n\n"
                f"Best regards,\n"
                f"SHAC ICPC Coaches"
            )
            send_email(sender_email, email, subject, body, api_key, api_secret)

    if missing_handles:
        print("The following handles were not found in the flags file:")
        for handle in missing_handles:
            print(f"- {handle}")

if __name__ == "__main__":
  
    handles_csv = "group_data.csv" 
    flags_csv = "../../reports/participants.csv" 

   
    api_secret = os.getenv('MAILJET_API_SECRET')
    api_key = os.getenv('MAILJET_API_KEY')
    sender_email = os.getenv('MAILJET_SENDER_EMAIL')


    process_and_email(handles_csv, flags_csv, api_key, api_secret)
