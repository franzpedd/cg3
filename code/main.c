#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "littlesnake.h"

 int32_t main( int32_t argc, char** argv)
{
    display_help();

    while (1)
    {
        fflush(stdin);
        printf("\n\nEnter your Choice: ");

        char ch = '0';
        if (scanf(" %c", &ch) != 1) {
            fflush(stdin);
            continue;
        }

        // uppercase/lowercase input
        ch = tolower(ch);

        if (ch == 'p')
        {
            glutInit(&argc, argv);
            glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
            glutInitWindowSize(640, 480);
            glutInitWindowPosition(800, 300);
            glutCreateWindow("Snake Game");
            glutDisplayFunc(gamestate_call_display);
            glutKeyboardFunc(gamestate_call_keyboard);
            glutSpecialFunc(gamestate_call_keyboard_special);
            gamestate_call_init();
            glutMainLoop();
        }

        else if (ch == 'q') {
            display_quit();
        }
        else
        {
            printf("\nInvalid choice! Press 'p' to play or 'q' to quit.\n");
            display_help();
        }
    }

    return 0;
}