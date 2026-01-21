#include "command.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include <string>

namespace {
static const std::string BASE64_CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static std::string base64_encode(const std::string& input) {
    std::string out;
    int val = 0;
    int valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(BASE64_CHARS[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        out.push_back(BASE64_CHARS[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (out.size() % 4) {
        out.push_back('=');
    }
    return out;
}
}

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
    std::string encoded = base64_encode(response);
    std::string packed = HackmdData::pack(STATUS_READY, "", encoded, "");
    api->update_note(note_id, &packed);
}
