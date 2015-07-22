#include "game.h"
#include "turn_inquire.h"
#include <string.h>
#include <stdio.h>

#define betDiff_0			betStage1
#define betDiff_0_200		betStage2
#define betDiff_200_500		betStage3
#define betDiff_500_1000	betStage4
#define betDiff_GT1000		betStage5

void GetActionCommandWithTurn(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard);

static void GetTurnActionCommandCore(char *allCard, CardType type, char *actionCmd,int diff);
static int  GetValidBet(int firstPara,int secondPara,int thirdPara,int fourthPara,int fifthPara,int diff);
static void Turn_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd);

extern struct AllUserInfo allUserInfo;
extern struct AllUserBehavior allUserBehavior;
/*
** 获取转牌圈的响应指令
*/
void GetActionCommandWithTurn(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard)
{
	CardType type;
	type = GetGreatestTypeFromSix(allCard,6);
	
	SortCardList(allCard, 2);			// 将手牌从大到小排序 A->K->...->2
	SortCardList(allCard + 2, 4);		// 将四张公共牌大到小排序 A->K->...->2

	int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;

	GetTurnActionCommandCore(allCard, type,actionCmd,diff);
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
// int GetValidBet(int firstPara,int secondPara,int thirdPara,int fourthPara,int fifthPara,int diff)
// {
// 	int vaildPara;		// 在自己的上一轮的基础上加的钱
// 	
// 	//根据diff,选择合适的参数
// 	if(diff == 0)
// 	{
// 		vaildPara = firstPara;
// 	}
// 	else if (diff <= 200)
// 	{
// 		vaildPara = secondPara;
// 	}
// 	else if (diff <= 500)
// 	{
// 		vaildPara = thirdPara;
// 	}
// 	else if (diff <= 1000)
// 	{
// 		vaildPara = fourthPara;
// 	}
// 	else  if (diff > 1000)
// 	{
// 		vaildPara = fifthPara;
// 	}
// 	
// 	return vaildPara;
// }

/*
** 根据validPara和diff值来获取响应指令
*/
void Turn_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
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
// void Turn_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
// {
// 	//根据参数，确定指令
// 	if(validPara == Bet4AllIn)				// all_in
// 	{
// 		//strcpy(actionCmd, " all_in  \n");
// 		strcpy(actionCmd, "raise 2000\n");
// 
// 		allUserInfo.turnRaised = true;	// 置为true 表示该局在转牌圈已加注过
// 		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff+ 2000;
// 
// 	}
// 	else if(validPara == Bet4Fold)			// fold
// 	{
// 		
// 		strcpy(actionCmd, " fold  \n");
// 	}
// 	else if(validPara == Bet4Check)			// check
// 	{
// 		
// 		strcpy(actionCmd, " check  \n");
// 		allUserInfo.allUser[allUserInfo.ownIndex].bet += 0;	
// 	}
// 	else if(validPara == diff)				// call
// 	{
// 		strcpy(actionCmd, " call  \n");
// 		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;	
// 	}
// 	else if(validPara > diff)				// raise
// 	{
// 		if(!allUserInfo.turnRaised)			// 该局在转牌圈还没有加注过
// 		{
// 			allUserInfo.turnRaised = true;	// 置为true 表示该局在转牌圈已加注过
// 			int raiseNumber = validPara - diff;
// 			sprintf(actionCmd, "raise %d\n", raiseNumber);
// 			allUserInfo.allUser[allUserInfo.ownIndex].bet += validPara;
// 		}
// 		else
// 		{
// 			strcpy(actionCmd, " call  \n");
// 			allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;	
// 		}
// 	}
// 	else
// 	{
// 		//error
// 	}
// }

/*
** 获取转牌圈响应指令的核心函数
*/
void GetTurnActionCommandCore(char *allCard, CardType type, char *actionCmd,int diff)
{
	int betStage1,betStage2,betStage3,betStage4,betStage5;
	int betValid;

	switch (type)
	{
		case CT_TONG_HUA_SHUN:	// 同花顺
		{
			betDiff_0		 = Bet4AllIn;
			betDiff_0_200	 = Bet4AllIn;
			betDiff_200_500  = Bet4AllIn;
			betDiff_500_1000 = Bet4AllIn;
			betDiff_GT1000   = Bet4AllIn;
			break;	
		}
		case CT_FOUR_TIAO:		// 四条
		{
			// 四张公共牌组成四条
			if ( (allCard[2] & 0x0F) == (allCard[3] & 0x0F) &&
				 (allCard[2] & 0x0F) == (allCard[4] & 0x0F) &&
				 (allCard[2] & 0x0F) == (allCard[5] & 0x0F) )
			{
				if ((allCard[0] & 0x0F) == 0x01 ||		// 手中有A 或者四条为四个A手中有K
					((allCard[2] & 0x0F) == 0x01) && (allCard[0] & 0x0F) == 0x0D)	
				{
					betDiff_0		 = Bet4AllIn;
					betDiff_0_200	 = Bet4AllIn;
					betDiff_200_500  = Bet4AllIn;
					betDiff_500_1000 = Bet4AllIn;
					betDiff_GT1000   = Bet4AllIn;
				}
				else if ((allCard[0] & 0x0F) == 0x0D || (allCard[0] & 0x0F) == 0x0C)
				{
					betDiff_0		 = Bet4Check;
					betDiff_0_200	 = Bet4Check;
					betDiff_200_500  = Bet4Check;

					if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// 当手中的筹码已不足500时
					{
						betDiff_500_1000 = Bet4Check;
						betDiff_GT1000   = Bet4Check;
					}
					else
					{		
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;
					}
				}
				else
				{
					betDiff_0		 = Bet4Check;
					betDiff_0_200	 = Bet4Fold;
					betDiff_200_500  = Bet4Fold;
					betDiff_500_1000 = Bet4Fold;
					betDiff_GT1000   = Bet4Fold;
				}
			}
			else
			{
				betDiff_0		 = Bet4AllIn;
				betDiff_0_200	 = Bet4AllIn;
				betDiff_200_500  = Bet4AllIn;
				betDiff_500_1000 = Bet4AllIn;
				betDiff_GT1000   = Bet4AllIn;
			}
			break;
		}
		case CT_HU_LU:			// 葫芦
		{
			// 公牌中有两对
			if ((allCard[2] & 0x0F) == (allCard[3] & 0x0F) && (allCard[4] & 0x0F) == (allCard[5] & 0x0F))
			{
				if ( (allCard[2] & 0x0F) == (allCard[0] & 0x0F) ||
					 (allCard[2] & 0x0F) == (allCard[1] & 0x0F) )	// 手牌中的其中一张与公牌中的较大的对子组成葫芦中的三条 
				{
					betDiff_0		 = Bet4AllIn;
					betDiff_0_200	 = Bet4AllIn;
					betDiff_200_500  = Bet4AllIn;
					betDiff_500_1000 = Bet4AllIn;
					betDiff_GT1000   = Bet4AllIn;
					
				}
				else// 手牌中的其中一张与公牌中的较小对子组成葫芦中的三条 
				{
					if (allUserInfo.allUserCount <= RiskCount)
					{				
						betDiff_0		 = diff + 3 * BlindBet;
						betDiff_0_200	 = diff + 1 * BlindBet;
						betDiff_200_500  = diff + 1 * BlindBet;
						betDiff_500_1000 = diff;
						betDiff_GT1000   = diff;					
					}
					else 
					{
						betDiff_0		 = diff + 1 * BlindBet;
						betDiff_0_200	 = diff + 0 * BlindBet;
						betDiff_200_500  = diff + 0 * BlindBet;
						betDiff_500_1000 = diff;
						betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					}
				}
			}
			// 公牌中为一个三条加一个单张
			else if ( ((allCard[2] & 0x0F) == (allCard[3] & 0x0F) && 
					   (allCard[2] & 0x0F) == (allCard[4] & 0x0F))	||
					  ((allCard[3] & 0x0F) == (allCard[4] & 0x0F) && 
					   (allCard[3] & 0x0F) == (allCard[5] & 0x0F)) )
			{
				if (allUserInfo.allUserCount <= RiskCount)
				{							
					betDiff_0		 = diff + 2 * BlindBet;
					betDiff_0_200	 = diff + 1 * BlindBet;
					betDiff_200_500  = diff + 1 * BlindBet;
					betDiff_500_1000 = diff + 1 * BlindBet;
					betDiff_GT1000   = diff + 1 * BlindBet;
				}
				else 
				{
					betDiff_0		 = diff + 1 * BlindBet;
					betDiff_0_200	 = diff + 0 * BlindBet;
					betDiff_200_500  = diff + 0 * BlindBet;
					betDiff_500_1000 = diff + 0 * BlindBet;
					betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				}
			}
			else 
			{	// 其他的手牌和公牌形式组成葫芦
				if (allUserInfo.allUserCount <= RiskCount)
				{				
					betDiff_0		 = Bet4AllIn;
					betDiff_0_200	 = Bet4AllIn;
					betDiff_200_500  = Bet4AllIn;
					betDiff_500_1000 = Bet4AllIn;
					betDiff_GT1000   = Bet4AllIn;
				}
				else 
				{
					betDiff_0		 = diff + 3 * BlindBet;
					betDiff_0_200	 = diff + 2 * BlindBet;
					betDiff_200_500  = diff + 1 * BlindBet;
					betDiff_500_1000 = diff;
					betDiff_GT1000   = diff;
				}
			}
			break;	
		}
		case CT_TONG_HUA:		// 同花
		{
			// 四张公共牌的花色一样
			if ((allCard[2] & 0xF0) == (allCard[3] & 0xF0) &&
				(allCard[2] & 0xF0) == (allCard[4] & 0xF0) &&
				(allCard[2] & 0xF0) == (allCard[5] & 0xF0) )
			{
				// 这里的大小是没有影响的，只是不知道是哪张牌花色一致，故进行if-else判断

				// 手牌中较大的那张牌的花色与公牌中的四张花色一致
				if ((allCard[0] & 0xF0) == (allCard[2] & 0xF0))
				{
					// 手牌中较大的那张牌为A或K
					if ((allCard[0] & 0x0F) == 0x01 || (allCard[0] & 0x0F) == 0x0D)	
					{
						betDiff_0		 = Bet4AllIn;
						betDiff_0_200	 = Bet4AllIn;
						betDiff_200_500  = Bet4AllIn;
						betDiff_500_1000 = Bet4AllIn;

						if ((allCard[0] & 0x0F) == 0x01)
						{
							betDiff_GT1000   = Bet4AllIn;
						}
						else
						{
							betDiff_GT1000  = diff;
						}
					}
					else // 手牌中较大的那张牌不是A或K
					{	
						if (allUserInfo.allUserCount <= RiskCount)
						{						
							betDiff_0		 = Bet4Check;
							betDiff_0_200	 = diff;
							betDiff_200_500  = diff;
							betDiff_500_1000 = diff;
							betDiff_GT1000   = diff;
						}
						else 
						{
							betDiff_0		 = Bet4Check;
							betDiff_0_200	 = diff;
							betDiff_200_500  = diff;
							betDiff_500_1000 = diff;
							betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						}
					}

				}
				else	// 手牌中较小的那张牌的花色与公牌中的四张花色一致
				{					
					// 手牌中较小的那张牌为A或K
					if ((allCard[1] & 0x0F) == 0x01 || (allCard[1] & 0x0F) == 0x0D)	
					{
						betDiff_0		 = Bet4AllIn;
						betDiff_0_200	 = Bet4AllIn;
						betDiff_200_500  = Bet4AllIn;
						betDiff_500_1000 = Bet4AllIn;
						//betDiff_GT1000   = Bet4AllIn;
						if ((allCard[1] & 0x0F) == 0x01)
						{
							betDiff_GT1000   = Bet4AllIn;
						}
						else
						{
							betDiff_GT1000  = diff;
						}
					}
					else 
					{	// 手牌中较大的那张牌不是A或K
						if (allUserInfo.allUserCount <= RiskCount)
						{						
							betDiff_0		 = Bet4Check;
							betDiff_0_200	 = diff;
							betDiff_200_500  = diff;
							betDiff_500_1000 = diff;
							betDiff_GT1000   = diff;
						}
						else 
						{
							betDiff_0		 = Bet4Check;
							betDiff_0_200	 = diff;
							betDiff_200_500  = diff;
							betDiff_500_1000 = diff;
							betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						}
					}
				}
			}
			else	// 三张公共牌和两张手牌组成同花
			{		
				if (allUserInfo.allUserCount <= RiskCount)
				{				
					betDiff_0		 = Bet4AllIn;
					betDiff_0_200	 = Bet4AllIn;
					betDiff_200_500  = Bet4AllIn;
					betDiff_500_1000 = Bet4AllIn;
					betDiff_GT1000   = Bet4AllIn;
				}
				else 
				{
					betDiff_0		 = diff + 3 * BlindBet;
					betDiff_0_200	 = diff + 2 * BlindBet;
					betDiff_200_500  = diff + 1 * BlindBet;
					betDiff_500_1000 = diff;
					betDiff_GT1000   = diff;			
				}
			}
			break;
		}
		case CT_SHUN_ZI:		// 顺子 这里可能还不够详细  需要考虑不同的组合情况
		{
			if (allUserInfo.allUserCount <= RiskCount) 
			{
				betDiff_0		 = Bet4AllIn;
				betDiff_0_200	 = Bet4AllIn;
				betDiff_200_500  = Bet4AllIn;
				betDiff_500_1000 = Bet4AllIn;
				betDiff_GT1000   = Bet4AllIn;
			}
			else 
			{
				betDiff_0		 = diff + 3 * BlindBet;
				betDiff_0_200	 = diff + 2 * BlindBet;
				betDiff_200_500  = diff + 1 * BlindBet;
				betDiff_500_1000 = diff;
				betDiff_GT1000   = diff;	
			}
			break;	
		}
		case CT_THREE_TIAO:		// 三条
		{
			// 公牌就是三条
			if (((allCard[2] & 0x0F) == (allCard[3] & 0x0F))  && ((allCard[2] & 0x0F) == (allCard[4] & 0x0F)) ||
				((allCard[2] & 0x0F) == (allCard[3] & 0x0F))  && ((allCard[2] & 0x0F) == (allCard[5] & 0x0F)) ||
				((allCard[2] & 0x0F) == (allCard[4] & 0x0F))  && ((allCard[2] & 0x0F) == (allCard[5] & 0x0F)) ||
				((allCard[3] & 0x0F) == (allCard[4] & 0x0F))  && ((allCard[3] & 0x0F) == (allCard[5] & 0x0F)) )
			{		
				betDiff_0 = Bet4Check;
				//betDiff_0_200 = diff;
				betDiff_0_200 = Bet4Fold;
				betDiff_200_500  = Bet4Fold;
				betDiff_500_1000 = Bet4Fold;
				betDiff_GT1000   = Bet4Fold;
			}
			else
			{
				// 两张手牌和一张公牌组成三条
				if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
				{					
					bool isStraight = MayFormStraight(allCard+2, 4);	// 判断这五张牌中的4张是否有组成顺子的可能性
					bool isFlush = MayFormFlush(allCard+2,4);			// 判断这五张牌中的4张是否有组成同花的可能性

					if (isStraight || isFlush)	// 五张的的四张能组成顺子或同花
					{
						betDiff_0        = Bet4Check;
						betDiff_0_200    = diff;
						betDiff_200_500  = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_GT1000   = Bet4Fold;
					}
					else
					{
						if (allUserInfo.allUserCount <= RiskCount) 
						{						
							betDiff_0		 = Bet4AllIn;
							betDiff_0_200	 = Bet4AllIn;
							betDiff_200_500  = Bet4AllIn;
							betDiff_500_1000 = Bet4AllIn;
							betDiff_GT1000   = Bet4AllIn;
						}
						else 
						{
							betDiff_0		 = diff + 2 * BlindBet;
							betDiff_0_200	 = diff + 1 * BlindBet;
							betDiff_200_500  = diff + 0 * BlindBet;
							betDiff_500_1000 = diff;
							betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;	
						}
					}
				}					
				else 
				{	
					// 一张手牌和一对公牌组成三条
					if (allUserInfo.allUserCount <= RiskCount) 
					{
						betDiff_0		 = Bet4Check;
						betDiff_0_200	 = diff;
						betDiff_200_500  = diff;

						if (allUserInfo.validUserCount <= 3)
						{
							betDiff_500_1000 = diff;
							betDiff_GT1000   = diff;
						}
						else
						{
							betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
							betDiff_GT1000   = Bet4Fold;
						}						
					}
					else 
					{
						betDiff_0		 = Bet4Check;
						betDiff_0_200	 = diff;
						betDiff_200_500  = diff;

						if (allUserInfo.validUserCount <= 3)
						{
							betDiff_500_1000 = diff;						
						}
						else
						{
							betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
 						}
						betDiff_GT1000   = Bet4Fold;
					}
				}
			}
			break;
		}
		case CT_TWO_LONG:		// 两对
		{
			// 一对手牌和一对公牌组成两对
			if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
			{
				// 公牌中就有两对
				//if ( (allCard[2] & 0x0F) == (allCard[3] & 0x0F) || (allCard[4] & 0x0F) == (allCard[5] & 0x0F))
				if ( (allCard[2] & 0x0F) == (allCard[3] & 0x0F) && (allCard[4] & 0x0F) == (allCard[5] & 0x0F))  // linaijun 2015-05-27
				{
					if (allUserInfo.allUserCount <= RiskCount) 
					{
						betDiff_0		 = Bet4Check;
						betDiff_0_200	 = Bet4Check;
						betDiff_200_500  = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;	
					}
					else 
					{
						betDiff_0		 = Bet4Check;
						betDiff_0_200	 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_200_500  = Bet4Fold;
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;
					}
				}
				else
				{
					if (allUserInfo.allUserCount <= RiskCount) 
					{
						betDiff_0		 = 0 + 1 * BlindBet;
						betDiff_0_200	 = diff + 1 * BlindBet;
						betDiff_200_500  = diff + 1 * BlindBet;
						betDiff_500_1000 = diff;
						betDiff_GT1000   = diff;	
					}
					else 
					{
						betDiff_0		 = 0 + 1 * BlindBet;
						betDiff_0_200	 = diff + 0 * BlindBet;
						betDiff_200_500  = diff + 0 * BlindBet;
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;							
					}
				}
			}				
			else 
			{   				
				if ((allCard[2] & 0x0F) == (allCard[3] & 0x0F) && (allCard[4] & 0x0F) == (allCard[5] & 0x0F))
				{	
					// 公牌中的四张组成两对
					betDiff_0		 = Bet4Check;
					betDiff_0_200	 = diff;
					//betDiff_200_500  = diff;
					betDiff_200_500  = Bet4Fold;
					betDiff_500_1000 = Bet4Fold;
					betDiff_GT1000 = Bet4Fold;
					// 当人数至少还有4个（并且余下的筹码至少还有300时）弃牌 因为其中的某一个很有可能已经组成葫芦
// 					if (allUserInfo.validUserCount >= 4 &&  
// 						allUserInfo.allUser[allUserInfo.ownIndex].jetton >= 300)	
// 					{
// 						betDiff_500_1000 = Bet4Fold;
// 						betDiff_GT1000 = Bet4Fold;
// 					}
// 					else 
// 					{	
// 						betDiff_500_1000 = diff;
// 						betDiff_GT1000 = diff;
// 					}
				}
				else 
				{
					 // 手牌的两个单张和公牌的两个单张组成两对 或者公牌的一对和公牌的一个单张和手牌的一个单张组成一对
					if (allUserInfo.allUserCount <= RiskCount) 
					{					
						betDiff_0		 = 0 + 3 * BlindBet;
						betDiff_0_200	 = diff + 2 * BlindBet;
						betDiff_200_500  = diff + 2 * BlindBet;
						betDiff_500_1000 = diff;
						betDiff_GT1000   = diff;	
					}
					else 
					{
						betDiff_0		 = 0 + 1 * BlindBet;
						betDiff_0_200	 = diff + 0 * BlindBet;
						betDiff_200_500  = diff + 0 * BlindBet;
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					}
				}			
			}	
			break;
		}
		case CT_ONE_LONG:		// 一对
		{
			// 只有手牌是一对
			if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
			{
				betDiff_0 = Bet4Check;

				// 如果组成对子的那张牌小于公牌中的第二大的牌 则弃牌
				if ((allCard[0] & 0x0F) < (allCard[3] & 0x0F))
				{
					betDiff_0_200	 = diff;
					betDiff_200_500  = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					betDiff_500_1000 = Bet4Fold;
					betDiff_GT1000   = Bet4Fold;	
				}
				else
				{
					betDiff_0_200	 = diff;
					betDiff_200_500  = diff;
					betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					betDiff_GT1000   = Bet4Fold;
				}
			}
			else 
			{  					
				// 只有公牌中有一对					
				if ( ((allCard[2] & 0x0F) == (allCard[3] & 0x0F)) ||
					 ((allCard[2] & 0x0F) == (allCard[4] & 0x0F)) ||
					 ((allCard[2] & 0x0F) == (allCard[5] & 0x0F)) ||
					 ((allCard[3] & 0x0F) == (allCard[4] & 0x0F)) ||
					 ((allCard[3] & 0x0F) == (allCard[5] & 0x0F)) ||
					 ((allCard[4] & 0x0F) == (allCard[5] & 0x0F)) )
				{
					if (allUserInfo.allUserCount <= RiskCount) 
					{
						betDiff_0	  = Bet4Check;
						betDiff_0_200 = diff;

						bool isStraight = MayFormStraight(allCard, 6);	// 判断这6张牌中的4张是否有组成顺子的可能性
						bool isFlush = MayFormFlush(allCard, 6);		// 判断这6张牌中的4张是否有组成同花的可能性

						if (isStraight || isFlush)		// 若还能组成同花或者是顺子
						{
							betDiff_200_500 = diff;
						}
						else
						{
							betDiff_200_500 = Bet4Fold;
						}
						
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;
					}
					else 
					{
						betDiff_0	  = Bet4Check;
						
						bool isStraight = MayFormStraight(allCard, 6);	// 判断这6张牌中的4张是否有组成顺子的可能性
						bool isFlush = MayFormFlush(allCard, 6);		// 判断这6张牌中的4张是否有组成同花的可能性
						
						if (isStraight || isFlush)		// 若还能组成同花或者是顺子
						{
							betDiff_0_200 = diff;
						}
						else
						{
							betDiff_0_200 = Bet4Fold;
						}
						
						betDiff_200_500  = Bet4Fold;
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;
					}
				}
				else 
				{  
					// 公牌中的最大一张和手牌组成一个对子
					if ((allCard[0] & 0x0F) == (allCard[2] & 0x0F) ||
						(allCard[1] & 0x0F) == (allCard[2] & 0x0F))
					{
						if (allUserInfo.allUserCount <= RiskCount) 
						{
							betDiff_0		 = Bet4Check;
							betDiff_0_200	 = diff;
							betDiff_200_500  = diff;
							betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
							betDiff_GT1000   = Bet4Fold;
						}
						else 
						{
							betDiff_0		 = Bet4Check;
							betDiff_0_200	 = diff;
							betDiff_200_500  = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
							betDiff_500_1000 = Bet4Fold;
							betDiff_GT1000   = Bet4Fold;
						}
					}
					else
					{
						betDiff_0		 = Bet4Check;
						betDiff_0_200	 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_200_500  = Bet4Fold;
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;
					}
				}
			}					
			break;
		}
		case CT_SINGLE:	// 高牌
		{
			betDiff_0 = Bet4Check;
			
			bool isStraight = MayFormStraight(allCard, 6);	// 判断这6张牌中的4张是否有组成顺子的可能性
			bool isFlush = MayFormFlush(allCard, 6);		// 判断这6张牌中的4张是否有组成同花的可能性

			if (isStraight || isFlush)		// 若还能组成同花或者是顺子
			{
				betDiff_0_200	= diff;
				betDiff_200_500 = Bet4Fold;
			}
			else
			{
				betDiff_0_200   = Bet4Fold;
				betDiff_200_500 = Bet4Fold;
			}
			
			betDiff_500_1000 = Bet4Fold;
			betDiff_GT1000   = Bet4Fold;
		}
	}
																// 根据diff值选取某个输入作为下一轮的下注额
	betValid = GetValidBet(betDiff_0, betDiff_0_200, betDiff_200_500, betDiff_500_1000, betDiff_GT1000, diff);

	Turn_LatestBet_ActionCmd(betValid,diff,actionCmd);			// 根据validPara和diff值来获取响应指令		
}
