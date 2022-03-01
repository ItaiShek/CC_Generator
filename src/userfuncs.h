// 
// (c) 2021 Itai Shek
// https://github.com/ItaiShek/CC_Generator
// Generate a stack of credit card numbers fast and easy
// user choices
//

#ifndef USERFUNCS_H
#define USERFUNCS_H

int get_card_type();			// what type of credit card to generate
int get_amount();			// how many credit card numbers to generate
FILE *get_output();			// stdout or file
int estimate(int amount, FILE *output);	// estimate the file's size
extern void print_banner();
extern int menu(char title[], char *options[], int len, FILE *output);

#define MAX_AMOUNT 13	// billion + '\n'

#endif
