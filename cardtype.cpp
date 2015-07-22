#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "cardtype.h"

#define	LOGIC_MASK_COLOR		0xF0	// Color Mask
#define	LOGIC_MASK_VALUE		0x0F	// Numerical Mask

#define  FULL_COUNT 52					// Total Nubmer Of Card

struct tagAnalyseResult
{
	char 	cbFourCount;				// Count of Four
	char 	cbThreeCount;				// Count of Three
	char 	cbLONGCount;				// Count of Two
	char	cbSignedCount;				// Count of One
	char 	cbFourLogicVolue[1];		// Logic List of Four
	char 	cbThreeLogicVolue[1];		// Logic List of Three
	char 	cbLONGLogicVolue[2];		// Logic List of Two
	char 	cbSignedLogicVolue[5];		// Logic List of One
	char	cbFourCardData[5];			// List of Four
	char	cbThreeCardData[5];			// List of Three
	char	cbLONGCardData[5];			// List of Two
	char	cbSignedCardData[5];		// List of One
};

//////////////////////////////////////////////////////////////////////////
// global function
char GetFiveCardsType(char cbCardData[], char cbCardCount);				// Get Five Cards Type

char GetGreatestTypeFromSix(char cbCardData[],char cbCardCount);		// Get greatest type from six cards

char GetGreatestTypeFromSeven(char cbCardData[],char cbCardCount);		// Get greatest type from seven cards

bool MayFormStraight(char cbCardData[], char cbCardCount);				// May form straight

bool MayFormFlush(char cbCardData[], char cbCardCount);					// May form flush

void SortCardList(char cbCardData[], char cbCardCount);					// Sort Card

// local function
static char GetSortedCardsType(char cbCardData[], char cbCardCount);	// Get Five Sorted Cards Type

static char GetCardValue(char cbCardData);								// Get Value

static char GetCardColor(char cbCardData);								// Get Color

static char GetCardLogicValue(char cbCardData);							// Logic Of Number
															
static char CompareCard(char cbFirstData[], char cbNextData[],			// Compare Card
	char cbCardCount);

static void AnalysebCardData(const char cbCardData[],					// Analyse Card
	char cbCardCount, tagAnalyseResult &AnalyseResult);
																		// Get the Type of Greatest Five From Seven Cards
static char GetGreatestFiveFromSeven(char cbHandCardData[],char cbHandCardCount,	
	char cbCenterCardData[],char cbCenterCardCount,char cbLastCardData[],char cbLastCardCount);
																		// Get the Type of Greatest Five From Six Cards
static char GetGreatestFiveFromSix(char cbHandCardData[],char cbHandCardCount,
	char cbCenterCardData[],char cbCenterCardCount,char cbLastCardData[],char cbLastCardCount);	

void SortCardListByColor(char cbCardData[], char cbCardCount);
//////////////////////////////////////////////////////////////////////////

// All the Cards(52)

// const char g_cbCardData[FULL_COUNT] =
// {
// 	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	// DIAMONDS  A->2->...->K
// 	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	// CLUBS	 A->2->...->K
// 	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	// HEARTS	 A->2->...->K
// 	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	// SPADES	 A->2->...->K
// };


/*
** Function: Get type from five cards.
*/
char GetFiveCardsType(char Data[], char Count)		
{
	char cbCardData[5] = {Data[0], Data[1], Data[2], Data[3], Data[4]};
	char cbCardCount = Count;

	SortCardList(cbCardData, cbCardCount);				// Sort cards
	
	return GetSortedCardsType(cbCardData, cbCardCount);	// Get five sorted cards type
}

/*
** Function: Get greatest type from six cards
*/
char GetGreatestTypeFromSix(char cbCardData[], char cbCardCount)
{
	char cbHandCardData[2] = {cbCardData[0], cbCardData[1]};
	char cbHandCardCount = 2;
	char cbCenterCardData[4] = {cbCardData[2], cbCardData[3], cbCardData[4], cbCardData[5]};
	char cbCenterCardCount = 4;
	char cbLastCardData[5];
	char cbLastCardCount = 5;

	return GetGreatestFiveFromSix(cbHandCardData, cbHandCardCount, cbCenterCardData, cbCenterCardCount, cbLastCardData, cbLastCardCount);	
}

