#ifndef LITTLESNAKE_HEADER
#define LITTLESNAKE_HEADER

#include <stdint.h>
#include <stdbool.h>

#include <GL/freeglut.h>

/// @brief how many obstacles at most the game has
#define MAX_OBSTACLES 10

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
    struct node_t* next;
    struct node_t* prev;
} node_t;

/// @brief holds information about obstacles
typedef struct obstacle_t
{
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} obstacle_t;

/// @brief holds information about the game
typedef struct gamestate_t
{
    node_t* head;
    node_t* change;
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
    obstacle_t obstacles[MAX_OBSTACLES];
    uint32_t current_obstacles;
} gamestate_t;

/////////////////////////////////////////////////////////////////////////////// node functions

/// @brief adds a node after a given node inside a double linked list
bool node_add(node_t* add);

/// @brief releases the double linked list
bool node_free();

/// @brief checks if node has same coordiantes
bool node_same_coordinates(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/// @brief searches on the node if node exists (based on coords)
bool node_search(int32_t x, int32_t y);

/////////////////////////////////////////////////////////////////////////////// obstacle functions

/// @brief handle viewport wrapping
void wrap_position(int32_t* x, int32_t* y);

/// @brief check if the snake head collides with any obstacle
bool check_obstacle_collision(int32_t x, int32_t y);

/// @brief checks if position is collinding with obstacle
bool is_position_overlapping_obstacle(int32_t x, int32_t y, int32_t margin);

/// @brief returns if position is colliding with snake
bool is_position_overlapping_snake(int32_t x, int32_t y);

/// @brief generate 6-10 obstacles with random sizes
void generate_obstacles();

/// @brief draws the obstacles
void draw_obstacles();

/////////////////////////////////////////////////////////////////////////////// opengl functions

/// @brief setup initial open gl config
void ogl_init();

/// @brief draws the game's standart rectangle on the coordinates
void ogl_draw_cell(int32_t x, int32_t y);

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
void gamestate_call_display(void);

/// @brief callback baclled by glutKeyboardFunc
void gamestate_call_keyboard(unsigned char key, int32_t x, int32_t y);

/// @brief callback called by glutSpecialFunc
void gamestate_call_keyboard_special(int32_t key, int32_t x, int32_t y);

/////////////////////////////////////////////////////////////////////////////// print-related stuff

/// @brief clears the screen, platform independent
void display_clear_screen();

/// @brief prints usefull info
void display_help();

/// @brief prints the playing state info
void display_play();

/// @brief prints info when game over appear
void display_out();

/// @brief prints info when exiting the game
void display_quit();

#endif // LITTLESNAKE_HEADER