// 
// (c) 2021 Itai Shek
// https://github.com/ItaiShek/CC_Generator
// Generate a stack of credit card numbers fast and easy
//
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <ctype.h>

#include "generatefuncs.h"
#include "graphics.h"
#include "sighandler.h"
#include "userfuncs.h"

// #define MAX_AMOUNT 13	// billion + '\n'
// #define CHUNK 4096	// for writing to file

int keepRunning=0;
struct termios old_term;

int main()
{
	FILE *output;
	int card_type, amount=0;
	int ok=1;

	tcgetattr(0, &old_term);	// get stdin parameters
	create_signal();
	srandom(time(NULL));

	while(ok)
	{
		ok=0;

		// get input from the user
		card_type=get_card_type();
		amount=get_amount();
		output=get_output();
		if(output != stdout)
		{
			ok=estimate(amount, output);	
		}
	}

	keepRunning=1;
	generate(output, card_type, amount);

	if(output != stdout)
	{
		fclose(output);
	}

	return 0;
}
