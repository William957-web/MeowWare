#ifndef STRUCT_HPP
#define STRUCT_HPP

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

const std::string STATUS_READY = "ready";
const std::string STATUS_EXECUTING = "executing";

class Target {
   public:
    std::string name;
    std::string note_id;
    std::string real_time_note_id;

    Target() : name(""), note_id(""), real_time_note_id("") {}

    Target(const std::string& name,
           const std::string& note_id = "",
           const std::string& real_time_note_id = "")
        : name(name), note_id(note_id), real_time_note_id(real_time_note_id) {}

    json to_dict() const {
        return json{{"name", name},
                    {"note_id", note_id},
                    {"real_time_note_id", real_time_note_id}};
    }

    static Target from_dict(const json& data) {
        return Target(data.value("name", ""), data.value("note_id", ""),
                      data.value("real_time_note_id", ""));
    }
};

class HackmdData {
   public:
    std::string status;
    std::string request;
    std::string response;
    std::string save_path;

    HackmdData()
        : status(STATUS_READY), request(""), response(""), save_path("") {}

    HackmdData(const std::string& status,
               const std::string& request,
               const std::string& response,
               const std::string& save_path)
        : status(status),
          request(request),
          response(response),
          save_path(save_path) {}

    json to_dict() const {
        return json{{"status", status},
                    {"request", request},
                    {"response", response},
                    {"save_path", save_path}};
    }

    static HackmdData from_dict(const json& data) {
        return HackmdData(data.value("status", STATUS_READY),
                          data.value("request", ""), data.value("response", ""),
                          data.value("save_path", ""));
    }

    static std::string pack(const std::string& status,
                            const std::string& request,
                            const std::string& response,
                            const std::string& save_path) {
        json packed = {{"status", status},
                       {"request", request},
                       {"response", response},
                       {"save_path", save_path}};
        return packed.dump();
    }

    static HackmdData unpack(const std::string& packed_str) {
        try {
            json data = json::parse(packed_str);
            return from_dict(data);
        } catch (const json::parse_error& e) {
            return HackmdData(STATUS_READY, "", "", "");
        }
    }
};

const std::string INIT_CONTENT = HackmdData::pack(STATUS_READY, "", "", "");

const std::string INIT_REAL_TIME_CONTENT = "";

#endif  // DATA_TYPES_HPP
