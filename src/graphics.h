// 
// (c) 2021 Itai Shek
// https://github.com/ItaiShek/CC_Generator
// Generate a stack of credit card numbers fast and easy
// manu, banner and arrow functions
// 
#ifndef GRAPHICS_H
#define GRAPHICS_H

void print_banner();						// print banner, version, etc
int menu(char title[], char *options[], int len, FILE *output);	// draw a menu with options
void arrow(int i, int position);
extern struct termios old_term;

#endif
