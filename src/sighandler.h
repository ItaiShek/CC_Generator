// 
// (c) 2021 Itai Shek
// https://github.com/ItaiShek/CC_Generator
// Generate a stack of credit card numbers fast and easy
// handle SIGINT
//
#ifndef SIGHANDLER_H
#define SIGHANDLER_H

void create_signal();	// craete a signal for ctrl-c
void sigint_handler();	// ctrl-c handler
extern struct termios old_term;
extern int keepRunning;

#endif
