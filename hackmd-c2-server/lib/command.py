from PyHackMD import API
from time import sleep
from . import *


class CommandManager:
    history: list[str] = []
    recv_sigint: bool = False

    def __init__(self, api: API, note_id: str):
        self.api = api
        self.note_id = note_id

    def sigint_handler(self):
        self.recv_sigint = True

    def reset_terminal(self):
        self.api.update_note(self.note_id, content=INIT_CONTENT)

    def wait(self, desired_status: str, check_interval: int = 1) -> HackmdData:
        while True:
            sleep(check_interval)
            if self.recv_sigint:
                self.recv_sigint = False
                self.reset_terminal()
                return ""

            note = self.api.get_note(self.note_id)
            note_content = HackmdData.unpack(note['content'])
            status = note_content.status

            if status == desired_status:
                break
        return note_content

    def send_command(self, command: str):
        self.wait(STATUS_READY)
        self.history.append(command)
        self.api.update_note(self.note_id, content=HackmdData.pack(
            request=command, response="", status=STATUS_EXECUTING, save_path=""))

    def receive_command(self) -> str:
        note_content = self.wait(STATUS_EXECUTING)
        return note_content.request

    def receive_response(self) -> str:
        note_content = self.wait(STATUS_READY)
        return note_content.response

    def send_response(self, response: str):
        self.wait(STATUS_EXECUTING)
        self.api.update_note(self.note_id, content=HackmdData.pack(
            request="", response=response, status=STATUS_READY, save_path=""))

    def execute_command(self, command: str) -> str:
        self.send_command(command)
        print(self.receive_response())
