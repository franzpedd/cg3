#include "littlesnake.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define NOMINMAX
#define VC_EXTRALEAN
#include <windows.h>
#endif

#include <time.h>

#include <gl/GL.h>

/// @brief global state
gamestate_t g_GameState;

/////////////////////////////////////////////////////////////////////////////// node functions

bool node_add(node_t* add)
{
    node_t* newnode = (node_t*)malloc(sizeof(node_t));

    if (!newnode) {
        return false;
    }

    newnode->x = add->x;
    newnode->y = add->y;
    newnode->next = add->next;
    newnode->prev = add;
    add->next = newnode;
    newnode->next->prev = newnode;

    g_GameState.snake_size++;
    return true;
}

bool node_free()
{
    if (g_GameState.head == NULL) {
        return false;
    }

    node_t* current = g_GameState.head->next;
    node_t* next = NULL;

    // free everybody except head
    while (current != NULL && current != g_GameState.head) {
        next = current->next;
        free(current);
        current = next;
    }

    // free head
    free(g_GameState.head);

    g_GameState.snake_size = 0;
    return true;
}

bool node_same_coordinates(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    static const int32_t node_size = 5;
    int32_t dx = abs(x1 - x2);
    int32_t dy = abs(y1 - y2);
    return dx <= node_size * 2 && dy <= node_size * 2;
}

bool node_search(int32_t x, int32_t y)
{
    node_t* aux = g_GameState.head;

    if (!aux) {
        return false;
    }

    do
    {
        if (node_same_coordinates(aux->x, aux->y, x, y)) {
            return true;
        }
        aux = aux->next;
    } while (aux != g_GameState.head);

    return false;
}

/////////////////////////////////////////////////////////////////////////////// obstacle functions

// handle viewport wrapping
void wrap_position(int32_t* x, int32_t* y)
{
    if (*x >= 640) *x = 5;
    if (*x <= 0) *x = 635;
    if (*y >= 480) *y = 5;
    if (*y <= 0) *y = 475;
}

bool check_obstacle_collision(int32_t x, int32_t y)
{
    for (uint32_t i = 0; i < g_GameState.current_obstacles; i++) {
        
        if (x + 5 >= g_GameState.obstacles[i].x && x - 5 <= g_GameState.obstacles[i].x + g_GameState.obstacles[i].width &&
            y + 5 >= g_GameState.obstacles[i].y && y - 5 <= g_GameState.obstacles[i].y + g_GameState.obstacles[i].height) {
            return true;
        }
    }
    return false;
}

bool is_position_overlapping_obstacle(int32_t x, int32_t y, int32_t margin)
{
    for (uint32_t i = 0; i < g_GameState.current_obstacles; i++) {
        if (x + margin >= g_GameState.obstacles[i].x && x - margin <= g_GameState.obstacles[i].x + g_GameState.obstacles[i].width &&
            y + margin >= g_GameState.obstacles[i].y && y - margin <= g_GameState.obstacles[i].y + g_GameState.obstacles[i].height) {
            return true;
        }
    }
    return false;
}

bool is_position_overlapping_snake(int32_t x, int32_t y)
{
    return node_search(x, y);
}

void generate_obstacles()
{
    srand((unsigned int)time(NULL));
    g_GameState.current_obstacles = 6 + (rand() % 5); // 6 to 10 obstacles

    for (uint32_t i = 0; i < g_GameState.current_obstacles; i++) {
         int32_t valid_position = 0;

        while (!valid_position) {
            
            // random size: medium (20x20) or large (30x30 or 40x40)
             int32_t size_choice = rand() % 3;
            if (size_choice == 0) {
                g_GameState.obstacles[i].width = 20;
                g_GameState.obstacles[i].height = 20;
            }
            else if (size_choice == 1) {
                g_GameState.obstacles[i].width = 30;
                g_GameState.obstacles[i].height = 30;
            }
            else {
                g_GameState.obstacles[i].width = 40;
                g_GameState.obstacles[i].height = 40;
            }

            // random position within bounds (leaving margin from edges)
            g_GameState.obstacles[i].x = 10 + (rand() % (620 - g_GameState.obstacles[i].width));
            g_GameState.obstacles[i].y = 10 + (rand() % (460 - g_GameState.obstacles[i].height));

            // check if obstacle overlaps with snake starting position
             int32_t center_x = g_GameState.obstacles[i].x + g_GameState.obstacles[i].width / 2;
             int32_t center_y = g_GameState.obstacles[i].y + g_GameState.obstacles[i].height / 2;

            valid_position = 1;

            // check overlap with snake head area (around 40,60)
            if (center_x >= 20 && center_x <= 60 && center_y >= 40 && center_y <= 80) {
                valid_position = 0;
            }

            // check overlap with other obstacles
            for (uint32_t j = 0; j < i && valid_position; j++) {
                if (g_GameState.obstacles[i].x < g_GameState.obstacles[j].x + g_GameState.obstacles[j].width &&
                    g_GameState.obstacles[i].x + g_GameState.obstacles[i].width > g_GameState.obstacles[j].x &&
                    g_GameState.obstacles[i].y < g_GameState.obstacles[j].y + g_GameState.obstacles[j].height &&
                    g_GameState.obstacles[i].y + g_GameState.obstacles[i].height > g_GameState.obstacles[j].y) {
                    valid_position = 0;
                }
            }
        }
    }
}

