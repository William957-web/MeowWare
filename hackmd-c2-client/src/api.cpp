#include "api.hpp"
#include <curl/curl.h>
#include <iostream>

static size_t WriteCallback(void* contents,
                            size_t size,
                            size_t nmemb,
                            std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

API::API(const std::string& api_key) : api_key(api_key) {
    headers["Authorization"] = "Bearer " + api_key;
    headers["Content-Type"] = "application/json";
}

std::string API::send_request(const std::string& method,
                              const std::string& url,
                              const json* post_data) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string response;
    struct curl_slist* curl_headers = nullptr;

    try {
        for (const auto& [key, value] : headers) {
            std::string header = key + ": " + value;
            curl_headers = curl_slist_append(curl_headers, header.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        std::string json_str;
        if (post_data) {
            json_str = post_data->dump();
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,
                             (long)json_str.length());
        }

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            throw std::runtime_error(std::string("CURL request failed: ") +
                                     curl_easy_strerror(res));
        }

        curl_slist_free_all(curl_headers);
        curl_easy_cleanup(curl);

        return response;
    } catch (...) {
        if (curl_headers) {
            curl_slist_free_all(curl_headers);
        }
        curl_easy_cleanup(curl);
        throw;
    }
}

json API::get_me() {
    std::string url = "https://api.hackmd.io/v1/me";
    std::string response = send_request("GET", url);
    return json::parse(response);
}

json API::get_note_list() {
    std::string url = "https://api.hackmd.io/v1/notes";
    std::string response = send_request("GET", url);
    return json::parse(response);
}

json API::get_note(const std::string& note_id) {
    if (note_id.length() != 22) {
        throw std::invalid_argument("note_id should be with length 22.");
    }
    std::string url = "https://api.hackmd.io/v1/notes/" + note_id;
    std::string response = send_request("GET", url);
    json result;
    try {
        result = json::parse(response);
    } catch (const std::exception& e) {
        result = {{"status", "ready"},
                  {"request", ""},
                  {"response", ""},
                  {"save_path", ""}};
        std::cerr << e.what() << '\n';
    }

    return result;
}

json API::create_note(const std::string& title,
                      const std::string& content,
                      const std::string& read_permission,
                      const std::string& write_permission,
                      const std::string& comment_permission) {
    std::string url = "https://api.hackmd.io/v1/notes";
    json post_data = {{"title", title},
                      {"content", content},
                      {"readPermission", read_permission},
                      {"writePermission", write_permission},
                      {"commentPermission", comment_permission}};
    std::string response = send_request("POST", url, &post_data);
    return json::parse(response);
}

std::string API::update_note(const std::string& note_id,
                             const std::string* content,
                             const std::string* read_permission,
                             const std::string* write_permission,
                             const std::string* comment_permission) {
    if (note_id.length() != 22) {
        throw std::invalid_argument("note_id should be with length 22.");
    }

    std::string url = "https://api.hackmd.io/v1/notes/" + note_id;
    // std::string url = "http://localhost:4444/" + note_id;
    json post_data;

    if (content != nullptr) {
        post_data["content"] = *content;
    }
    if (read_permission != nullptr) {
        post_data["readPermission"] = *read_permission;
    }
    if (write_permission != nullptr) {
        post_data["writePermission"] = *write_permission;
    }
    if (comment_permission != nullptr) {
        post_data["commentPermission"] = *comment_permission;
    }

    return send_request("PATCH", url, &post_data);
}

std::string API::delete_note(const std::string& note_id) {
    std::string url = "https://api.hackmd.io/v1/notes/" + note_id;
    return send_request("DELETE", url);
}

json API::get_note_read_history() {
    std::string url = "https://api.hackmd.io/v1/history";
    std::string response = send_request("GET", url);
    return json::parse(response);
}