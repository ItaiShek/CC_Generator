// 
// (c) 2021 Itai Shek
// https://github.com/ItaiShek/CC_Generator
// Generate a stack of credit card numbers fast and easy
// manu, banner and arrow functions
// 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include "graphics.h"

// print banner, version, etc
void print_banner()
{
	system("clear");
	printf("\n");
	printf("    $$$$$$\\   $$$$$$\\                                                                    $$\\                         \n");            
	printf("   $$  __$$\\ $$  __$$\\                                                                   $$ |                        \n");                 
	printf("   $$ /  \\__|$$ /  \\__|       $$$$$$\\   $$$$$$\\  $$$$$$$\\   $$$$$$\\   $$$$$$\\  $$$$$$\\ $$$$$$\\    $$$$$$\\   $$$$$$\\  \n");              
	printf("   $$ |      $$ |            $$  __$$\\ $$  __$$\\ $$  __$$\\ $$  __$$\\ $$  __$$\\ \\____$$\\\\_$$  _|  $$  __$$\\ $$  __$$\\ \n");                    
	printf("   $$ |      $$ |            $$ /  $$ |$$$$$$$$ |$$ |  $$ |$$$$$$$$ |$$ |  \\__|$$$$$$$ | $$ |    $$ /  $$ |$$ |  \\__|\n");                 
	printf("   $$ |  $$\\ $$ |  $$\\       $$ |  $$ |$$   ____|$$ |  $$ |$$   ____|$$ |     $$  __$$ | $$ |$$\\ $$ |  $$ |$$ |      \n");              
	printf("   \\$$$$$$  |\\$$$$$$  |      \\$$$$$$$ |\\$$$$$$$\\ $$ |  $$ |\\$$$$$$$\\ $$ |     \\$$$$$$$ | \\$$$$  |\\$$$$$$  |$$ |      \n");               
	printf("    \\______/  \\______/        \\____$$ | \\_______|\\__|  \\__| \\_______|\\__|      \\_______|  \\____/  \\______/ \\__|      \n");             
	printf("                             $$\\   $$ |                                                                              \n");              
	printf("                             \\$$$$$$  |                                                                              \n");          
	printf("                              \\______/                                                                               \n");          
	printf("    Version 1.0\n");
	printf("    https://github.com/ItaiShek/CC_Generator\n\n");


}


// draw a menu with options
int menu(char title[], char *options[], int len, FILE *output)
{

	int position=0;
	int i, j;
	int columns=len/10 + 1;
	int rows=len/columns;
	char ch;
	struct termios new_term;

	if(len%columns != 0)
	{
		rows = (len+columns)/columns;
	}

	memcpy(&new_term, &old_term, sizeof(struct termios));

	// keystroke		
	new_term.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(0, TCSANOW, &new_term);

	do 
	{
		// system("clear");
		print_banner();
		printf("    %s\n\n", title);
		for(i=0; i<rows; i++)
		{
			for(j=0; j<columns; j++)
			{
				if(i*columns+j >= len)
				{
					break;
				}
				arrow(i*columns+j, position);
				printf("%s", options[i*columns+j]);
			}
			printf("\n");
		}

		arrow(i*columns, position);
		printf("Exit\n");

		ch = fgetc(stdin);

		if(ch == 66 && position<len)	// down
		{
			if((position+columns)<len || (position+columns)==rows*columns)
			{
				position+=columns;
			}
		}
		else if(ch == 65 && position>0 && (position-columns)>=0)	// up 
		{
			position-=columns;
		}
		else if(ch == 68 && position%columns)	// left
		{
			position--;
		}
		else if(ch == 67 && (position+1)%columns && (position+1)<len)	// right
		{
			position++;
		}
	}
	while(ch != 10);	// enter key

	// restore previous state
	tcsetattr(0, TCSANOW, &old_term);

	if(position ==rows*columns)	// Exit option
	{
		if(output)
		{
			fclose(output);
		}
		printf("exit");
		exit(0);
	}

	return position+1;
}


void arrow(int i, int position)
{
	if(i == position)
	{
		printf("    (*) ");
	}
	else
	{
		printf("    ( ) ");
	}
}
