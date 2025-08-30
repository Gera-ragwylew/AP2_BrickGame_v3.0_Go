#include <iostream>
#include <string>
#include <vector>
#include "src/brick_game/defines.h"
#include "src/brick_game/specification.h"

struct State {
    std::vector<std::vector<bool>> field;
    std::vector<std::vector<bool>> next;
    int score;
    int high_score;
    int level;
    int speed;
    bool pause;
};

class GameWrapper {
public:
    void run() {
        GameInfo_t state;
        userInput(UserAction_t(2), false);
        std::string input;
        while (state.level != -1) {
            if (std::getline(std::cin, input)) {
                size_t space_pos = input.find(' ');
                if (space_pos != std::string::npos) {
                    std::string action_str = input.substr(0, space_pos);
                    bool hold = (input.substr(space_pos + 1) == "1");
                    
                    UserAction_t action = parseAction(action_str);
                    userInput(action, hold);
                }
            }
            
            state = updateCurrentState();
            std::cout << serializeState(state) << std::endl;
        }
    }

private:
    UserAction_t parseAction(const std::string& action_str) {
        if (action_str == "Start") return UserAction_t::Start;
        if (action_str == "Pause") return UserAction_t::Pause;
        if (action_str == "Terminate") return UserAction_t::Terminate;
        if (action_str == "Left") return UserAction_t::Left;
        if (action_str == "Right") return UserAction_t::Right;
        if (action_str == "Up") return UserAction_t::Up;
        if (action_str == "Down") return UserAction_t::Down;
        if (action_str == "Action") return UserAction_t::Action;
        return UserAction_t::Terminate;
    }

    std::string serializeState(const GameInfo_t& state) {
        auto bool_to_str = [](bool b) { return b ? "1" : "0"; };
        
        std::string json = "{";
        json += "\"score\":" + std::to_string(state.score) + ",";
        json += "\"high_score\":" + std::to_string(state.high_score) + ",";
        json += "\"level\":" + std::to_string(state.level) + ",";
        json += "\"speed\":" + std::to_string(state.speed) + ",";
        json += "\"pause\":" + std::to_string(state.pause) + ",";

        auto serializeMatrix = [&](int** matrix, const std::string& name, int rows, int cols) {
            std::string result = "\"" + name + "\":[";
            for (int i = 0; i < rows; ++i) {
                if (i != 0) result += ",";
                result += "[";
                for (int j = 0; j < cols; ++j) {
                    if (j != 0) result += ",";
                    result += bool_to_str(matrix[i][j]);
                }
                result += "]";
            }
            return result + "]";
        };

        json += serializeMatrix(state.field, "field", WINDOW_HEIGHT, WINDOW_WIDTH) + ",";
        json += serializeMatrix(state.next, "next", NEXT_SIZE, NEXT_SIZE);
        
        return json + "}";
    }
};

int main() {
    GameWrapper wrapper;
    wrapper.run();
    return 0;
}