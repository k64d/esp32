#!/usr/bin/env python3
import logging
import os
from datetime import datetime
from flask import Flask, request
from google.oauth2.service_account import Credentials
from google.auth.transport.requests import Request
from googleapiclient.discovery import build

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

app = Flask(__name__)

# Google Sheets configuration
SPREADSHEET_ID = os.getenv('SPREADSHEET_ID', 'YOUR_SPREADSHEET_ID')
SHEET_NAME = os.getenv('SHEET_NAME', 'Sheet1')
COLUMN_RANGE = os.getenv('COLUMN_RANGE', 'A')  # Column to append to (A, B, C, etc.)
CREDENTIALS_PATH = os.getenv('GOOGLE_CREDENTIALS_PATH', 'credentials.json')

# Initialize Google Sheets service
def get_sheets_service():
    """Initialize and return Google Sheets API service"""
    try:
        credentials = Credentials.from_service_account_file(
            CREDENTIALS_PATH,
            scopes=['https://www.googleapis.com/auth/spreadsheets']
        )
        return build('sheets', 'v4', credentials=credentials)
    except Exception as e:
        logger.error(f"Failed to initialize Google Sheets service: {e}")
        return None

def append_to_sheet(message, service=None):
    """Append message to Google Sheet"""
    if not service:
        service = get_sheets_service()
    
    if not service:
        logger.error("Google Sheets service not available")
        return False
    
    try:
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        range_name = f"{SHEET_NAME}!{COLUMN_RANGE}:D"  # Append to columns A through D
        values = [[timestamp, message]]
        
        body = {'values': values}
        result = service.spreadsheets().values().append(
            spreadsheetId=SPREADSHEET_ID,
            range=range_name,
            valueInputOption='USER_ENTERED',
            body=body
        ).execute()
        
        logger.info(f"Successfully appended to Google Sheet: {message}")
        return True
    except Exception as e:
        logger.error(f"Failed to append to Google Sheet: {e}")
        return False

@app.route('/api/message', methods=['GET'])
def receive_message():
    """
    Endpoint that listens to incoming strings and logs them to Google Sheet.
    Expected query parameter: message=your string here
    """
    incoming_string = request.args.get('message')
    if incoming_string:
        logger.info(f"Received message: {incoming_string}")
        service = get_sheets_service()
        append_to_sheet(incoming_string, service)
    return '', 200

@app.route('/health', methods=['GET'])
def health_check():
    """Health check endpoint"""
    return {'status': 'healthy'}, 200

if __name__ == '__main__':
    logger.info("Starting server on port 8080...")
    app.run(host='0.0.0.0', port=8080, debug=True)
