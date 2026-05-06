#ifndef LITTLESNAKE_HEADER
#define LITTLESNAKE_HEADER

#include <stdint.h>
#include <stdbool.h>

#include "dependencies/freeglut/include/GL/freeglut.h"

/// @brief all types of movement available on the grid
typedef enum direction_t
{
    direction_right = 0,
    direction_left,
    direction_up,
    direction_down
} direction;

/// @brief holds information about a snake body cell
typedef struct node_t
{
    int32_t x;
    int32_t y;
    node* next;
    node* prev;
} node;

/// @brief holds information about the game
typedef struct gamestate_t
{
    node* head;
    node* change;
    int32_t snake_size;
    int32_t highest_score;
    int32_t change_x;
    int32_t change_y;
    int32_t food_x;
    int32_t food_y;
    int32_t speed;
    bool ate;
    bool play;
    direction dir;
} gamestate;

/////////////////////////////////////////////////////////////////////////////// node functions

/// @brief adds a node after a given node inside a double linked list
bool node_add(node* add);

/// @brief releases the double linked list
bool node_free();

/// @brief checks if node has same coordiantes
bool node_same_coordinates(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/// @brief searches on the node if node exists (based on coords)
bool node_search(int32_t x, int32_t y);

/////////////////////////////////////////////////////////////////////////////// opengl functions

/// @brief setup initial open gl config
void ogl_init();

/// @brief draws the game's standart rectangle on the coordinates
void ogl_draw_cell(int x, int y);

/////////////////////////////////////////////////////////////////////////////// gamestate functions

/// @brief initializes the gamestate nodes (snake body represented as a double linked list)
bool gamestate_init_node();

/// @brief initializes the game itself
bool gamestate_init();

/// @brief awaits for a given time in milliseconds
void gamestate_wait(int32_t time);

/// @brief creates the food in the board
void gamestate_create_food();

/// @brief draws the snake into the game
void gamestate_draw_snake();

/// @brief draws the food on the coordinate
void gamestate_draw_food(int32_t x, int32_t y);

/// @brief draws the snake's head
void gamestate_draw_head(int32_t x, int32_t y);

/// @brief moves the snake based on previous calculated varaibles
void gamestate_snake_move();

/// @brief callback called uppon game's initialization
void gamestate_call_init();

/// @brief callback called by glutDisplayFunc
void gamestate_call_display();

/// @brief callback baclled by glutKeyboardFunc
void gamestate_call_keyboard(unsigned char key, int32_t x, int32_t y);

/// @brief callback called by glutSpecialFunc
void gamestate_call_keyboard_special(int32_t key, int32_t x, int32_t y);

/////////////////////////////////////////////////////////////////////////////// print-related stuff

/// @brief clears the screen, platform independent
void display_clear_screen();

/// @brief clears the screen input without fflush
void display_clear_input_buffer();

/// @brief prints usefull info
void display_help();

/// @brief prints the playing state info
void display_play();

/// @brief prints info when game over appear
void display_out();

/// @brief prints info when exiting the game
void display_quit();

#endif // LITTLESNAKE_HEADER