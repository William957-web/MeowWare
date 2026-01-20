#include "command.hpp"
#include <chrono>
#include <iostream>
#include <thread>

CommandManager::CommandManager(API* api, const std::string& note_id)
    : api(api), note_id(note_id), recv_sigint(false) {}

void CommandManager::sigint_handler() {
    recv_sigint = true;
}

void CommandManager::reset_terminal() {
    api->update_note(note_id, &INIT_CONTENT);
}

HackmdData CommandManager::wait(const std::string& desired_status,
                                int check_interval) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(check_interval));

        if (recv_sigint) {
            recv_sigint = false;
            reset_terminal();
            return HackmdData();
        }

        json note = api->get_note(note_id);
        std::string content = note["content"].get<std::string>();
        HackmdData note_content = HackmdData::unpack(content);
        std::string status = note_content.status;

        if (status == desired_status) {
            return note_content;
        }
    }
}

std::string CommandManager::receive_command() {
    HackmdData note_content = wait(STATUS_EXECUTING);
    return note_content.request;
}

void CommandManager::send_response(const std::string& response) {
    wait(STATUS_EXECUTING);
    std::string packed = HackmdData::pack(STATUS_READY, "", response, "");
    api->update_note(note_id, &packed);
}
