#include "game_client.h"
#include <curl/curl.h>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <atomic>
#include <mutex>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GameClient {
private:
    // Конфигурация
    static constexpr int FIELD_ROWS = 20;
    static constexpr int FIELD_COLS = 20;
    static constexpr int NEXT_ROWS = 4;
    static constexpr int NEXT_COLS = 4;

    // Состояние
    GameInfo_t client_state_;
    std::vector<std::vector<int>> field_data_;
    std::vector<std::vector<int>> next_data_;
    
    std::map<int, std::string> game_list_;
    std::string server_url_;
    std::atomic<int> current_game_id_{0};
    std::atomic<bool> connected_{false};
    std::mutex state_mutex_;

    // HTTP клиент
    struct HttpClient {
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
            output->append(static_cast<char*>(contents), size * nmemb);
            return size * nmemb;
        }

        static bool Get(const std::string& url, std::string& response) {
            CURL* curl = curl_easy_init();
            if (!curl) return false;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            
            return res == CURLE_OK;
        }

        static bool Post(const std::string& url, const std::string& data, std::string& response) {
            CURL* curl = curl_easy_init();
            if (!curl) return false;

            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            CURLcode res = curl_easy_perform(curl);
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);

            return res == CURLE_OK;
        }
    };

public:
    GameClient() {
        InitializeState();
    }

    ~GameClient() {
        // Автоматическое освобождение памяти благодаря vector
    }

    void InitializeState() {
        field_data_.resize(FIELD_ROWS, std::vector<int>(FIELD_COLS, 0));
        next_data_.resize(NEXT_ROWS, std::vector<int>(NEXT_COLS, 0));

        // Настраиваем указатели
        client_state_.field = new int*[FIELD_ROWS];
        client_state_.next = new int*[NEXT_ROWS];
        
        for (int i = 0; i < FIELD_ROWS; ++i) {
            client_state_.field[i] = field_data_[i].data();
        }
        for (int i = 0; i < NEXT_ROWS; ++i) {
            client_state_.next[i] = next_data_[i].data();
        }

        client_state_.score = 0;
        client_state_.high_score = 0;
        client_state_.level = 0;
        client_state_.speed = 0;
        client_state_.pause = 0;
    }

    bool Connect(const std::string& server_url) {
        server_url_ = server_url;
        std::string response;
        
        if (HttpClient::Get(server_url_ + "/", response)) {
            connected_ = true;
            return true;
        }
        return false;
    }

    void HandleInput(UserAction_t action, bool hold) {
        if (!connected_ && !Connect("http://localhost:8080")) {
            print_debug_info("Failed to connect");
            return;
        }

        // first call - POST /games/{action}
        if (connected_ && current_game_id_ == 0) {
            // print_debug_info("Selection the game");
            SelectGame(static_cast<int>(action));
            current_game_id_ = static_cast<int>(action);
            return;
        }

        SendAction(action, hold);
        // print_debug_info("Send Action");
    }

    GameInfo_t UpdateState() {
        if (!connected_ && !Connect("http://localhost:8080")) {
            print_debug_info("Failed to connect");
            return client_state_;
        }

        // first call - GET /games
        if (connected_ && current_game_id_ == 0) {
            // print_debug_info("Waiting to choose the game");
            LoadGames();
            DrawGameList();
            return client_state_;
        }

        FetchGameState();

        if (client_state_.level == -1) {
            print_debug_info("Server send Game over");
            current_game_id_ = 0;
        }
        
        return client_state_;
    }

