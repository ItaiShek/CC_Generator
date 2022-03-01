// 
// (c) 2021 Itai Shek
// https://github.com/ItaiShek/CC_Generator
// Generate a stack of credit card numbers fast and easy
// functions for generating the numbers
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "generatefuncs.h"

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
