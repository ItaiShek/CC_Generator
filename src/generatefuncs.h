// 
// (c) 2021 Itai Shek
// https://github.com/ItaiShek/CC_Generator
// Generate a stack of credit card numbers fast and easy
// functions for generating the numbers
//

#ifndef GENERATEFUNCS_H
#define GENERATEFUNCS_H

long int generate_card(int prefix, int len);						// generate a single credit card number
int checknumber(long int num);								// calculate the "check number" using luhn algorithm
void generate(FILE *output, int card_type, int amount);					// send the info to "generate_all()"
void generate_all(FILE *output, int amount, int prefixes[], int array_len, int len);	// generate all the credit card numbers of a specific type
extern void print_banner();
extern int keepRunning;
#define CHUNK 4096	// for writing to file

#endif	
