/*
Library function implementations
Name: Kameren Jouhal
*/

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <asm/termbits.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/select.h>
#include "iso_font.h"
#include <errno.h>

typedef unsigned short color_t;

// File descriptor
int file;
// Memory map pointer
char *map;
// y length
int y_pixel_length;
// x length
int x_pixel_length;
// x length
int x_byte_length;
// Size of the map
int map_size;

// Initializes the graphics
void init_graphics() {
	// Open the video display
	file = open("/dev/fb0", O_RDWR);
	// Check if it opened correctly
	if(file == -1) {
		perror("file failed to open");
		_exit(1);
	}
	// For handling return values
	int return_value;
	// Get information about the screen
	static struct fb_var_screeninfo s1;
	return_value = ioctl(file, FBIOGET_VSCREENINFO, &s1);
	// Check the return value
	if(return_value == -1) {
		perror("VSCREENINFO fail");
		_exit(1);
	}
	// Set y pixel length
	y_pixel_length = s1.yres_virtual;
	// Set x pixel length
	x_pixel_length = s1.xres_virtual;
	// Get information about the screen
	static struct fb_fix_screeninfo s2;
	return_value = ioctl(file, FBIOGET_FSCREENINFO, &s2);
	// Check the return value
	if(return_value == -1) {
		perror("FSCREENINFO fail");
		_exit(1);
	}
	// Set x byte length
	x_byte_length = s2.line_length;
	// Get the size of the map
	map_size = y_pixel_length * x_byte_length;
	// Set the terminal settings
	struct termios terminal;
	// File descriptor 0 is stdout
	return_value = ioctl(0, TCGETS, &terminal);
	// Check the return value
	if(return_value == -1) {
		perror("init ioctl gets");
		_exit(1);
	}
	// Set the ECHO and ICANON bits
	terminal.c_lflag &= ~ECHO;
	terminal.c_lflag &= ~ICANON;
	return_value = ioctl(0, TCSETS, &terminal);
	//Check the return value
	if(return_value == -1) {
		perror("init ioctl sets");
		_exit(1);
	}
	// Create a new memory mapping
	map = (char *)mmap(NULL, map_size, PROT_READ | PROT_WRITE,
	MAP_SHARED, file, 0);
	// Check if the mapping succeeded
	if(map == MAP_FAILED) {
		perror("map failed to allocate");
		_exit(1);
	}
}

// Cleans up before exiting the program
void exit_graphics() {
	// Reset the terminal settings
	int return_value;
	struct termios terminal;
	// File decriptor 0 is stdout
	return_value = ioctl(0, TCGETS, &terminal);
	// Check the return value
	if(return_value == -1) {
		perror("exit ioctl gets");
		_exit(1);
	}
	// Reset the ECHO and ICANON bits
	terminal.c_lflag |= ~ECHO;
	terminal.c_lflag |= ~ICANON;
	return_value = ioctl(0, TCSETS, &terminal);
	// Check the return value
	if(return_value == -1) {
		perror("exit ioctl sets");
		_exit(1);
	}
	// Unmap the memory mapping
	int unmap = munmap(map, map_size);
	// Check if the unmapping succeeded
	if(unmap != 0) {
		perror("unmap");
		_exit(1);
	}
	// Close the file
	int closed = close(file);
	if(closed != 0) {
		perror("close file");
		_exit(1);
	}
}

// Put program to sleep while graphics are drawn
void sleep_ms(long ms) {
	// Initialize timespec struct and set nanoseconds
	struct timespec time;
	time.tv_sec = 0;
	time.tv_nsec = ms * 100;
	// Call nanosleep
	int return_value = nanosleep(&time, NULL);
	// Check return value
	if(return_value == -1) {
		perror("nanosleep");
		_exit(1);
	}
}

// Key pressed input
char getkey() {
	// Instatiate a fd_set
	fd_set set;
	// Add standard input to the set
	FD_SET(0, &set);
	// Instantiate a return value
	int return_value;
	// Call select
	return_value = select(1, &set, NULL, NULL, NULL);
	// Check the return value
	if(return_value == -1) {
		perror("getkey");
		return -1;
	}
	// Otherwise, read the key
	else {
		// Instantiate a variabke for the input
		char key[1];
		// Instantiate a return value handler
		int r;
		// Call read
		r = read(0, key, 1);
		// Check if we read anything
		if(r > 0) {
			return *key;
		}
		// Otherwise return -1
		else {
			return -1;
		}
	}
}

