#include "littlesnake.h"

int main(int argc, char** argv)
{
    char ch;
    display_help();
    
    while(1)
    {
        display_clear_input_buffer();
        printf("\n\nEnter your Choice: ");
        
        if(scanf(" %c", &ch) != 1) {
            display_clear_input_buffer();
            continue;
        }
        
        // uppercase/lowercase input
        ch = tolower(ch);
        
        if(ch == 'p')
        {
            glutInit(&argc, argv);
            glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
            glutInitWindowSize(640, 480);
            glutInitWindowPosition(800, 300);
            glutCreateWindow("Snake Game");
            glutDisplayFunc(gamestate_call_display);
            glutSpecialFunc(gamestate_call_keyboard);
            glutKeyboardFunc(gamestate_call_keyboard_special);
            gamestate_call_init();
            glutMainLoop();
        }

        else if(ch == 'q') {
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