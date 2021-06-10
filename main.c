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

#define MAX_AMOUNT 13	// billion + '\n'
#define CHUNK 4096	// for writing to file

int keepRunning=0;
struct termios old_term;

void sigint_handler();
void create_signal();

void print_banner();
int menu(char title[], char *options[], int len, FILE *output);
void arrow(int i, int position);

long int generate_card(int prefix, int len);
int checknumber(long int final);

int estimate(int amount, FILE *output);
int get_card_type();
int get_amount();
FILE *get_output();
void generate(FILE *output, int card_type, int amount);
void generate_all(FILE *output, int amount, int prefixes[], int array_len, int len);


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

// what type of credit card to generate
int get_card_type()
{
	char *options[]={	"Visa                        ",
				"MasterCard                  ",
				"American Express            ",
				"Visa Electron               ",
				"China UnionPay              ",
				"Maestro                     ",
				"Maestro UK                  ",
				"Diners Club International   ",
				"Diners Club US & Ca         ",
				"Diners Club enRoute         ",
				"Diners Club Carte Blanche   ",
				"Discover                    ",
				"Interpayment                ",
				"Intstapayment               ",
				"JCB                         ",
				"Dankort                     ",
				"UATP                        ",
				"Bankcard                    ",
				"China T-Union               ",
				"RuPay                       ",
				"Laser                       ",
				"Solo                        ",
				"Ca Imperial Bank of Commerce",
				"Royal Bank of Canada        ",
				"TD Canada Trust             ",
				"Scotiabank                  ",
				"BMO                         ",
				"HSBC Bank Canada            ",
				"MIR                         ",
				"NPS Pridnestrovie           ",
				"Troy                        ",
				"Verve                       ",
				"LankaPay                    "
				};
	int options_len=sizeof(options)/sizeof(options[0]);

	return menu("Choose card type:", options, options_len, NULL);	

}

// how many credit card numbers to generate
int get_amount()
{
	int i, amount=0;
	char str[MAX_AMOUNT] = {0};
	int str_size;
	char tempchar;

	// system("clear");
	print_banner();
	printf("    Enter the amount of credit card numbers to generate: [ maximum: %d digit number]\n    ", MAX_AMOUNT-1);
	fgets(str, MAX_AMOUNT, stdin);

	str[strcspn(str, "\n")]=0;	// remove \n from str
	if((str_size=strlen(str)) == MAX_AMOUNT-1)	// remove trailing characters
	{
		while((tempchar=getchar()) != '\n' && tempchar != EOF);
	}
	
	for(i=0; i<str_size; i++)
	{
		if(!isdigit(str[i]))
		{
			perror("Not a number");
			exit(1);
		}
	}
	sscanf(str, "%d", &amount);

	return amount;
}

// stdout or file
FILE *get_output()
{
	FILE *output=NULL;
	char filename[51]={0};
	char tempchar;
	int check=1;
	char *options[]={	"File",
				"Screen",
				};
	int options_len=sizeof(options)/sizeof(options[0]);

	char *yes_no[]={	"No",
				"Yes"
				};
	int yes_no_len=sizeof(yes_no)/sizeof(yes_no[0]);

	if(menu("Write the output to:", options, options_len, NULL) - 1)
	{
		// screen
		output=stdout;
	}
	else
	{
		// file
		while(check)
		{
			// system("clear");
			print_banner();
			printf("    Enter the file's name:\n    ");
			fgets(filename, 51, stdin);

			filename[strcspn(filename, "\n")]=0;	// remove \n from filename
			if(strlen(filename) == MAX_AMOUNT-1)	// remove trailing characters
			{
				while((tempchar=getchar()) != '\n' && tempchar != EOF);
			}
		
			// check if the file exists
			if((output = fopen(filename, "r")))
			{
				fclose(output); 
				output=NULL;
				if(menu("The file already exists, do you want to overwrite it?", yes_no, yes_no_len, NULL) - 1)
				{
					// Yes
					check=0;
				}
			}
			else
			{
				// the file doesn't exists
				check=0;
			}
		}
		
		// create/overwrite the file
		if(!(output=fopen(filename, "w")))
		{
			perror("Could not create file");
			exit(1);
		}

	}

	return output;
}

