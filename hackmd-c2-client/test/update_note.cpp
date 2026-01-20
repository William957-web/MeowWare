#include <iostream>
#include <cstdlib>
#include "../src/api.hpp"

const std::string NOTE_ID = "Kx43mmtdQkCzWnpy-tBOMA";

int main() {
    const char* api_key_env = std::getenv("API_KEY");
    if (!api_key_env) {
        std::cerr << "Error: API_KEY environment variable not set" << std::endl;
        return 1;
    }
    std::string api_key(api_key_env);
    
    API api(api_key);
    try {
        std::string new_content =
            "{\"request\":\"\",\"response\":\"/"
            "Users/wilsonlu/Desktop/MeowWare/"
            "hackmd-c2-client\",\"save_path\":\"\","
            "\"status\":\"ready\"}";
        std::string response = api.update_note(NOTE_ID, &new_content);
        std::cout << "Update Note Response: " << response << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}