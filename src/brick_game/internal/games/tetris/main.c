#include <unistd.h>
#include "src/backend/backend.h"
#include "src/backend/fsm.h"

UserAction_t parse_action(const char* action_str) {
    static const struct {
        const char* name;
        UserAction_t action;
    } actions[] = {
        {"Start", Start}, {"Pause", Pause}, {"Terminate", Terminate},
        {"Left", Left}, {"Right", Right}, {"Up", Up}, {"Down", Down}, {"Action", Action}
    };

    for (size_t i = 0; i < sizeof(actions)/sizeof(actions[0]); i++) {
        if (strcmp(action_str, actions[i].name) == 0) {
            return actions[i].action;
        }
    }
    return Terminate;
}

void print_field(const char* name, int** field, int rows, int cols) {
    printf(",\"%s\":[", name);
    for (int i = 0; i < rows; i++) {
        printf(i ? ",[" : "[");
        for (int j = 0; j < cols; j++) {
            int val = field ? field[i][j] : 0;
            printf(j ? ",%d" : "%d", val);
        }
        putchar(']');
    }
    putchar(']');
}

void print_game_info(const GameInfo_t* info) {
    putchar('{');
    printf("\"score\":%d,\"high_score\":%d,\"level\":%d,\"speed\":%d,\"pause\":%d",
           info->score, info->high_score, info->level, info->speed, info->pause);
    if (info->level != -1) {
        print_field("field", info->field, WINDOW_HEIGHT, WINDOW_WIDTH);
        print_field("next", info->next, NEXT_SIZE, NEXT_SIZE);
    } else {
        print_field("field", NULL, WINDOW_HEIGHT, WINDOW_WIDTH);
        print_field("next", NULL, NEXT_SIZE, NEXT_SIZE);
    }
    
    printf("}\n");
    fflush(stdout);
}

int main() {
    char input[256];
    GameInfo_t state;
    UserAction_t action;
    while (1) {
        if (!fgets(input, sizeof(input), stdin)) break;

        char action_str[32];
        int hold = 0;
        sscanf(input, "%31s %d", action_str, &hold);

        action = parse_action(action_str);
        userInput(action, hold);

        state = updateCurrentState();
        print_game_info(&state);
        
    }
    return 0;
}