/*
** Function: Get greatest type from seven cards
*/
char GetGreatestTypeFromSeven(char cbCardData[], char cbCardCount)
{
	char cbHandCardData[2] = {cbCardData[0], cbCardData[1]};
	char cbHandCardCount = 2;
	char cbCenterCardData[5] = {cbCardData[2], cbCardData[3], cbCardData[4], cbCardData[5], cbCardData[6]};
	char cbCenterCardCount = 5;
	char cbLastCardData[5];
	char cbLastCardCount = 5;
	
	return GetGreatestFiveFromSeven(cbHandCardData, cbHandCardCount, cbCenterCardData, cbCenterCardCount, cbLastCardData, cbLastCardCount);	
}

/*
** Function: May form straight 
*/
bool MayFormStraight(char Data[], char Count)
{
	char i, j, k;
	char cbCardData[6];				// Count的值为5或6
	for (i = 0; i < Count; i++)
		cbCardData[i] = Data[i];
	
	char cbCardCount = Count;
	
	SortCardList(cbCardData, cbCardCount);				// Sort cards
	
	// 这里只考虑两头顺的情况（如4,5,6,7,10），单个顺的情况暂时先不考虑（如4,5,6,8,10），因为较复杂
	for (i = 0; i < cbCardCount - 3; i++)
	{
		char cbFirstValue = GetCardLogicValue(cbCardData[i]);
		k = 1;
		for (j = i + 1; j < i + 4; j++, k++)
		{
			if (cbFirstValue != (GetCardLogicValue(cbCardData[j]) + k))		
				break;
		}	
		if (j >= i + 4)
			return true;
	}
	return false;
}

/*
** Function: May form flush
*/
bool MayFormFlush(char Data[], char Count)
{
	char i, j ,k, l;
	char cbCardData[6];				// Count的值为5或6
	for (i = 0; i < Count; i++)
		cbCardData[i] = Data[i];
	
	for (i = 0; i < Count - 3; i++)
	{
		for (j = i + 1; j < Count - 2; j++)
		{
			for (k = j + 1; k < Count - 1; k++)
			{
				for (l = k + 1; l < Count; l++)
				{
					if (GetCardColor(cbCardData[i]) == GetCardColor(cbCardData[j]) &&
						GetCardColor(cbCardData[i]) == GetCardColor(cbCardData[k]) &&
						GetCardColor(cbCardData[i]) == GetCardColor(cbCardData[l]) &&
						GetCardColor(cbCardData[j]) == GetCardColor(cbCardData[k]) &&
						GetCardColor(cbCardData[j]) == GetCardColor(cbCardData[l]) &&
						GetCardColor(cbCardData[k]) == GetCardColor(cbCardData[l]) )
						return true;		// 表示其中的四张是同花
				}
			}
		}
	}
	return false;						// 表示没有四张能够组成同花
}

/*
** 	Function: Get card value.
*/
inline char GetCardValue(char cbCardData)							
{ 
	return cbCardData & LOGIC_MASK_VALUE; 
}

/*
** 	Function: Get card color.
*/
inline char GetCardColor(char cbCardData)
{
	return cbCardData & LOGIC_MASK_COLOR; 
}

/*
** Function: Numerical convert of the cards.
*/
char GetCardLogicValue(char cbCardData)
{
	// Get the color and value of a card
	char bCardColor = GetCardColor(cbCardData);
	char bCardValue = GetCardValue(cbCardData);
	
	// Convert the cardvalue
	return (bCardValue == 1) ? (bCardValue + 13) : bCardValue;
}

