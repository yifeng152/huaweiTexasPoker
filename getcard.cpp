#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getcard.h"

struct CardInfo {
	char Color[10];
	char Num[4];
};

// global function 
void GetHoldCard(const char *head4Hold, char *holdCard);
void GetFlopCard(const char *head4Flop, char *flopCard);
void GetTurnCard(const char *head4Turn, char *turnCard);
void GetRiverCard(const char *head4River, char *riverCard);

// local function 
static char GetLogicValue(const char *colorStr,const char *numStr);


void GetHoldCard(const char *head4Hold, char *holdCard)
{
	struct CardInfo firstCard, secondCard;
	sscanf(head4Hold,"hold/\n%s %s\n%s %s\n/hold",firstCard.Color,
			firstCard.Num,secondCard.Color,secondCard.Num);

	holdCard[0] = GetLogicValue(firstCard.Color, firstCard.Num);
	holdCard[1] = GetLogicValue(secondCard.Color, secondCard.Num);
}

void GetFlopCard(const char *head4Flop, char *flopCard)
{	
	struct CardInfo thirdCard, fourthCard, fifthCard;
	sscanf(head4Flop,"flop/\n%s %s\n%s %s\n%s %s\n/flop",thirdCard.Color,
		thirdCard.Num, fourthCard.Color, fourthCard.Num, fifthCard.Color,
		fifthCard.Num);

	flopCard[0] = GetLogicValue(thirdCard.Color,  thirdCard.Num);
	flopCard[1] = GetLogicValue(fourthCard.Color, fourthCard.Num);
	flopCard[2] = GetLogicValue(fifthCard.Color,  fifthCard.Num);
}

void GetTurnCard(const char *head4Turn, char *turnCard)
{
	struct CardInfo sixthCard;
	sscanf(head4Turn,"turn/\n%s %s\n/turn", 
		sixthCard.Color, sixthCard.Num);

	turnCard[0] = GetLogicValue(sixthCard.Color, sixthCard.Num);
}

void GetRiverCard(const char *head4River, char *riverCard)
{
	struct CardInfo seventhCard;
	sscanf(head4River,"river/\n%s %s\n/river", 
		seventhCard.Color, seventhCard.Num);

	riverCard[0] = GetLogicValue(seventhCard.Color, seventhCard.Num);
}

char GetLogicValue(const char *colorStr,const char *numStr)
{
	char logicValue = 0;

	if (strcmp(colorStr, "DIAMONDS") == 0)
		logicValue |= 0x00;
	else if (strcmp(colorStr, "CLUBS") == 0)
		logicValue |= 0x10;
	else if (strcmp(colorStr, "HEARTS") == 0)
		logicValue |= 0x20;
	else if (strcmp(colorStr, "SPADES") == 0)
		logicValue |= 0x30;

	if (strcmp(numStr, "A") == 0)
		logicValue |= 0x01;	
	else if (strcmp(numStr, "10") == 0)
		logicValue |= 0x0A;
	else if (strcmp(numStr, "J") == 0)
		logicValue |= 0x0B;
	else if (strcmp(numStr, "Q") == 0)
		logicValue |= 0x0C;
	else if (strcmp(numStr, "K") == 0)
		logicValue |= 0x0D;
	else
	{
		int decValue;
		sscanf(numStr, "%d", &decValue);
		logicValue |= decValue;
	}
	return logicValue;
}