private:
    void SelectGame(int game_id) {
        std::string response;
        HttpClient::Post(server_url_ + "/api/games/" + std::to_string(game_id), "{}", response);
    }

    void SendAction(UserAction_t action, bool hold) {
        json action_data = {{"action_id", action}, {"hold", hold}};
        std::string response;
        HttpClient::Post(server_url_ + "/api/actions", action_data.dump(), response);
    }

    void LoadGames() {
        std::string response;
        if (HttpClient::Get(server_url_ + "/api/games", response)) {
            try {
                auto data = json::parse(response);
                if (data.contains("games") && data["games"].is_array()) {
                    game_list_.clear();
                    for (const auto& game : data["games"]) {
                        if (game.contains("id") && game.contains("name")) {
                            int id = game["id"].get<int>();
                            std::string name = game["name"].get<std::string>();
                            game_list_[id] = name;
                        }
                    }
                }
            } catch (const std::exception&) {
            }
        }
    }

    void DrawGameList() {
        // Очистка поля
        for (auto& row : field_data_) {
            std::fill(row.begin(), row.end(), 0);
        }

        // Отрисовка списка игр
        int row = 2;
        for (const auto& [id, name] : game_list_) {
            if (row >= FIELD_ROWS) break;
            
            field_data_[row][1] = id + '0';
            field_data_[row][2] = '.';
            
            for (size_t j = 0; j < name.size() && j < 7; ++j) {
                field_data_[row][3 + j] = name[j];
            }
            row += 2;
        }

        // std::string filename = "vector_debug.log";
        // std::ofstream file(filename, std::ios::app);
        // if (!file.is_open())  client_state_;
        
        // file << "=== Vector Debug: " << "Vector" << " ===\n";
        // file << "=== Game list size: " << game_list_.size() << "\n";
        // for (size_t i = 1; i <= game_list_.size(); i++) {
        //     file << game_list_[i] << "\n";
        // }
        
        
        // for (size_t i = 0; i < field_data_.size(); ++i) {
        //     for (size_t j = 0; j < field_data_[i].size(); j++) {
        //         file << field_data_[i][j];
        //     }
        //     file << "\n";
        // }
        
        // file << "============================\n\n";
        // file.close();
    }

    void FetchGameState() {
        std::string response;
        if (HttpClient::Get(server_url_ + "/api/state", response)) {
            ParseServerState(response);
        }
    }

    void ParseServerState(const std::string& json_response) {
        try {
            auto data = json::parse(json_response);
            std::lock_guard<std::mutex> lock(state_mutex_);

            // Обновление поля
            if (data.contains("field") && data["field"].is_array()) {
                auto field_json = data["field"];
                for (size_t i = 0; i < field_data_.size() && i < field_json.size(); ++i) {
                    for (size_t j = 0; j < field_data_[i].size() && j < field_json[i].size(); ++j) {
                        field_data_[i][j] = field_json[i][j].get<bool>() ? 1 : 0;
                    }
                }
            }

            if (data.contains("next") && data["next"].is_array()) {
                auto next_json = data["next"];
                for (size_t i = 0; i < next_data_.size() && i < next_json.size(); ++i) {
                    for (size_t j = 0; j < next_data_[i].size() && j < next_json[i].size(); ++j) {
                        next_data_[i][j] = next_json[i][j].get<bool>() ? 1 : 0;
                    }
                }
            }

            // Обновление простых полей
            if (data.contains("score")) client_state_.score = data["score"].get<int>();
            if (data.contains("high_score")) client_state_.high_score = data["high_score"].get<int>();
            if (data.contains("level")) client_state_.level = data["level"].get<int>();
            if (data.contains("speed")) client_state_.speed = data["speed"].get<int>();
            if (data.contains("pause")) client_state_.pause = data["pause"].get<bool>() ? 1 : 0;

        } catch (const std::exception&) {
            // Логирование ошибки
        }
    }

    void print_debug_info(const char* info) {
        FILE* debug = fopen("debug.log", "a");
        fprintf(debug, "%s\n", info);
        fclose(debug);
    }
};

// Глобальный экземпляр (можно сделать thread-local если нужно)
namespace {
    GameClient game_client;
}

// C интерфейс
extern "C" {

void userInput(UserAction_t action, bool hold) {
    game_client.HandleInput(action, hold);
}

GameInfo_t updateCurrentState() {
    return game_client.UpdateState();
}

} // extern "C"