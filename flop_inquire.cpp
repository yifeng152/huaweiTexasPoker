#include "game.h"
#include "flop_inquire.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
  
//betStage1: diff=0 
//betStage2: diff<= 200
//betStage3: diff<=500 
//betStage4: diff <= 1000 
//betStage5: diff > 1000
#define betDiff_0			betStage1
#define betDiff_0_200		betStage2
#define betDiff_200_500		betStage3
#define betDiff_500_1000	betStage4
#define betDiff_GT1000		betStage5


void GetActionCommandWithFlop(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard);

static void GetFlopActionCommandCore(char *allCard, CardType type, char *actionCmd,int diff);
static int  GetValidBet(int firstPara,int secondPara,int thirdPara,int fourthPara,int fifthPara,int diff);
static void Flop_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd);

extern struct AllUserInfo allUserInfo;
extern struct AllUserBehavior allUserBehavior;

/*
** 获取公牌圈的响应指令
*/
void GetActionCommandWithFlop(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard)
{
	CardType type;
	type = GetFiveCardsType(allCard, 5);

	SortCardList(allCard, 2);			// 将手牌从大到小排序 A->K->...->2
	SortCardList(allCard + 2, 3);		// 将三张公共牌大到小排序 A->K->...->2

	int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;

	GetFlopActionCommandCore(allCard, type, actionCmd, diff);
}

/*
** 根据diff值选取某个输入作为下一轮的下注额
*/
int GetValidBet(int firstPara,int secondPara,int thirdPara,int fourthPara,int fifthPara,int diff)
{
	int vaildPara;		// 在自己的上一轮的基础上加的钱
	
	//根据diff,选择合适的参数
	if(diff == 0)
	{
		vaildPara = firstPara;
	}
	//else if (diff <= 200)
	else if (diff <= 2 * BlindBet)
	{
		vaildPara = secondPara;
	}
	//else if (diff <= 500)
	else if (diff <= 5 * BlindBet)
	{
		vaildPara = thirdPara;
	}
	//else if (diff <= 1000)
	else if (diff <= 10 * BlindBet)
	{
		vaildPara = fourthPara;
	}
	//else  if (diff > 1000)
	else if (diff > 10 * BlindBet)
	{
		vaildPara = fifthPara;
	}

	return vaildPara;
}

/*
** 根据validPara和diff值来获取响应指令
*/
void Flop_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
{
	//根据参数，确定指令
	if(validPara == Bet4AllIn)				// all_in
	{
		int raiseNumber = 40 * BlindBet;
		sprintf(actionCmd, "raise %d\n", raiseNumber);
		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;

		//strcpy(actionCmd, " all_in  \n");
		//strcpy(actionCmd, "raise 2000\n");
		
		allUserInfo.flopRaised = true;	// 置为true 表示该局在转牌圈已加注过
		//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff+ 2000;
		
	}
// 	if(validPara == Bet4AllIn)				// all_in
// 	{
// 		strcpy(actionCmd, " all_in  \n");
// 	}
	else if(validPara == Bet4Fold)			// fold
	{
		strcpy(actionCmd, " fold  \n");
		allUserInfo.allUser[allUserInfo.ownIndex].isValid = false;
	}	
	else if(validPara == Bet4Survive)			// fold
	{
		strcpy(actionCmd, " call  \n");
		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;	
	}
	else if(validPara == Bet4Check)			// check
	{
		strcpy(actionCmd, " check  \n");
		allUserInfo.allUser[allUserInfo.ownIndex].bet += 0;	
	}
	else if(validPara == diff)				// call
	{
		strcpy(actionCmd, " call  \n");
		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;	
	}
	else if(validPara > diff)				// raise
	{
		if(!allUserInfo.flopRaised)			// 该局在公牌圈还没有加注过
		{
			allUserInfo.flopRaised = true;	// 置为true 表示该局在公牌圈已加注过
			//int raiseNumber = validPara - diff;

			int raiseNumber = validPara - diff + 1 + BlindBet;
			//sprintf(actionCmd, "raise %d\n", raiseNumber);
			//allUserInfo.allUser[allUserInfo.ownIndex].bet += validPara;
			if (raiseNumber >= diff)
			{
				sprintf(actionCmd, "raise %d\n", raiseNumber);
				allUserInfo.allUser[allUserInfo.ownIndex].bet += validPara;
			}
			else
			{
				strcpy(actionCmd, " call  \n");
				allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;	
			}
		}
		else
		{
			strcpy(actionCmd, " call  \n");
			allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;	
		}
	}
	else
	{
		//error
	}
}