// estimate the file's size
int estimate(int amount, FILE *output)
{
	char *sizes[] = {"B", "KB", "MB", "GB", "TB"};
	char *options[]={	"No",
				"Yes"
				};
	int options_len=sizeof(options)/sizeof(options[0]);
	int i;
	char title[70];
	double size=amount*18;
	for(i=0; i<5; i++)
	{
		if(size<1024)
		{
			break;
		}
		size /= 1024.0;
	}

	sprintf(title, "The file size will be %.2f %s, do you wish to continue?", size, sizes[i]); 

	if(menu(title, options, options_len, output) - 1)
	{
		// yes
		return 0;
	}

	// no
	return 1;
}

// generate a single credit card number
long int generate_card(int prefix, int len)
{
	long int final=prefix;
	int i;
		
	for(i=0; i<len; i++)
	{
		final *= 10;
		final += random()%10;
	}

	final *= 10;
	final += checknumber(final);

	return final;
}

// calculate the "check number" using luhn algorithm
int checknumber(long int num)
{
	int nextdigit;
	int total=0;
	int i=1;

	while(num > 0)
	{
		nextdigit = num%10;
		num /= 10;

		if(i%2 == 0 && (nextdigit*=2) >= 10)
		{
			nextdigit -= 9; 
		}

		total += nextdigit;
		i++;
	}

	total = total%10;
	if(total)
	{
		total = 10-total;
	}

	return total;
}

