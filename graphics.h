/*
Header file for library
Name: Kameren Jouhal
*/

typedef unsigned short color_t;

// Initializes the graphics
void init_graphics();

// Cleans up before exiting the program
void exit_graphics();

// Key pressed input
char getkey();

// Put program to sleep while graphics are drawn
void sleep_ms(long ms);

// Clears the screen
void clear_screen();

// Draws pixels to the screen
void draw_pixel(int x, int y, color_t color);

// Draws a rectangle to the screen
void draw_rect(int x1, int y1, int width, int height, color_t c);

// Draws text to the screen
void draw_text(int x, int y, const char *text, color_t c);

// Draws a cirlce to the screen
void draw_circle(int x, int y, int r, color_t c);
