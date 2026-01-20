#ifndef API_HPP
#define API_HPP

#include <string>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class API {
private:
    std::string api_key;
    std::map<std::string, std::string> headers;
    
    std::string send_request(const std::string& method, const std::string& url, const json* post_data = nullptr);

public:
    API(const std::string& api_key);

    json get_me();

    json get_note_list();

    json get_note(const std::string& note_id);

    json create_note(
        const std::string& title = "",
        const std::string& content = "",
        const std::string& read_permission = "owner",
        const std::string& write_permission = "owner",
        const std::string& comment_permission = "disabled"
    );

    std::string update_note(
        const std::string& note_id,
        const std::string* content = nullptr,
        const std::string* read_permission = nullptr,
        const std::string* write_permission = nullptr,
        const std::string* comment_permission = nullptr
    );

    std::string delete_note(const std::string& note_id);

    json get_note_read_history();
};

#endif // API_HPP
