/*
Driver: Game where you get points by moving a player into coins
Name: Kameren Jouhal
*/

#include "graphics.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main() {
	// Initialize the graphics
	init_graphics();
	clear_screen();
	// Create a boolean variable
	char boolean = 0;
	// Intro messages
	char *welcome = "Welcome to the Game!";
	char *instructions = "You will move a square around the";
	char *instructions2 = "screen and collect coins";
	char *instructions3 = "Use WSAD to move";
	char *warning = "Warning: if you go off the screen";
	char *warning2 = "you may have to restart the game";
	char *start = "Press 'o' to start the game!";
	// Draw intro messages
	sleep_ms(1000);
	draw_text(150, 100, welcome, 0xFFFF);
	draw_text(150, 125, instructions, 0xFFFF);
	draw_text(150, 142, instructions2, 0xFFFF);
	draw_text(150, 175, instructions3, 0xFFFF);
	draw_text(150, 200, warning, 0x0FA0);
	draw_text(150, 217, warning2, 0x0FA0);
	draw_text(150, 250, start, 0xFFFF);
	// Start the game when 'o' has been pressed
	while(!boolean) {
		char s = getkey();
		if(s == 'o' || s == 'O') {
			boolean = 1;
		}
	}
	// Clear the screen
	clear_screen();
	// Draw the player in the starting position
	sleep_ms(1000);
	draw_rect(100, 100, 5, 5, 0xFFFF);
	// Initialize variables for the x and y coordinate of the player
	int rect_x = 100;
	int rect_y = 100;
	// Initialize random number generator
	srand(time(0));
	// Instantiate a random circle x and y
	int circ_x = rand() % (590 - 50 + 1) + 50;
	int circ_y = rand() % (430 - 50 + 1) + 50;
	// Draw the circle
	draw_circle(circ_x, circ_y, 5, 0xF00F);
	// Instantiate a score variable to 0
	int score = 0;
	// Instantiate strings for the score
	char *score_message = "Score:";
	char score_string[100];
	// Instantiate a string to tell the user to press 'p' to end
	// the game
	char *end = "Press 'p' to end the game";
	// Instantiate a message about the circle going off the screen
	char *disappear = "If the circle disappears, press 'r'";
	// Covert the score int to the string
	sprintf(score_string, "%d", score);
	// Display the text
	draw_text(10, 30, score_message, 0x0FA0);
	draw_text(60, 30, score_string, 0x0FA0);
	draw_text(100, 30, end, 0x0FA0);
	draw_text(325, 30, disappear, 0x0FA0);
	// Loop until 'p' is pressed
	while(boolean) {
		// Put the program to sleep
		sleep_ms(10000);
		// Get a key for the user input
		char key = getkey();
		// If key is greater than 0, clear the screen for
		// graphics update
		if(key > 0) {
			clear_screen();
		}
		// If the key is w, decrement rect_y
		if(key == 'w' || key == 'W') {
			rect_y -= 5;
		}
		// If the key is s, increment rect_y
		if(key == 's' || key == 'S') {
			rect_y += 5;
		}
		// If the key is a, decrement rect_x
		if(key == 'a' || key == 'A') {
			rect_x -= 5;
		}
		// If the key is d, increment rect_x
		if(key == 'd' || key == 'D') {
			rect_x += 5;
		}
		// If the player touches the coin, move the coin
		if((circ_x >= rect_x && circ_x <= rect_x + 5)
		&& (circ_y >= rect_y && circ_y <= rect_y + 5)) {
			// Increment the score and convert the score
			// into a string
			score += 1;
			sprintf(score_string, "%d", score);
			circ_x = rand() % (590 - 50 + 1) + 50;
			circ_y = rand() % (430 - 50 + 1) + 50;
		}
		// If the key is r, reset the circle location
		if(key == 'r' || key == 'R') {
			circ_x = rand() % (590 - 50 + 1) + 50;
			circ_y = rand() % (430 - 50 + 1) + 50;
		}
		// Draw the graphics
		draw_rect(rect_x, rect_y, 5, 5, 0xFFFF);
		draw_circle(circ_x, circ_y, 5, 0xF00F);
		draw_text(10, 30, score_message, 0x0FA0);
		draw_text(60, 30, score_string, 0x0FA0);
		draw_text(100, 30, end, 0x0FA0);
		draw_text(325, 30, disappear, 0x0FA0);
		// If the key is p, end the game
		if(key == 'p' || key == 'P') {
			boolean = 0;
			char *game_over = "Game Over!";
			draw_text(250, 200, game_over, 0x0FA0);
		}
	}
	// Exit the graphics
	exit_graphics();
	return 0;
}
