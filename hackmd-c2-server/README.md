# HackMD C2 Server

A command and control server built on HackMD that allows remote command execution through collaborative notes.

## Overview

This project provides a server interface for managing and executing commands on targets through HackMD notes. It uses the HackMD API to create a communication channel between the server and connected targets.

## Features

- Target management and tracking
- Remote command execution via HackMD notes
- Interactive shell interface for command execution
- Note-based command/response system
- Persistent target configuration using JSON

## Requirements

- Python 3.10+
- PyHackMD 1.0.2

## Installation

1. Clone the repository
2. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```

3. Set up your HackMD API key in a `.dotenv` file:
   ```
   API_KEY=your_api_key_here
   ```

## Configuration

Define your targets in `targets.json`:
```json
{
  "target_name": {
    "name": "target_name",
    "note_id": "your_note_id",
    "real_time_note_id": "another_note_id"
  }
}
```

## Usage

Start the server:
```bash
python server.py
```

The server provides an interactive shell with commands:
- `current` - Show the current target
- `exit` - Exit the shell
- Any other input is executed as a command on the target

## Project Structure

- `server.py` - Main server application
- `lib/command.py` - Command execution management
- `lib/notes.py` - HackMD note management
- `lib/__init__.py` - Shared utilities and data structures
- `targets.json` - Target configuration
- `requirements.txt` - Python dependencies

## License

This project is part of MeowWare.
