import csv
from mailjet_rest import Client
import os
from dotenv import load_dotenv

load_dotenv()

def read_csv(file_path):
    """Read data from a CSV file and return as a list of dictionaries."""
    try:
        with open(file_path, mode='r') as file:
            reader = csv.DictReader(file)
            return [row for row in reader]
    except FileNotFoundError:
        print(f"Error: File not found - {file_path}")
        return []
    except Exception as e:
        print(f"Error reading CSV file {file_path}: {e}")
        return []

def send_email(sender_email, recipient_email, subject, body, api_key, api_secret):
    print(f"Sending email to {recipient_email}...")
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
        print(f"Error sending email to {recipient_email}: {e}")

def process_and_email(handles_file, flags_file, api_key, api_secret):
    """Process handles and send emails based on flags."""
    if not api_key or not api_secret:
        print("Error: Missing Mailjet API key or secret. Ensure they are set in the environment variables.")
        return

    handles_data = read_csv(handles_file)
    flags_data = read_csv(flags_file)

    if not handles_data:
        print("Error: No data found in handles CSV file.")
        return

    if not flags_data:
        print("Error: No data found in flags CSV file.")
        return

    flags_dict = {
        row.get('Handle'): (row.get('MaxProblem'), row.get('Flag') == 'True') 
        for row in flags_data
    }
    missing_handles = []

    for entry in handles_data:
        handle = entry.get('Handle')
        email = entry.get('Email')
        name = entry.get('Name')

        if not handle or not email or not name:
            print(f"Warning: Missing data in row: {entry}")
            continue

        if handle not in flags_dict:
            missing_handles.append(handle)
            continue

        # print (f"Flagged handle: {handle}")
        # for handle, flag in flags_dict.items():
        #     print(f"{handle}: {flag}")
        if flags_dict[handle]:
            problem = flags_dict[handle][0]
            print(f"Flagged handle: {handle}")
            subject = "CIA SHA ICPC Training: "
            body = (
                f"Dear {name},\n\n"
                f"You are being watched by the CIA (Codeforces Integrity Agency) members in SHA, and they’ve noticed some *unusual activity* in problem {problem} during the training. "
                f"We know you’re brilliant, but even brilliance has its limits so let’s keep things honest, shall we? 🕵️‍♂️\n\n"
                f"Remember, this training is about learning and growing, not shortcuts. If you think there’s been a mistake, reach out to the SHAC ICPC Coaches. "
                f"We’ll be happy to clear things up (and no, we’re not secretly spying on you probably). 😉\n\n"
                f"Let’s focus on fair competition and having fun!\n\n"
                f"Best regards,\n"
                f"The SHAC ICPC Coaches (with honorary CIA badges!)"
            )
            send_email(sender_email, email, subject, body, api_key, api_secret)

    if missing_handles:
        print("The following handles were not found in the flags file:")
        for handle in missing_handles:
            print(f"- {handle}")

if __name__ == "__main__":
    try:
        handles_csv = os.path.abspath("./sending_mails/group_data.csv")
        flags_csv = os.path.abspath("reports/participants.csv")

        api_secret = os.getenv('MAILJET_API_SECRET')
        api_key = os.getenv('MAILJET_API_KEY')
        sender_email = os.getenv('MAILJET_SENDER_EMAIL')

        if not sender_email:
            raise ValueError("MAILJET_SENDER_EMAIL environment variable is not set.")

        process_and_email(handles_csv, flags_csv, api_key, api_secret)
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
