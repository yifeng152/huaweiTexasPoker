#ifndef CARDTYPE_H
#define CARDTYPE_H

typedef char CardType;

// Type Of five cards
#define CT_SINGLE				1		// HIGH_CARD
#define CT_ONE_LONG				2		// ONE_PAIR
#define CT_TWO_LONG				3		// TWO_PAIR
#define CT_THREE_TIAO			4		// THREE_OF_A_KING
#define	CT_SHUN_ZI				5		// STRAIGHT
#define CT_TONG_HUA				6		// FLUSH
#define CT_HU_LU				7		// FULL_HOUSE
#define CT_FOUR_TIAO			8		// FOUR_OF_A_KIND
#define CT_TONG_HUA_SHUN		9		// STRAIGHT_FLUSH


// Get type of five cards
char GetFiveCardsType(char cbCardData[], char cbCardCount);		

// Get greatest type from six cards
char GetGreatestTypeFromSix(char cbCardData[],char cbCardCount);	

// Get greatest type from seven cards
char GetGreatestTypeFromSeven(char cbCardData[],char cbCardCount);	

// May make up to straight
bool MayFormStraight(char cbCardData[], char cbCardCount);

// May make up to flush
bool MayFormFlush(char cbCardData[], char cbCardCount);

// Sort Card
void SortCardList(char cbCardData[], char cbCardCount);				

// Sort Card by color
void SortCardListByColor(char cbCardData[], char cbCardCount);
#endif
