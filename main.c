#include <stdio.h>
#include "raylib.h"
int score = 3;
const int screen_height = 1000;
const int screen_width = 1000;
const int rows = 20;
const int cols = 20;
const int cell_width = screen_width/rows;
const int cell_height = screen_width/cols;
const int max_snake_cells = rows * cols;
const double GAME_SPEED = 8;

typedef enum {
    left,
    right,
    up,
    down,
}Dir;

typedef enum {
    game,
    end,
}Game_State;

typedef struct {
    Vector2 pos;
    Vector2 prev_pos;
    bool active;
    Dir dir;
}Snake;


void draw_grid() {
    for (int x = 0; x<screen_width; x += cell_width) {
        for (int y = 0; y <screen_height; y+= cell_height) {
            DrawRectangleLines(x, y, cell_width, cell_height, PURPLE);
        }
    }
}


void move_head(Snake *snake) {
    snake->prev_pos = snake->pos;

    switch (snake[0].dir) {
        case left: {
                       snake->pos.x -=cell_width;
                   }break;
        case right: {
                        snake->pos.x +=cell_width;
                    }break;
        case up: {
                     snake->pos.y -=cell_height;
                 }break;
        case down: {
                       snake->pos.y +=cell_height;
                   }break;
    }
}


void move_body(Snake *snake, int max_snake_cells) {

    for (int i = 1; i <max_snake_cells; ++i) {
        if (snake[i].active == true) {
            bool should_move = (snake[i].pos.x != snake[i-1].pos.x || snake[i].pos.y != snake[i -1].pos.y) ;

            if (should_move) {
                snake[i].prev_pos = snake[i].pos;
                snake[i].pos = snake[i-1].prev_pos;
            }
        }else break;
    }
}


void draw_body(Snake *snake, int max_snake_cells) {
    for (int i = max_snake_cells -1; i >0; --i) {
        if (snake[i].active == true) {
            DrawRectangle(snake[i].pos.x, snake[i].pos.y, cell_width, cell_height, ORANGE);
        }
    }

}

void draw_apple(Vector2 *apple_pos) {
    DrawRectangle(apple_pos->x, apple_pos->y, cell_width, cell_height, RED);
}


void spawn_new_apple(Vector2 *apple_pos, Snake *snake) {
    while (1) {
        int x = GetRandomValue(0, 19) * cell_width;
        int y = GetRandomValue(0, 19) * cell_height;

        bool match = false;
        for (int i =0; i <max_snake_cells; ++i) {
            if (snake[i].active == false) break;
            if (snake[i].pos.x == x && snake[i].pos.y == y) {
                match = true;
            }
        }
        if (match == false) {
            apple_pos->x = x;
            apple_pos->y = y;
            break;
        }
    }
}


bool eat_apple_check_and_grow(Vector2 *apple_pos, Snake *snake) {
    if (apple_pos->x == snake[0].pos.x && apple_pos->y == snake[0].pos.y ) {
        for (int i = 2; i <max_snake_cells; ++i) {
            if (snake[i].active == true) continue;
            snake[i].active = true;
            snake[i].pos = snake[i-1].pos; 
            snake[i].dir = snake[i-1].dir;
            ++score;
            break;
        }
        return true;

    } 
    else return false;
}

void get_input(Dir *loaded_input, Snake *snake) {
    if (IsKeyDown(KEY_LEFT) && snake[0].dir != right ) {
        *loaded_input = left;
    }
    else if (IsKeyDown(KEY_RIGHT)&& snake[0].dir != left ) {
        *loaded_input = right;
    }
    else if (IsKeyDown(KEY_DOWN)&& snake[0].dir != up ) {
        *loaded_input = down;
    }
    else if (IsKeyDown(KEY_UP)&& snake[0].dir != down ) {
        *loaded_input = up;
    }
}

void update_head_dir(Dir *loaded_input, Snake *snake) {

        snake[0].dir = *loaded_input;
}



bool check_lose_cond(Snake *snake) {
    for (int i = 0; i <max_snake_cells; ++i) {
        if (snake[i].active == false) {
            for (int cell_index = i-1; cell_index >0; --cell_index) {
                if (snake[0].pos.x == snake[cell_index].pos.x && snake[0].pos.y == snake[cell_index].pos.y) {
                    return true;
                }
            }
            if ( (snake[0].pos.x + cell_width)  > screen_width) return true;
            if ( (snake[0].pos.x)  < 0) return true;
            if ( (snake[0].pos.y)  < 0) return true;
            if ( (snake[0].pos.y + cell_height) > screen_height) return true;
            break;
        };
    }
    return false;
}


int main() {
    InitWindow(screen_width, screen_height, "snake game");

    Snake snake[max_snake_cells];
    for (int i = 0; i < (rows * cols); ++i) {
        snake[i].active = false;
    }

    snake[0].pos = (Vector2){ cell_width * (cols/2) , cell_height * (rows/2)}; 
    snake[1].pos = (Vector2){ cell_width * (cols/2) - cell_width, cell_height * (rows/2)}; 
    snake[2].pos = (Vector2){ cell_width * (cols/2)  - (cell_width *2), cell_height * (rows/2)}; 
    snake[0].dir = right;
    snake[1].dir = right;
    snake[2].dir = right;

    snake[0].active = true;
    snake[1].active = true;
    snake[2].active = true;

    Vector2 apple_pos;
    spawn_new_apple(&apple_pos, &snake[0]);

    SetTargetFPS(60);
    double update_time = 0;
    double frame_time = 1.0/GAME_SPEED;
    Game_State game_state = game;
    Dir loaded_input = right;
    HideCursor();
    while (!WindowShouldClose()) {
        BeginDrawing();
        update_time += GetFrameTime();
        switch (game_state) {
            case game: {
                           ClearBackground(BROWN);
                           get_input(&loaded_input, &snake[0]);

                               //draw_grid();
                           if (update_time > frame_time) {
                               update_time = 0;

                               update_head_dir(&loaded_input,&snake[0]);
                               move_head( &snake[0]);
                               move_body(snake, max_snake_cells);

                               if (eat_apple_check_and_grow(&apple_pos, &snake[0]) ) {
                                   spawn_new_apple(&apple_pos, &snake[0]);
                               }


                               bool lost = check_lose_cond(&snake[0]);
                               if (lost) game_state = end;
                           }

                           draw_body(snake, max_snake_cells);
                           DrawRectangle(snake[0].pos.x, snake[0].pos.y, cell_width, cell_height, GREEN);//draw_head
                           draw_apple(&apple_pos);
                           DrawText(TextFormat("Length:%d",score  ),0, 0, 30,SKYBLUE);
                       }break;
            case  end:
                       {
                           char * game_over = "Game Over";
                           int game_over_font_size = 90;
                           DrawText(game_over, screen_width/2 - MeasureText(game_over,game_over_font_size)/2 , screen_height/2, game_over_font_size, SKYBLUE);
                       }break;
        }

        EndDrawing();
    }
}