/*
** 获取公牌圈响应指令的核心函数
*/
void GetFlopActionCommandCore(char *allCard, CardType type, char *actionCmd,int diff)
{
	// -2: all_in -1: fold positive: 加注或注 0: 过
	// betStage1: diff=0 betStage2: diff<= 200 betStage3: diff<=500 betStage4: diff <= 1000 betStage5:diff > 1000
	int betStage1, betStage2, betStage3, betStage4, betStage5;
	int betValid;

	switch (type)
	{
		case CT_TONG_HUA_SHUN:	// 同花顺
		case CT_FOUR_TIAO:		// 四条
		case CT_HU_LU:			// 葫芦
		case CT_TONG_HUA:		// 同花
		case CT_SHUN_ZI:		// 顺子
		{
			betDiff_0 = betDiff_0_200 = betDiff_200_500 = betDiff_500_1000 = betDiff_GT1000 = Bet4AllIn; 			
			break;	
		}
		case CT_THREE_TIAO:		// 三条
		{
			if ((allCard[2] & 0x0F) == (allCard[4] & 0x0F))		// 公牌就是三条
			{	
				betDiff_0 = Bet4Check;
				betDiff_0_200 = diff;

				// 公牌就是三条  手里有一个A 或者至少有一个牌大于等于Q 跟注
				if (((allCard[0] & 0x0F == 0x01) || (allCard[1] & 0x0F == 0x01)) ||	
					((allCard[0] & 0x0F >= 0x0C) || (allCard[1] & 0x0F >= 0x0C)))	
				{
					betDiff_200_500  = diff;
					//betDiff_500_1000 = diff;
					//betDiff_GT1000   = diff;
					//betDiff_500_1000 = Bet4Fold;
					betDiff_500_1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					betDiff_GT1000   = Bet4Fold;
				}
				else	// 弃牌
				{
					betDiff_200_500  = Bet4Fold;
					betDiff_500_1000 = Bet4Fold;
					betDiff_GT1000   = Bet4Fold;
				}
			}
			else
			{
				// 两张手牌和一张公牌组成三条
				if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
				{
					betDiff_0 = Bet4AllIn;
				
					// 当前已下注额大于1000 并且下注额已大于手中剩下的筹码 跟注
					if ((allUserInfo.allUser[allUserInfo.ownIndex].bet >= 1000) 
						&& (allUserInfo.allUser[allUserInfo.ownIndex].bet > allUserInfo.allUser[allUserInfo.ownIndex].jetton))
					{
						betDiff_0_200 = diff;
					}
					else
					{
						//betDiff_0_200 = diff + 2 * BlindBet;

						betDiff_0_200 = diff + 1 * BlindBet;
					}
					
					if (allUserInfo.allUserCount <= RiskCount)
					{
						betDiff_200_500  = diff + 1 * BlindBet;
						betDiff_500_1000 = diff + 1 * BlindBet;
						betDiff_GT1000   = diff + 1 * BlindBet;
					}
					else 
					{
						betDiff_200_500  = diff + 0 * BlindBet;					
						betDiff_500_1000 = diff + 0 * BlindBet;					
						betDiff_GT1000   = diff + 0 * BlindBet;
					}
				}					
				else // 一张手牌和一对公牌组成三条
				{	
					if (allUserInfo.allUserCount <= RiskCount)
					{
						betDiff_0        = 0 + 2 * BlindBet;
						betDiff_0_200    = diff;
						betDiff_200_500  = diff;
						betDiff_500_1000 = diff;
						//betDiff_GT1000   = Bet4Fold;
						betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					}
					else
					{
						betDiff_0        = 0 + 0 * BlindBet;
						betDiff_0_200    = diff;
						betDiff_200_500  = diff;
// 						betDiff_500_1000 = Bet4Fold;					
// 						betDiff_GT1000   = Bet4Fold;
						
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;					
						betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					}
				}
			}
			break;
		}
		case CT_TWO_LONG:		// 两对
		{			
			if ((allCard[0] & 0x0F) == (allCard[1] & 0x0F))	// 一对手牌和一对公牌组成两对
			{
				if (allUserInfo.allUserCount <= RiskCount)
				{				
					betDiff_0        = Bet4Check;
					betDiff_0_200    = diff;
					betDiff_200_500  = diff;
					betDiff_500_1000 = diff;
//					betDiff_GT1000   = Bet4Fold;
					betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				}
				else
				{		
					betDiff_0        = Bet4Check;
					betDiff_0_200    = diff;
					betDiff_200_500  = diff;
				//	betDiff_500_1000 = Bet4Fold;				
				//	betDiff_GT1000   = Bet4Fold;

					betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				}
			}				
			else // 手牌的两个单张和公牌的两个单张组成两对 或者公牌的一对和公牌的一个单张和手牌的一个单张组成一对
			{   
				if (allUserInfo.allUserCount <= RiskCount)
				{		
					betDiff_0        = 0 + 3 * BlindBet;
					betDiff_0_200    = diff + 1 * BlindBet;
					betDiff_200_500  = diff;
					betDiff_500_1000 = diff;
					//betDiff_GT1000   = Bet4Fold;
					betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				}
				else 
				{
					betDiff_0        = 0 + 1 * BlindBet;
					betDiff_0_200    = diff + 0 * BlindBet;
					betDiff_200_500  = diff + 0 * BlindBet;
// 					betDiff_500_1000 = Bet4Fold;
// 					betDiff_GT1000   = Bet4Fold;

					betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				}
			}	
			break;
		}
		case CT_ONE_LONG:		// 一对
		{
			// 只有手牌是一对
			if ((allCard[0] & 0x0F) == (allCard[1] & 0x0F))
			{
				bool holdGTFlop;	// 手牌比公牌中最大牌还大
				if ((allCard[0] & 0x0F) == 0x01)	// 手牌是AA,必然比公牌中最大牌还大，可以考虑加注
				{
					holdGTFlop = true;
				}
				else if ((allCard[2] & 0x0F) == 0x01)	// 公牌中有AA
				{
					holdGTFlop = false;
				}
				else if ((allCard[0] & 0x0F) > (allCard[2] & 0x0F))
				{
					holdGTFlop = true;
				}
				else
				{
					holdGTFlop = false;
				}

				if (holdGTFlop == true)	// 手牌比公牌中最大牌还大，可以考虑加注
				{
					if (allUserInfo.allUserCount <= RiskCount)
					{					
						betDiff_0        = 0 + 2 * BlindBet;
						betDiff_0_200    = diff + 2 * BlindBet;
						betDiff_200_500  = diff + 2 * BlindBet;
						betDiff_500_1000 = diff;
						//betDiff_GT1000   = Bet4Fold;
						betDiff_GT1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					}
					else 
					{
						betDiff_0        = 0 + 0 * BlindBet;
						betDiff_0_200    = diff + 0 * BlindBet;
						betDiff_200_500  = diff + 0 * BlindBet;
						//betDiff_500_1000 = Bet4Fold;
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						//betDiff_GT1000   = Bet4Fold;
						betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					}
				}
				else
				{
					betDiff_0        = Bet4Check;
					betDiff_0_200    = diff;
					betDiff_200_500  = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					betDiff_GT1000   = Bet4Fold;				
				}
			}
			else 
			{  					
				// 只有公牌中有一对					
				if ( ((allCard[2] & 0x0F) == (allCard[3] & 0x0F)) ||
					 ((allCard[3] & 0x0F) == (allCard[4] & 0x0F)) ||
					 ((allCard[2] & 0x0F) == (allCard[4] & 0x0F)) )
				{
					betDiff_0 = Bet4Check;

					bool isStraight = MayFormStraight(allCard, 5);	// 判断这五张牌中的4张是否有组成顺子的可能性
								
					bool isFlush = MayFormFlush(allCard, 5);		// 判断这五张牌中的4张是否有组成同花的可能性
					
					if (isStraight || isFlush)	// 五张的的四张能组成顺子或同花
					{
						betDiff_0_200    = diff;
						betDiff_200_500  = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_GT1000   = Bet4Fold;
					}
					else
					{
						betDiff_0_200    = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_200_500  = Bet4Fold;
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;
					}
				}
				else 
				{  	// 一张手牌和一张公牌组成一对

					// 公牌中的最大牌恰好与手牌组成一对
					if (((allCard[2] & 0x0F) == (allCard[1] & 0x0F) ) || ((allCard[2] & 0x0F) == (allCard[0] & 0x0F)))
					{
						if (allUserInfo.allUserCount <= RiskCount)
						{						
							betDiff_0        = 0 + 2 * BlindBet;
							betDiff_0_200    = diff + 2 * BlindBet;
							betDiff_200_500  = diff;
							betDiff_500_1000 = diff;
							betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						}
						else 
						{
							betDiff_0        = 0 + 0 * BlindBet;
							betDiff_0_200    = diff + 0 * BlindBet;
							betDiff_200_500  = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
							betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
							betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						}
					}
					else
					{
						if (allUserInfo.allUserCount <= RiskCount)
						{
							betDiff_0        = Bet4Check;
							betDiff_0_200    = diff;
							betDiff_200_500  = diff;
							betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
							betDiff_GT1000   = Bet4Fold;	
						}
						else 
						{
							betDiff_0        = Bet4Check;
							betDiff_0_200    = diff;
							betDiff_200_500  = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
							betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
							betDiff_GT1000   = Bet4Fold;
						}
					}
				}
			}					
			break;
		}
		case CT_SINGLE:	// 高牌
		{
			betDiff_0 = Bet4Check;

			bool isStraight = MayFormStraight(allCard, 5);	// 判断这五张牌中的4张是否有组成顺子的可能性
								
			bool isFlush = MayFormFlush(allCard, 5);		// 判断这五张牌中的4张是否有组成同花的可能性

			if (isStraight || isFlush)		// 五张的的四张能组成顺子或同花
			{

				if (allUserInfo.allUserCount <= RiskCount)
				{		
					betDiff_0_200    = diff;
					betDiff_200_500  = diff;
					betDiff_500_1000 = Bet4Fold;
					betDiff_GT1000   = Bet4Fold;
				}
				else {
					betDiff_0_200    = diff;
					betDiff_200_500  = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					betDiff_500_1000 = Bet4Fold;
					betDiff_GT1000   = Bet4Fold;
				}
			}
			else
			{
				betDiff_0_200    = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				betDiff_200_500  = Bet4Fold;
				betDiff_500_1000 = Bet4Fold;
				betDiff_GT1000   = Bet4Fold;
			}
			break;
		}
	}

																// 根据diff值选取某个输入作为下一轮的下注额
	betValid = GetValidBet(betDiff_0, betDiff_0_200, betDiff_200_500, betDiff_500_1000, betDiff_GT1000, diff);
																
	Flop_LatestBet_ActionCmd(betValid,diff,actionCmd);			// 根据validPara和diff值来获取响应指令	
}
