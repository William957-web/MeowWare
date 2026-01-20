#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>
#include "api.hpp"
#include "data_types.hpp"

class CommandManager {
private:
    API* api;
    std::string note_id;

public:
    std::vector<std::string> history;
    bool recv_sigint;

    CommandManager(API* api, const std::string& note_id);

    void sigint_handler();

    void reset_terminal();

    HackmdData wait(const std::string& desired_status, int check_interval = 1);

    std::string receive_command();

    void send_response(const std::string& response);
};

#endif // COMMAND_HPP