// send the info to "generate_all()"
void generate(FILE *output, int card_type, int amount)
{
	int *prefixes=NULL;
	int array_len;
	int len=16;
	int i;
	print_banner();
	switch(card_type)
	{
		// Visa
		case 1:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=4;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// MasterCard
		case 2:
			array_len=800;	// bigger chance of getting a prefix in the range 51-55
			prefixes = (int *) malloc(sizeof(int)*array_len);
			for(i=0; i<300; i++)
			{
				prefixes[i] = 51 + i%5;
			}
			for(i=0; i<array_len-300; i++)
			{
				prefixes[i+300] = 2221 + i%500;
			}
			generate_all(output, amount, prefixes, array_len, len);
 			break;
		// American Express
		case 3:
			array_len=2;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=34;
			prefixes[1]=37;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Visa Electron
		case 4:
			array_len=6;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=4026;
			prefixes[1]=417500;
			prefixes[2]=4508;
			prefixes[3]=4844;
			prefixes[4]=4913;
			prefixes[5]=4917;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// China UnionPay
		case 5:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=62;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Maestro
		case 6:
			array_len=8;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=5018;
			prefixes[1]=5020;
			prefixes[2]=5038;
			prefixes[3]=5893;
			prefixes[4]=6304;
			prefixes[5]=6759;
			prefixes[6]=6761;
			prefixes[7]=6762;
			prefixes[8]=6763;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Maestro UK
		case 7:
			array_len=3;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=6759;
			prefixes[1]=676770;
			prefixes[2]=676774;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Diners Club International
		case 8:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=36;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Diners Club US & Ca
		case 9:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=54;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Diners Club enRoute
		case 10:
			array_len=2;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=20;
			prefixes[1]=21;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Diners Club Carte Blanche
		case 11:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=30;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Discover
		case 12:
			array_len=808;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=65;
			prefixes[1]=6011;
			for(i=0; i<6; i++)
			{
				prefixes[i+2] = 644 + i;
			}
			for(i=0; i<800; i++)
			{
				prefixes[i+8] = 622126 + i;
			}
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Interpayment
		case 13:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=636;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Intstapayment
		case 14:
			array_len=3;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=637;
			prefixes[1]=638;
			prefixes[2]=639;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// JCB
		case 15:
			array_len=62;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			for(i=0; i<array_len; i++)
			{
				prefixes[i] = 3528 + i;
			}
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Dankort
		case 16:
			array_len=2;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=5019;
			prefixes[1]=4571;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// UATP
		case 17:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=1;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Bankcard
		case 18:
			array_len=6;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=5610;
			for(i=0; i<5; i++)
			{
				prefixes[i+1]=560221 + i;
			}
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// China T-Union 
		case 19:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=31;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// RuPay
		case 20:
			array_len=3;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=60;
			prefixes[1]=6521;
			prefixes[2]=6522;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Laser
		case 21:
			array_len=4;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=6304;
			prefixes[1]=6706;
			prefixes[2]=6771;
			prefixes[3]=6709;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Solo
		case 22:
			array_len=2;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=6334;
			prefixes[1]=6767;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Canadian Imperial Bank of Commerce
		case 23:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=4506;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Royal Bank of Canada
		case 24:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=45;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// TD Canada Trust
		case 25:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=4724;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Scotiabank 
		case 26:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=4536;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// BMO 
		case 27:
			array_len=2;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=500;
			prefixes[1]=5510;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// HSBC Bank Canada
		case 28:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=56;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// MIR
		case 29:
			array_len=5;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=2200;
			prefixes[1]=2201;
			prefixes[2]=2202;
			prefixes[3]=2203;
			prefixes[4]=2204;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// NPS Pridnestrovie
		case 30:
			array_len=5;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=6054740;
			prefixes[1]=6054741;
			prefixes[2]=6054742;
			prefixes[3]=6054743;
			prefixes[4]=6054744;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Troy 
		case 31:
			array_len=4;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=6;
			prefixes[1]=7;
			prefixes[2]=8;
			prefixes[3]=9;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// Verve
		case 32:
			array_len=126;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			for(i=0; i<100; i++)
			{
				prefixes[i] = 506099 + i;
			}
			for(i=0; i<26; i++)
			{
				prefixes[i+100] = 650002 + i;
			}
			generate_all(output, amount, prefixes, array_len, len);
			break;
		// LankaPay
		case 33:
			array_len=1;
			prefixes = (int *) malloc(sizeof(int)*array_len);
			prefixes[0]=357111;
			generate_all(output, amount, prefixes, array_len, len);
			break;
		default:
			break;
	}

	if(prefixes)
	{
		free(prefixes);
	}
}

// generate all the credit card numbers of a specific type
void generate_all(FILE *output, int amount, int prefixes[], int array_len, int len)
{
	int bufferCount=0;

	char cc_num[CHUNK + 46];
	int i=0;

	int prefix;
	int prefix_len;
	int newlen;

	float percent;
	char progress[30];
	int progress_len = 30;
	memset(progress, '_', progress_len);

	while(keepRunning && i<amount)
	{
		prefix = prefixes[random()%array_len];
		prefix_len = log10(prefix) + 1;
		newlen = len - prefix_len - 1;		// length minus check number and prefix

		bufferCount += sprintf(&cc_num[bufferCount], "%ld\n", generate_card(prefix, newlen));

		// write chunk of data
		if(bufferCount >= CHUNK)
		{
			fwrite(cc_num, CHUNK, 1, output);
			bufferCount -= CHUNK;
			memcpy(cc_num, &cc_num[CHUNK], bufferCount);
		}

		// progress bar
		if(output != stdout)
		{
			percent = (i+1)*100.0/amount;
			memset(progress, '#', (int)(percent*progress_len/100));
			printf("\r    [%s] %.2f%%", progress, percent);
			fflush(stdout);
		}
		i++;
	}

	// write the remainder
	if(bufferCount > 0)
	{
		fwrite(cc_num, bufferCount, 1, output);
	}
}
