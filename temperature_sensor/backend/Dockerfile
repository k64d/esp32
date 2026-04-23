# Use Python 3.11 slim image as base
FROM python:3.11-slim

# Set working directory
WORKDIR /app

# Copy requirements.txt and install dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
RUN pip install --no-cache-dir flask google-auth google-auth-oauthlib google-auth-httplib2 google-api-python-client

# Copy application files
COPY api_server.py .
COPY sheet-credentials.json .

# Set environment variables
ENV FLASK_APP=api_server.py
ENV GOOGLE_CREDENTIALS_PATH=/app/sheet-credentials.json
ENV SPREADSHEET_ID=${SPREADSHEET_ID:-YOUR_SPREADSHEET_ID}
ENV SHEET_NAME=${SHEET_NAME:-Sheet1}
ENV COLUMN_RANGE=${COLUMN_RANGE:-A}

# Expose port 8182
EXPOSE 8182

# Run the Flask application
CMD ["python", "-m", "flask", "run", "--host=0.0.0.0", "--port=8182"]
