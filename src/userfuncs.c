// 
// (c) 2021 Itai Shek
// https://github.com/ItaiShek/CC_Generator
// Generate a stack of credit card numbers fast and easy
// user choices
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "userfuncs.h"

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
	char str[MAX_AMOUNT];// = {0};
	int str_size;
	char tempchar;

	memset(str, 0, MAX_AMOUNT);

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
