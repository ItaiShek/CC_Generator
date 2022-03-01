// 
// (c) 2021 Itai Shek
// https://github.com/ItaiShek/CC_Generator
// Generate a stack of credit card numbers fast and easy
// handle SIGINT
//
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include "sighandler.h"

// craete a signal for ctrl-c
void create_signal()
{
	struct sigaction sa;
	sa.sa_handler=sigint_handler;
	sa.sa_flags=0;
	if(sigemptyset(&sa.sa_mask) == -1)
	{
		perror("sigemptyset");
		exit(1);
	}
	if(sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}
}

// ctrl-c handler
void sigint_handler()
{
	tcsetattr(0, TCSANOW, &old_term);

	puts("\nThe program was terminated by the user");

	if(!keepRunning)
	{
		exit(1);
	}

	keepRunning=0;
}
