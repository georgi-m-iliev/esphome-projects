#pragma once

#include "esphome.h"
#include <map>
#include <string>
#include <vector>

// Maximum number of stored codes and tags
#define MAX_CODES 50
#define MAX_TAGS 50

// Storage for authorized codes and tags
static std::map<std::string, std::string> authorized_codes;
static std::map<std::string, std::string> authorized_tags;

// Parse stored credentials from global string
void parse_stored_credentials(const std::string& stored_data, std::map<std::string, std::string>& target_map) {
    if (stored_data.empty()) return;
    
    std::string data = stored_data;
    size_t pos = 0;
    while ((pos = data.find(',')) != std::string::npos) {
        std::string entry = data.substr(0, pos);
        size_t colon_pos = entry.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = entry.substr(0, colon_pos);
            std::string value = entry.substr(colon_pos + 1);
            target_map[key] = value;
        }
        data.erase(0, pos + 1);
    }
    // Handle last entry
    size_t colon_pos = data.find(':');
    if (colon_pos != std::string::npos) {
        std::string key = data.substr(0, colon_pos);
        std::string value = data.substr(colon_pos + 1);
        target_map[key] = value;
    }
}

// Convert credentials map to stored string format
std::string credentials_to_string(const std::map<std::string, std::string>& creds_map) {
    std::string result = "";
    bool first = true;
    for (const auto& pair : creds_map) {
        if (!first) result += ",";
        result += pair.first + ":" + pair.second;
        first = false;
    }
    return result;
}

// Save credentials to persistent storage
void save_credentials() {
    std::string codes_str = credentials_to_string(authorized_codes);
    std::string tags_str = credentials_to_string(authorized_tags);
    
    id(stored_codes) = codes_str;
    id(stored_tags) = tags_str;
    
    ESP_LOGI("AUTH", "Saved %d codes and %d tags to persistent storage", 
             authorized_codes.size(), authorized_tags.size());
}

// Initialize authentication system with persistent storage
void init_default_auth() {
    static bool initialized = false;
    if (!initialized) {
        // Load from persistent storage
        std::string codes_data = id(stored_codes);
        std::string tags_data = id(stored_tags);
        
        parse_stored_credentials(codes_data, authorized_codes);
        parse_stored_credentials(tags_data, authorized_tags);

        initialized = true;
        ESP_LOGI("AUTH", "Authentication system initialized with %d codes and %d tags", 
                 authorized_codes.size(), authorized_tags.size());
        for (const auto& pair : authorized_codes) {
            ESP_LOGI("AUTH", "Code: %s -> User: %s", pair.first.c_str(), pair.second.c_str());
        }
        for (const auto& pair : authorized_tags) {
            ESP_LOGI("AUTH", "Tag: %s -> User: %s", pair.first.c_str(), pair.second.c_str());
        }
    }
}

// Function to add authorized code
void add_authorized_code(const std::string& code, const std::string& name) {
    init_default_auth();
    
    if (authorized_codes.size() >= MAX_CODES) {
        ESP_LOGW("AUTH", "Maximum number of codes reached (%d)", MAX_CODES);
        return;
    }
    
    authorized_codes[code] = name;
    save_credentials();
    ESP_LOGI("AUTH", "Added authorized code for: %s", name.c_str());
}

// Function to add authorized tag
void add_authorized_tag(const std::string& tag, const std::string& name) {
    init_default_auth();
    
    if (authorized_tags.size() >= MAX_TAGS) {
        ESP_LOGW("AUTH", "Maximum number of tags reached (%d)", MAX_TAGS);
        return;
    }
    
    authorized_tags[tag] = name;
    save_credentials();
    ESP_LOGI("AUTH", "Added authorized tag for: %s", name.c_str());
}

// Function to remove authorized code
void remove_authorized_code(const std::string& code) {
    init_default_auth();
    
    auto it = authorized_codes.find(code);
    if (it != authorized_codes.end()) {
        ESP_LOGI("AUTH", "Removed authorized code for: %s", it->second.c_str());
        authorized_codes.erase(it);
        save_credentials();
    } else {
        ESP_LOGW("AUTH", "Code not found for removal: %s", code.c_str());
    }
}

// Function to remove authorized tag
void remove_authorized_tag(const std::string& tag) {
    init_default_auth();
    
    auto it = authorized_tags.find(tag);
    if (it != authorized_tags.end()) {
        ESP_LOGI("AUTH", "Removed authorized tag for: %s", it->second.c_str());
        authorized_tags.erase(it);
        save_credentials();
    } else {
        ESP_LOGW("AUTH", "Tag not found for removal: %s", tag.c_str());
    }
}

// Function to clear all codes
void clear_all_codes() {
    authorized_codes.clear();
    save_credentials();
    ESP_LOGI("AUTH", "All authorized codes cleared");
}

// Function to clear all tags
void clear_all_tags() {
    authorized_tags.clear();
    save_credentials();
    ESP_LOGI("AUTH", "All authorized tags cleared");
}

// Function to check if code is authorized
bool check_authorized_code(const std::string& code) {
    init_default_auth();
    
    auto it = authorized_codes.find(code);
    bool authorized = (it != authorized_codes.end());
    
    if (authorized) {
        ESP_LOGI("AUTH", "Code authorized for: %s", it->second.c_str());
    } else {
        ESP_LOGW("AUTH", "Unauthorized code attempt: %s", code.c_str());
    }
    
    return authorized;
}

// Function to check if tag is authorized
bool check_authorized_tag(const std::string& tag) {
    init_default_auth();
    
    auto it = authorized_tags.find(tag);
    bool authorized = (it != authorized_tags.end());
    
    if (authorized) {
        ESP_LOGI("AUTH", "Tag authorized for: %s", it->second.c_str());
    } else {
        ESP_LOGW("AUTH", "Unauthorized tag attempt: %s", tag.c_str());
    }
    
    return authorized;
}

// Function to get user name for code
std::string get_code_user_name(const std::string& code) {
    init_default_auth();
    
    auto it = authorized_codes.find(code);
    if (it != authorized_codes.end()) {
        return it->second;
    }
    return "Unknown";
}

// Function to get user name for tag
std::string get_tag_user_name(const std::string& tag) {
    init_default_auth();
    
    auto it = authorized_tags.find(tag);
    if (it != authorized_tags.end()) {
        return it->second;
    }
    return "Unknown";
}

// Function to list all authorized codes (for debugging)
void list_authorized_codes() {
    init_default_auth();
    
    ESP_LOGI("AUTH", "=== Authorized Codes ===");
    for (const auto& pair : authorized_codes) {
        ESP_LOGI("AUTH", "Code: %s -> User: %s", pair.first.c_str(), pair.second.c_str());
    }
    ESP_LOGI("AUTH", "Total codes: %d", authorized_codes.size());
}

// Function to list all authorized tags (for debugging)
void list_authorized_tags() {
    init_default_auth();
    
    ESP_LOGI("AUTH", "=== Authorized Tags ===");
    for (const auto& pair : authorized_tags) {
        ESP_LOGI("AUTH", "Tag: %s -> User: %s", pair.first.c_str(), pair.second.c_str());
    }
    ESP_LOGI("AUTH", "Total tags: %d", authorized_tags.size());
}
