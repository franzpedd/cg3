#include "littlesnake.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define NOMINMAX
    #define VC_EXTRALEAN
    #include <windows.h>
#else
    #include <time.h>
#endif

#include <gl/GL.h>

gamestate g_GameState;

/////////////////////////////////////////////////////////////////////////////// node functions

bool node_add(node* add)
{
    node* newnode = (node *)malloc(sizeof(node));

    if(!newnode) {
        return false;
    }

    newnode->x = add->x;
    newnode->y = add->y;
    newnode->next = add->next;
    newnode->prev = add;
    add->next = newnode;
    newnode->next->prev = newnode;

    g_GameState.snake_size++;
}

bool node_free()
{
    if(g_GameState.head == NULL) {
        return false;
    }

    node* current = g_GameState.head->next;
    node* next = NULL;

    // free everybody except head
    while(current != NULL && current != g_GameState.head) {
        next = current->next;
        free(current);
        current = next;
    }

    // free head
    free(g_GameState.head);
    g_GameState.snake_size = 0;
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
    node* aux = g_GameState.head;

    if(!aux) {
        return false;
    }

    do
    {
        if(node_same_coordinates(aux->x, aux->y, x, y)) {
            return true;
        }
        aux = aux->next;
    } while (aux != g_GameState.head);
    
    return false;
}

/////////////////////////////////////////////////////////////////////////////// opengl functions

void ogl_init()
{
    glColor3f(0.0, 0.0, 0.0);
}

void ogl_draw_cell(int x, int y)
{
    glRecti( x-5 ,  y-5 , x+5 , y+5 );
}

/////////////////////////////////////////////////////////////////////////////// gamestate functions

bool gamestate_init_node()
{
    // this was added because author never bother with it
    if(g_GameState.head != NULL) {
        if(g_GameState.head->next != NULL) {
            free(g_GameState.head->next);
        }

        if(g_GameState.head->prev != NULL) {
            free(g_GameState.head->prev);
        }
        free(g_GameState.head);
    }

    g_GameState.head = (node*)malloc(sizeof(node));

    if(!g_GameState.head) {
        return false;
    }

    g_GameState.head->next = NULL;
    g_GameState.head->prev = NULL;
    g_GameState.head->x = 40;
    g_GameState.head->y = 60;

    node* tail = g_GameState.head;
    node* aux = NULL;

    // build the snake body by adding segments after the head
    for(int32_t i = 0; i < g_GameState.snake_size; i++) {
        aux = (node*)malloc(sizeof(node));
        if(!aux) {
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
    memset(&g_GameState, 0, sizeof(gamestate));

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
    return gamestate_init_node();
}

void gamestate_wait(int32_t time)
{
    #ifdef _WIN32
    Sleep(time * 10);  // t * 10 milliseconds
    #else
    struct timespec rem;
    rem.tv_sec = 0;
    rem.tv_nsec = 10000000 * t;
    nanosleep(&rem, NULL);
    #endif
}

void gamestate_create_food()
{
    srand(time(0));

	do {
		g_GameState.food_x = rand() % 628 + 6	;
		g_GameState.food_y = rand() % 468 + 6;
	} while(node_search(g_GameState.food_x , g_GameState.food_y));
}

void gamestate_draw_snake()
{
    node* aux = g_GameState.head;

    if(!aux) {
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
    ogl_draw_cell(x , y);
}

void gamestate_draw_head(int32_t x, int32_t y)
{
    if( g_GameState.dir == direction_right || g_GameState.dir == direction_left )
    {
        glBegin(GL_TRIANGLES);
        glVertex2i( x - 5 , y + 5);
        glVertex2i( x     , y + 9);	
        glVertex2i( x + 5 , y + 5);
        glEnd();

        return;
    }

    glBegin(GL_TRIANGLES);
    glVertex2i( x + 5 , y - 5);
	glVertex2i( x + 9  , y   );	
	glVertex2i( x + 5 , y + 5);
	glEnd();
}

void gamestate_snake_move()
{
    int32_t move_x = g_GameState.change->prev-> x + g_GameState.change_x;
	int32_t move_y = g_GameState.change->prev-> y + g_GameState.change_y;

	if((move_x >= 640 ) || ( move_y >= 480) || ( move_x <= 0 ) || ( move_y <= 0 ) || search(move_x , move_y)) {
        display_out();
    }

	else
	{
		if(node_same_coordinates(move_x , move_y , g_GameState.food_x , g_GameState.food_y))
		{
			node_add(g_GameState.change);
            display_play();
            gamestate_create_food();
		}	

		g_GameState.change->x  =  move_x;
		g_GameState.change->y  =  move_y;

        gamestate_draw_snake();

        if(g_GameState.change != NULL) {
            gamestate_draw_head(g_GameState.change->x, g_GameState.change->y);
        }
		
		gamestate_draw_food(g_GameState.food_x, g_GameState.food_y);

		glFlush();

		gamestate_wait(g_GameState.speed);

		g_GameState.change = g_GameState.change->next;

		glutPostRedisplay();	
	}		
}

void gamestate_call_init()
{
    glPointSize(4.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);
	glClearColor(0.127, 0.252, 0.0, 0.0);
	initGame();
	g_GameState.play = true;
}

void gamestate_call_display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    if(g_GameState.play) {
        gamestate_snake_move();
    }

	else
	{
		display_play();
		gamestate_draw_snake();
		gamestate_draw_head(g_GameState.change->prev->x , g_GameState.change->prev->y);
		gamestate_draw_food(g_GameState.food_x, g_GameState.food_y);
		glFlush();
	}
}

void gamestate_call_keyboard(unsigned char key, int32_t x, int32_t y)
{
    switch(key)
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
    if(g_GameState.play)
	{
 		switch(key)
		{
			case GLUT_KEY_RIGHT:
            {
                if(g_GameState.dir != direction_left) {
					g_GameState.dir = direction_right;
					g_GameState.change_x = 11;
					g_GameState.change_y = 0;
				}
                break;
            }
		
			case GLUT_KEY_LEFT:
            {
                if(g_GameState.dir != direction_right) {
					g_GameState.dir = direction_left;
					g_GameState.change_x = -11;
					g_GameState.change_y = 0;
				}
                break;
            }

	  		case GLUT_KEY_UP:
            {
                if(g_GameState.dir != direction_down) {
					g_GameState.dir = direction_up;
					g_GameState.change_x = 0;
					g_GameState.change_y = 11;
				}
                break;
            }

			case GLUT_KEY_DOWN:
            {
                if(g_GameState.dir != direction_up) {
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

/////////////////////////////////////////////////////////////////////////////// print stuff

void display_clear_screen()
{
    #ifdef _WIN32
    #define CLEAR_SCREEN() system("cls");
    #else
    #define CLEAR_SCREEN() system("clear");
    #endif
}

void display_clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void display_help()
{
    CLEAR_SCREEN();

	printf("\n---------------------[ Snake Game ]------------------------\n");
	printf("\n              Developed by Rohit kadam, Modified by franzpedd\n\n");
	printf("-----------------------------------------------------------\n");
	printf("\n p : Start / Play / Pause ");
	printf("\n q : Exit");
	printf("\n     Use arrow key to move the snake ");
	printf("\n [1 - 9] : change level of your game");
	printf("\n-----------------------------------------------------------\n");

    display_clear_input_buffer();
}

void display_play()
{
    CLEAR_SCREEN();

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