void draw_obstacles()
{
    glColor3f(0.5f, 0.0f, 0.0f); // dark red color for obstacles
    for (uint32_t i = 0; i < g_GameState.current_obstacles; i++) {
        glRecti(g_GameState.obstacles[i].x, g_GameState.obstacles[i].y, g_GameState.obstacles[i].x + g_GameState.obstacles[i].width, g_GameState.obstacles[i].y + g_GameState.obstacles[i].height);
    }
    glColor3f(0.0f, 0.0f, 0.0f); // reset to black for snake
}

/////////////////////////////////////////////////////////////////////////////// opengl functions

void ogl_init()
{
    glColor3f(0.0, 0.0, 0.0);
}

void ogl_draw_cell(int32_t x,  int32_t y)
{
    glRecti(x - 5, y - 5, x + 5, y + 5);
}

/////////////////////////////////////////////////////////////////////////////// gamestate functions

bool gamestate_init_node()
{
    // this was added because author never bother with it
    if (g_GameState.head != NULL) {
        if (g_GameState.head->next != NULL) {
            free(g_GameState.head->next);
        }

        if (g_GameState.head->prev != NULL) {
            free(g_GameState.head->prev);
        }
        free(g_GameState.head);
    }

    g_GameState.head = (node_t*)malloc(sizeof(node_t));

    if (!g_GameState.head) {
        return false;
    }

    g_GameState.head->next = NULL;
    g_GameState.head->prev = NULL;
    g_GameState.head->x = 40;
    g_GameState.head->y = 60;

    node_t* tail = g_GameState.head;
    node_t* aux = NULL;

    // build the snake body by adding segments after the head
    for (int32_t i = 0; i < g_GameState.snake_size; i++) {
        aux = (node_t*)malloc(sizeof(node_t));
        if (!aux) {
            return false;
        }

        aux->x = tail->x + 11;
        aux->y = tail->y;
        aux->next = NULL;
        aux->prev = tail;
        tail->next = aux;
        tail = aux;
    }

    // make it a circular list
    tail->next = g_GameState.head;
    g_GameState.head->prev = tail;
    g_GameState.change = g_GameState.head;  // change points to actual head

    return true;
}

bool gamestate_init()
{
    // setting everything to 0/NULL just to be sure
    memset(&g_GameState, 0, sizeof(gamestate_t));

    g_GameState.snake_size = 3;
    g_GameState.dir = direction_right;
    g_GameState.change_x = 11;
    g_GameState.change_y = 0;
    g_GameState.food_x = 160;
    g_GameState.food_y = 60;
    g_GameState.ate = false;
    g_GameState.speed = 5;
    g_GameState.play = false;

    ogl_init();
    generate_obstacles(); // Generate obstacles at game start
    return gamestate_init_node();
}

void gamestate_wait(int32_t time)
{
    #ifdef _WIN32
    Sleep(time * 10);  // t * 10 milliseconds
    #else
    struct timespec rem;
    rem.tv_sec = 0;
    rem.tv_nsec = 10000000 * time;
    nanosleep(&rem, NULL);
    #endif
}

void gamestate_create_food()
{
    srand((unsigned int)time(NULL));

    do {
        g_GameState.food_x = rand() % 628 + 6;
        g_GameState.food_y = rand() % 468 + 6;
    } while (node_search(g_GameState.food_x, g_GameState.food_y) || is_position_overlapping_obstacle(g_GameState.food_x, g_GameState.food_y, 5));
}

void gamestate_draw_snake()
{
    node_t* aux = g_GameState.head;

    if (!aux) {
        return;
    }

    do
    {
        ogl_draw_cell(aux->x, aux->y);
        aux = aux->next;
    } while (aux != g_GameState.head);
}

void gamestate_draw_food(int32_t x, int32_t y)
{
    glColor3f(1.0f, 0.0f, 0.0f); // red color for food
    ogl_draw_cell(x, y);
    glColor3f(0.0f, 0.0f, 0.0f); // reset to black
}

void gamestate_draw_head(int32_t x, int32_t y)
{
    glColor3f(0.0f, 0.5f, 0.0f); // dark green for head
    if (g_GameState.dir == direction_right || g_GameState.dir == direction_left)
    {
        glBegin(GL_TRIANGLES);
        glVertex2i(x - 5, y + 5);
        glVertex2i(x, y + 9);
        glVertex2i(x + 5, y + 5);
        glEnd();

        glColor3f(0.0f, 0.0f, 0.0f); // reset
        return;
    }

    glBegin(GL_TRIANGLES);
    glVertex2i(x + 5, y - 5);
    glVertex2i(x + 9, y);
    glVertex2i(x + 5, y + 5);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f); // reset
}