// Draws pixels to the screen
void draw_pixel(int x, int y, color_t color) {
	// Check if the x coordinate is valid
	if(x >= x_pixel_length || x < 0) {
		return;
	}
	// Check if the y coordinate is valid
	if(y >= y_pixel_length || y < 0) {
		return;
	}
	// Get the index of the map
	int index = y * x_byte_length + x * 2;
	// Get the 8 most significant bits of the color
	unsigned char msb = (color >> 8) & 0xFF;
	// Get the 8 least significant bits of the color
	unsigned char lsb = color & 0xFF;
	// Set the pixel on the map
	map[index] = msb;
	map[index+1] = lsb;
}

// Draws a rectangle to the screen
void draw_rect(int x1, int y1, int width, int height, color_t c) {
	// Get the x2 and y2 coordinates
	int x2 = x1 + width;
	int y2 = y1 + height;
	// Initialize a for loop counter
	int i = 0;
	// Loop until counter = width
	for(i; i < width; ++i) {
		draw_pixel(x1 + i, y1, c);
		draw_pixel(x1 + i, y2, c);
	}
	// Reset the for loop counter
	i = 0;
	// Loop until counter = height
	for(i; i < height; ++i) {
		draw_pixel(x1, y1 + i, c);
		draw_pixel(x2, y1 + i, c);
	}
	// Draw the final pixel
	draw_pixel(x2, y2, c);
}

// Draws a circle to the screen
void draw_circle(int x, int y, int r, color_t c) {
	// Jesko's method for drawing a circle
	// From schwarzers.com/algorithms/
	// Initialize all variables
	int temp_y = 0;
	int t1 = r >> 4;
	int temp_x = r;
	int t2;
	// Loop until the circle is drawn
	while(temp_x >= temp_y) {
		// Draw the symmetrical pixels
		draw_pixel(x + temp_x, y + temp_y, c);
		draw_pixel(x - temp_x, y + temp_y, c);
		draw_pixel(x + temp_x, y - temp_y, c);
		draw_pixel(x - temp_x, y - temp_y, c);
		draw_pixel(x + temp_y, y + temp_x, c);
		draw_pixel(x - temp_y, y + temp_x, c);
		draw_pixel(x + temp_y, y - temp_x, c);
		draw_pixel(x - temp_y, y - temp_x, c);
		// Do the rest of the operations
		++temp_y;
		t1 += temp_y;
		t2 = t1 - temp_x;
		if(t2 >= 0) {
			t1 = t2;
			--temp_x;
		}
	}
}

// Draws a character to the screen
void draw_char(int x, int y, char c, color_t color) {
	// Get the index of the character in the isofont
	int iso_index = c * 16;
	// Get the index of the mmap
	int map_index = y * x_byte_length + x * 2;
	// Create an unsigned char for the character
	unsigned char character;
	unsigned char temp;
	int temp_map_index;
	int temp_x = x;
	int temp_y = y;
	// For loop counter
	int i = 0;
	// Loop for 16 iterations
	for(i; i < 16; ++i) {
		// For each iteration, the next encoding is at the
		// starting index + i
		character = iso_font[iso_index + i];
		// For loop counter
		int j = 0;
		// Loop through the bits in charactdr
		for(j; j < 8; ++j) {
			// The map index is the starting map index +
			// j * 2
			temp_map_index = map_index + j * 2;
			// Increment temp_x by j
			temp_x += j;
			// Mask off the lsb
			temp = character & 0x1;
			// If temp is 1, draw a pixel
			if(temp == 1) {
				draw_pixel(temp_x, temp_y, color);
			}
			// Shift character right by 1
			character = character >> 1;
			// Reset temp_x
			temp_x = x;
		}
		// Go to the next row in the screen
		map_index += x_byte_length;
		temp_y += 1;
	}
}

// Draws text to the screen based on Apple's format
void draw_text(int x, int y, const char *text, color_t c) {
	// Initialize a counter
	int counter = 1;
	// Initialize temp x and y
	int temp_x = x;
	int temp_y = y;
	// Initialize a char variable to the first character in text
	char character = *text;
	// Loop until the end of the string
	while(character != '\0') {
		// Draw the character to the screen
		draw_char(temp_x, temp_y, character, c);
		// Incrememnt temp_x by 8
		temp_x += 8;
		// Set character to the next character in text
		character = *(text + counter);
		// Incrememnt counter
		++counter;
	}
}

// Clears the screen
void clear_screen() {
	char* string = "\033[2J";
	write(1, string, 4);
}