/*
** Function: Arrangement the cards by "A - > K...- > 2" from big to small,
** if the value is equal, according to the color: SPADES HEARTS CLUBS DIAMONDS.
*/
void SortCardList(char cbCardData[], char cbCardCount)
{
	// Numerical convert of the cards
	char cbLogicValue[FULL_COUNT];
	for (char i = 0; i < cbCardCount; i++) 
		cbLogicValue[i] = GetCardLogicValue(cbCardData[i]);	
	
	// Sort the cards
	bool bSorted = true;
	char cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (char i = 0; i < bLast; i++)
		{
			if ((cbLogicValue[i] < cbLogicValue[i+1])||
				((cbLogicValue[i] == cbLogicValue[i+1]) && (cbCardData[i] < cbCardData[i+1])))
			{
				// Swap position of the cards
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i+1];
				cbCardData[i+1] = cbTempData;
				cbTempData = cbLogicValue[i];
				cbLogicValue[i] = cbLogicValue[i+1];
				cbLogicValue[i+1] = cbTempData;
				bSorted = false;
			}	
		}
		bLast--;
	} while (bSorted == false);
	
	return;
}

/*
** Function: Arrangement the cards by color of SPADE HEART CLUB DIAMOND and then by "A - > K...- > 2" from big to small,
*/
void SortCardListByColor(char cbCardData[], char cbCardCount)
{
	// Numerical convert of the cards
	int spadeCount, heartCount, clubCount, diamondCount;
	int i, j, k;
	char tempCardData[10];

	spadeCount = heartCount = clubCount = diamondCount = 0;
	for (i = 0; i < cbCardCount; i++)
	{
		if ((cbCardData[i] & 0xF0) == 0x30)
			spadeCount++;
		else if ((cbCardData[i] & 0xF0) == 0x20)
			heartCount++;
		else if ((cbCardData[i] & 0xF0) == 0x10)
			clubCount++;
		else
			diamondCount++;
	}

	int spadeIndex   = 0;
	int heartIndex   = spadeIndex + spadeCount;
	int clubIndex    = heartIndex + heartCount;
	int diamondIndex = clubIndex  + clubCount;

	for (i = 0; i < cbCardCount; i++)
	{
		if ((cbCardData[i] & 0xF0) == 0x30)
		{
			tempCardData[spadeIndex++] = cbCardData[i];		
		}
		else if ((cbCardData[i] & 0xF0) == 0x20)
		{
			tempCardData[heartIndex++] = cbCardData[i];		
		}
		else if ((cbCardData[i] & 0xF0) == 0x10)
		{
			tempCardData[clubIndex++] = cbCardData[i];		
		}
		else
		{
			tempCardData[diamondIndex++] = cbCardData[i];		
		}
	}

	for (i = 0; i < cbCardCount; i++)
		cbCardData[i] = tempCardData[i];

	SortCardList(cbCardData, spadeCount);

	SortCardList(cbCardData + spadeCount, heartCount);

	SortCardList(cbCardData + spadeCount + heartCount, clubCount);

	SortCardList(cbCardData + spadeCount + heartCount + clubCount, diamondCount);

	return;
}