void gamestate_snake_move()
{
    int32_t move_x = g_GameState.change->prev->x + g_GameState.change_x;
    int32_t move_y = g_GameState.change->prev->y + g_GameState.change_y;

    // apply wrapping instead of collision detection
    wrap_position(&move_x, &move_y);

    // check collision with obstacles
    if (check_obstacle_collision(move_x, move_y)) {
        display_out();
        return;
    }

    // check collision with self (only, since boundaries now wrap)
    if (node_search(move_x, move_y)) {
        display_out();
        return;
    }

    // check food collision
    if (node_same_coordinates(move_x, move_y, g_GameState.food_x, g_GameState.food_y))
    {
        node_add(g_GameState.change);
        display_play();
        gamestate_create_food();
    }

    g_GameState.change->x = move_x;
    g_GameState.change->y = move_y;

    gamestate_draw_snake();
    draw_obstacles();

    if (g_GameState.change != NULL) {
        gamestate_draw_head(g_GameState.change->x, g_GameState.change->y);
    }

    gamestate_draw_food(g_GameState.food_x, g_GameState.food_y);

    glFlush();

    gamestate_wait(g_GameState.speed);

    g_GameState.change = g_GameState.change->next;

    glutPostRedisplay();
}

void gamestate_call_init()
{
    glPointSize(4.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 640.0, 0.0, 480.0);
    glClearColor(0.127f, 0.252f, 0.0f, 0.0f);
    gamestate_init();
    g_GameState.play = true;
}

void gamestate_call_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    if (g_GameState.play) {
        gamestate_snake_move();
    }
    
    else
    {
        display_play();
        gamestate_draw_snake();
        draw_obstacles();
        gamestate_draw_head(g_GameState.change->prev->x, g_GameState.change->prev->y);
        gamestate_draw_food(g_GameState.food_x, g_GameState.food_y);
        glFlush();
    }
}

void gamestate_call_keyboard(unsigned char key, int32_t x, int32_t y)
{
    switch (key)
    {
        case 'p':
        {
            g_GameState.play = !g_GameState.play;
            break;
        }

        case 'q':
        {
            display_out();
            display_quit();
            break;
        }

        case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        {
            g_GameState.speed = (int32_t)key - 48;
            break;
        }
    }
    glutPostRedisplay();
}

void gamestate_call_keyboard_special(int32_t key, int32_t x, int32_t y)
{
    if (g_GameState.play)
    {
        switch (key)
        {
        case GLUT_KEY_RIGHT:
        {
            if (g_GameState.dir != direction_left) {
                g_GameState.dir = direction_right;
                g_GameState.change_x = 11;
                g_GameState.change_y = 0;
            }
            break;
        }

        case GLUT_KEY_LEFT:
        {
            if (g_GameState.dir != direction_right) {
                g_GameState.dir = direction_left;
                g_GameState.change_x = -11;
                g_GameState.change_y = 0;
            }
            break;
        }

        case GLUT_KEY_UP:
        {
            if (g_GameState.dir != direction_down) {
                g_GameState.dir = direction_up;
                g_GameState.change_x = 0;
                g_GameState.change_y = 11;
            }
            break;
        }

        case GLUT_KEY_DOWN:
        {
            if (g_GameState.dir != direction_up) {
                g_GameState.dir = direction_down;
                g_GameState.change_x = 0;
                g_GameState.change_y = -11;
            }
            break;
        }
        }
        glutPostRedisplay();
    }
}

/////////////////////////////////////////////////////////////////////////////// pr int32_t stuff

void display_clear_screen()
{
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

void display_help()
{
    display_clear_screen();

    printf("\n---------------------[ Snake Game ]------------------------\n");
    printf("\n              Developed by Rohit kadam, Modified by franzpedd\n\n");
    printf("-----------------------------------------------------------\n");
    printf("\n p : Start / Play / Pause ");
    printf("\n q : Exit");
    printf("\n     Use arrow key to move the snake ");
    printf("\n [1 - 9] : change level of your game");
    printf("\n-----------------------------------------------------------\n");

    fflush(stdin);
}

void display_play()
{
    display_clear_screen();

    display_help();

    printf("\n\n");
    printf("         -------------\n");
    printf("           Score: %d \n", g_GameState.snake_size - 5);
    printf("         -------------");
    printf("\n");
}

void display_out()
{
    display_play();
    printf("\n----GAME OVER----\n");
    gamestate_init();
}

void display_quit()
{
    printf("\nThank for Playing Game\n");
    printf("-----------------------------------------------------------\n");
    exit(0);
}