/*
** Function: Get the type of five cards.
*/
char GetSortedCardsType(char cbCardData[], char cbCardCount)
{
	// Check the cbCardCount
	assert(cbCardCount == 5);	
	
	bool cbSameColor = true, bLineCard = true;
	char cbFirstColor = GetCardColor(cbCardData[0]);
	char cbFirstValue = GetCardLogicValue(cbCardData[0]);
	
	// Analyser the cards
	for (char i = 1; i < cbCardCount; i++)
	{
		// Analyse the color and value
		if (GetCardColor(cbCardData[i]) != cbFirstColor)				
			cbSameColor = false;
		if (cbFirstValue != (GetCardLogicValue(cbCardData[i]) + i))		
			bLineCard = false;
		
		// Judge is over or not
		if ((cbSameColor == false) && (bLineCard == false))		break;
	}
	
	// the smallest STRAIGHT_FLUSH
	if ((bLineCard == false) && (cbFirstValue == 14))
	{
		char i = 1;
		for (i = 1; i < cbCardCount; i++)
		{
			char cbLogicValue = GetCardLogicValue(cbCardData[i]);
			if ((cbFirstValue != (cbLogicValue + i + 8)))	break;
		}
		if (i == cbCardCount)
			bLineCard = true;
	}
	
	// STRAIGHT
	if ((cbSameColor == false) && (bLineCard == true)) 
		return CT_SHUN_ZI;
	
	// FLUSH
	if ((cbSameColor == true) && (bLineCard == false)) 
		return CT_TONG_HUA;
	
	// STRAIGHT_FLUSH
	if ((cbSameColor == true) && (bLineCard == true))
		return CT_TONG_HUA_SHUN;
	
	// Analyse the cards
	tagAnalyseResult AnalyseResult;
	AnalysebCardData(cbCardData, cbCardCount, AnalyseResult);
	
	// Judge the tpye
	if (AnalyseResult.cbFourCount == 1) 
		return CT_FOUR_TIAO;			// FOUR_OF_A_KIND
	if (AnalyseResult.cbLONGCount == 2) 
		return CT_TWO_LONG;				// TWO_PAIR
	if ((AnalyseResult.cbLONGCount == 1) && (AnalyseResult.cbThreeCount == 1))
		return CT_HU_LU;				// FULL_HOUSE
	if ((AnalyseResult.cbThreeCount == 1) && (AnalyseResult.cbLONGCount == 0))
		return CT_THREE_TIAO;			// THREE_OF_A_KING// THREE_OF_A_KING
	if ((AnalyseResult.cbLONGCount == 1) && (AnalyseResult.cbSignedCount == 3)) 
		return CT_ONE_LONG;				// ONE_PAIR
	
	return CT_SINGLE;					// HIGH_CARD
}

/*
** Function: Analyse the cards.
*/
void AnalysebCardData(const char cbCardData[], char cbCardCount, tagAnalyseResult &AnalyseResult)
{
	// Fill variable AnalyseResult with 0
	memset(&AnalyseResult, 0, sizeof(AnalyseResult));	
	
	// Analyse the cards
	for (char i = 0; i < cbCardCount; i++)
	{
		char cbSameCount = 1;
		char cbSameCardData[4] = {cbCardData[i], 0, 0, 0};
		char cbLogicValue = GetCardLogicValue(cbCardData[i]);
		
		// Get the cards
		for (int j = i + 1; j < cbCardCount; j++)
		{
			// Numerical convert of the cards
			if (GetCardLogicValue(cbCardData[j]) != cbLogicValue)	break;
			
			// Set the card
			cbSameCardData[cbSameCount++] = cbCardData[j];
		}
		
		// Save the result
		switch (cbSameCount)
		{
		case 1:		// HIGH_CARD
			{
				AnalyseResult.cbSignedLogicVolue[AnalyseResult.cbSignedCount] = cbLogicValue;
				strncpy(&AnalyseResult.cbSignedCardData[(AnalyseResult.cbSignedCount++)*cbSameCount],cbSameCardData,cbSameCount);	
				break;
			}
		case 2:		// ONE_PAIR
			{
				AnalyseResult.cbLONGLogicVolue[AnalyseResult.cbLONGCount] = cbLogicValue;
				strncpy(&AnalyseResult.cbLONGCardData[(AnalyseResult.cbLONGCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 3:		// THREE_OF_A_KING
			{
				AnalyseResult.cbThreeLogicVolue[AnalyseResult.cbThreeCount] = cbLogicValue;
				strncpy(&AnalyseResult.cbThreeCardData[(AnalyseResult.cbThreeCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		case 4:		// FOUR_OF_A_KIND
			{
				AnalyseResult.cbFourLogicVolue[AnalyseResult.cbFourCount]=cbLogicValue;
				strncpy(&AnalyseResult.cbFourCardData[(AnalyseResult.cbFourCount++)*cbSameCount],cbSameCardData,cbSameCount);
				break;
			}
		}
		
		// Set the increament
		i += cbSameCount - 1;
	}
	
	return;
}

/*
** Function: Get greatest type of five cards from all the six cards.
*/
char GetGreatestFiveFromSix(char cbHandCardData[], char cbHandCardCount,
	char cbCenterCardData[], char cbCenterCardCount, char cbLastCardData[], char cbLastCardCount)
{
	char cbTempCardData[2+4], cbTempLastCardData[5];
	
	memset(cbTempCardData, 0, sizeof(cbTempCardData));
	memset(cbTempLastCardData, 0, sizeof(cbTempLastCardData));
	
	// Copy the cards
	strncpy(cbTempCardData, cbHandCardData, sizeof(char) * 2);		
	strncpy(&cbTempCardData[2], cbCenterCardData, sizeof(char) * 4);
	
	// Sort the cards
	SortCardList(cbTempCardData, sizeof(cbTempCardData) / sizeof(cbTempCardData[0]));
	
	// Copy the cards
	strncpy(cbLastCardData, cbTempCardData, sizeof(char) * 5);
	char cbCardKind = GetSortedCardsType(cbLastCardData, sizeof(char) * 5);
	char cbTempCardKind = 0;
	
	// Combination algorithm
	for (int i = 0; i < 2; i++)
	{
		for (int j = i + 1; j < 3; j++)
		{
			for (int k = j + 1; k < 4; k++)
			{
				for (int l = k + 1; l < 5; l++)
				{
					for (int m = l + 1; m < 6; m++)
					{
						// Get the card
						cbTempLastCardData[0]=cbTempCardData[i];
						cbTempLastCardData[1]=cbTempCardData[j];
						cbTempLastCardData[2]=cbTempCardData[k];
						cbTempLastCardData[3]=cbTempCardData[l];
						cbTempLastCardData[4]=cbTempCardData[m];
						
						// Get the type of five cards
						cbTempCardKind = GetSortedCardsType(cbTempLastCardData,sizeof(cbTempLastCardData) / sizeof(cbTempLastCardData[0]));
						
						// Compare types of different combination
						if (CompareCard(cbTempLastCardData,cbLastCardData, sizeof(cbTempLastCardData) / sizeof(cbTempLastCardData[0])) == 2)
						{
							cbCardKind = cbTempCardKind;
							strncpy(cbLastCardData,cbTempLastCardData,sizeof(char) * sizeof(cbTempLastCardData) / sizeof(cbTempLastCardData[0]));
						}
					}
				}
			}
		}
	}
	return cbCardKind;
}

/*
** Function: Get greatest type of five cards from all the seven cards.
*/
char GetGreatestFiveFromSeven(char cbHandCardData[], char cbHandCardCount,
	char cbCenterCardData[], char cbCenterCardCount, char cbLastCardData[], char cbLastCardCount)
{
	char cbTempCardData[2+5], cbTempLastCardData[5];

	memset(cbTempCardData, 0, sizeof(cbTempCardData));
	memset(cbTempLastCardData, 0, sizeof(cbTempLastCardData));
	
	// Copy the cards
	strncpy(cbTempCardData,cbHandCardData, sizeof(char) * 2);		
	strncpy(&cbTempCardData[2], cbCenterCardData, sizeof(char) * 5);
	
	// Sort the cards
	SortCardList(cbTempCardData,sizeof(cbTempCardData) / sizeof(cbTempCardData[0]));
	
	strncpy(cbLastCardData,cbTempCardData, sizeof(char) * 5);
	char cbCardKind = GetSortedCardsType(cbLastCardData, sizeof(char) * 5);
	char cbTempCardKind = 0;
	
	// Combination algorithm
	for (int i = 0; i < 3; i++)
	{
		for (int j = i + 1; j < 4; j++)
		{
			for (int k = j + 1; k < 5; k++)
			{
				for (int l = k + 1; l < 6; l++)
				{
					for (int m = l + 1; m < 7; m++)
					{
						// Get the cards
						cbTempLastCardData[0]=cbTempCardData[i];
						cbTempLastCardData[1]=cbTempCardData[j];
						cbTempLastCardData[2]=cbTempCardData[k];
						cbTempLastCardData[3]=cbTempCardData[l];
						cbTempLastCardData[4]=cbTempCardData[m];
						
						// Get the type of five cards
						cbTempCardKind = GetSortedCardsType(cbTempLastCardData,sizeof(cbTempLastCardData) / sizeof(cbTempLastCardData[0]));
						
						// Compare types of different combination
						if (CompareCard(cbTempLastCardData,cbLastCardData, sizeof(cbTempLastCardData) / sizeof(cbTempLastCardData[0])) == 2)
						{
							cbCardKind = cbTempCardKind;
							strncpy(cbLastCardData,cbTempLastCardData,sizeof(char) * sizeof(cbTempLastCardData) / sizeof(cbTempLastCardData[0]));
						}
					}
				}
			}
		}
	}
	return cbCardKind;
}

/*
** Function: Compare the type of different five cards.
*/
char CompareCard(char cbFirstData[], char cbNextData[], char cbCardCount)
{
	// Get the type of five cards
	char cbNextType  = GetSortedCardsType(cbNextData,cbCardCount);
	char cbFirstType = GetSortedCardsType(cbFirstData,cbCardCount);

	// Judge the type 
	if(cbFirstType > cbNextType)	// greater
		return 2;

	if(cbFirstType < cbNextType)	// less
		return 1;

	// Simple type
	switch(cbFirstType)
	{
	case CT_SINGLE:					// HIGH_CARD
		{
			char i = 0;
			for (i = 0; i < cbCardCount; i++)
			{
				char cbNextValue=GetCardLogicValue(cbNextData[i]);
				char cbFirstValue=GetCardLogicValue(cbFirstData[i]);
									// greater	
				if(cbFirstValue > cbNextValue)
					return 2;
									// less
				else if(cbFirstValue <cbNextValue)
					return 1;
				else				// equal
					continue;
			}
			if( i == cbCardCount)	// equal
				return 0;
			assert(false);
		}
	case CT_ONE_LONG:				// ONE_PAIR
	case CT_TWO_LONG:				// TWO_PAIR
	case CT_THREE_TIAO:				// THREE_OF_A_KING
	case CT_FOUR_TIAO:				// FOUR_OF_A_KIND
	case CT_HU_LU:					// FULL_HOUSE
		{
			// Analuse the cards
			tagAnalyseResult AnalyseResultNext;
			tagAnalyseResult AnalyseResultFirst;
			AnalysebCardData(cbNextData, cbCardCount, AnalyseResultNext);
			AnalysebCardData(cbFirstData, cbCardCount, AnalyseResultFirst);

			// FOUR_OF_A_KIND
			if (AnalyseResultFirst.cbFourCount > 0)
			{
				char cbNextValue=AnalyseResultNext.cbFourLogicVolue[0];
				char cbFirstValue=AnalyseResultFirst.cbFourLogicVolue[0];

				// Compare the FOUR_OF_A_KIND
				if(cbFirstValue != cbNextValue) return (cbFirstValue > cbNextValue) ? 2 : 1;

				// if FOUR_OF_A_KIND is equal, Compare the HIGH_CARD
				assert(AnalyseResultFirst.cbSignedCount == 1 && AnalyseResultNext.cbSignedCount == 1);
				cbFirstValue = AnalyseResultFirst.cbSignedLogicVolue[0];
				cbNextValue = AnalyseResultNext.cbSignedLogicVolue[0];
				if (cbFirstValue != cbNextValue) return (cbFirstValue > cbNextValue) ? 2 : 1;
				else return 0;
			}

			// THREE_OF_A_KING
			if (AnalyseResultFirst.cbThreeCount > 0)
			{
				char cbNextValue=AnalyseResultNext.cbThreeLogicVolue[0];
				char cbFirstValue=AnalyseResultFirst.cbThreeLogicVolue[0];

				// Compare the THREE_OF_A_KIND
				if(cbFirstValue != cbNextValue)return (cbFirstValue > cbNextValue)?2:1;

				// FULL_HOUSE
				if(CT_HU_LU == cbFirstType)
				{
					// Compre the pair
					assert(AnalyseResultFirst.cbLONGCount == 1 && AnalyseResultNext.cbLONGCount == 1);
					cbFirstValue = AnalyseResultFirst.cbLONGLogicVolue[0];
					cbNextValue = AnalyseResultNext.cbLONGLogicVolue[0];
					if(cbFirstValue != cbNextValue) return (cbFirstValue > cbNextValue) ? 2 : 1;
					else return 0;
				}
				else// THREE_OF_A_KING with a single 
				{
					// Compare the HIGH_CARD
					assert(AnalyseResultFirst.cbSignedCount == 2 && AnalyseResultNext.cbSignedCount == 2);

					// High card number
					char i = 0;
					for (i = 0; i < AnalyseResultFirst.cbSignedCount; i++)
					{
						char cbNextValue = AnalyseResultNext.cbSignedLogicVolue[i];
						char cbFirstValue = AnalyseResultFirst.cbSignedLogicVolue[i];

						if(cbFirstValue > cbNextValue)		// greater
							return 2;
						else if(cbFirstValue <cbNextValue)	// less
							return 1;
						else continue;						// equal
					}
					if (i == AnalyseResultFirst.cbSignedCount)
						return 0;
					assert(false);
				}
			}

			// PAIR
			char i = 0;
			for (i = 0; i < AnalyseResultFirst.cbLONGCount; i++)
			{
				char cbNextValue = AnalyseResultNext.cbLONGLogicVolue[i];
				char cbFirstValue = AnalyseResultFirst.cbLONGLogicVolue[i];
			
				if(cbFirstValue > cbNextValue)				// greater
					return 2;
				else if(cbFirstValue <cbNextValue)			// less
					return 1;
				else										// equal
					continue;
			}

			// Compare the High_Card
			assert (i == AnalyseResultFirst.cbLONGCount);
			{
				assert(AnalyseResultFirst.cbSignedCount == AnalyseResultNext.cbSignedCount
						&& AnalyseResultNext.cbSignedCount > 0);
				// High card number
				for (i = 0; i < AnalyseResultFirst.cbSignedCount; i++)
				{
					char cbNextValue = AnalyseResultNext.cbSignedLogicVolue[i];
					char cbFirstValue = AnalyseResultFirst.cbSignedLogicVolue[i];

					if(cbFirstValue > cbNextValue)			// greater
						return 2;
					else if(cbFirstValue < cbNextValue)		// less
						return 1;
					else continue;							// equal
				}
				if (i == AnalyseResultFirst.cbSignedCount)	// equal
					return 0;
			}
			break;
		}
	case CT_SHUN_ZI:		// STRAIGHT
	case CT_TONG_HUA_SHUN:	// STRAIGHT_FLUSH
		{
			char cbNextValue = GetCardLogicValue(cbNextData[0]);
			char cbFirstValue = GetCardLogicValue(cbFirstData[0]);

			bool bFirstmin = (cbFirstValue ==(GetCardLogicValue(cbFirstData[1])+9));
			bool bNextmin = (cbNextValue ==(GetCardLogicValue(cbNextData[1])+9));

			// big and small STRAIGHT
			if ((bFirstmin == true) && (bNextmin == false))
			{
				return 1;
			}
			// big and small STRAIGHT
			else if ((bFirstmin == false) && (bNextmin == true))
			{
				return 2;
			}
			// equal STRAIGHT
			else
			{
				if(cbFirstValue == cbNextValue)return 0;		
				return (cbFirstValue > cbNextValue) ? 2 : 1;			
			}
		}
	case CT_TONG_HUA:		// FLUSH
		{	
			// High card number
			char i;
			for (i = 0; i < cbCardCount; i++)
			{
				char cbNextValue = GetCardLogicValue(cbNextData[i]);
				char cbFirstValue = GetCardLogicValue(cbFirstData[i]);
				if (cbFirstValue == cbNextValue)	continue;
				return (cbFirstValue > cbNextValue) ? 2 : 1;
			}
			if (i == cbCardCount)	return 0;		// equal
		}		
	}
	return  0;
}


