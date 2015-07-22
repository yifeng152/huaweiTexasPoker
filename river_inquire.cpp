#include "game.h"
#include "river_inquire.h"
#include <string.h>
#include <stdio.h>

#define betDiff_0			betStage1
#define betDiff_0_200		betStage2
#define betDiff_200_500		betStage3
#define betDiff_500_1000	betStage4
#define betDiff_GT1000		betStage5

static void GetRiverActionCommandCore(char *allCard, CardType type, char *actionCmd,int diff);
static int GetValidBet(int firstPara,int secondPara,int thirdPara,int fourthPara,int fifthPara,int diff);
static void River_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd);

void GetActionCommandWithRiver(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard);

extern struct AllUserInfo allUserInfo;
extern struct AllUserBehavior allUserBehavior;

/*
** 获取河牌圈的响应指令
*/
void GetActionCommandWithRiver(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard)
{
	CardType type;
	type = GetGreatestTypeFromSeven(allCard, 7);
	
	SortCardList(allCard, 2);			// 将手牌从大到小排序 A->K->...->2
	SortCardList(allCard + 2, 5);		// 将四张公共牌大到小排序 A->K->...->2

	int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;

	GetRiverActionCommandCore(allCard, type,actionCmd,diff);
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
void River_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
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

// void River_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
// {
// 	//根据参数，确定指令
// 	if(validPara == Bet4AllIn)				// all_in
// 	{
// 		//strcpy(actionCmd, " all_in  \n");
// 		strcpy(actionCmd, "raise 2000\n");
// 		
// 		allUserInfo.riverRaised = true;	// 置为true 表示该局在转牌圈已加注过
// 		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff+ 2000;
// 		
// 	}
// // 	if(validPara == Bet4AllIn)			//all_in
// // 	{
// // 		strcpy(actionCmd, " all_in  \n");
// // 	}
// 	else if(validPara == Bet4Fold)	//fold
// 	{		
// 		strcpy(actionCmd, " fold  \n");
// 	}
// 	else if(validPara == Bet4Check)		// check
// 	{
// 		
// 		strcpy(actionCmd, " check  \n");
// 		allUserInfo.allUser[allUserInfo.ownIndex].bet += 0;	
// 	}
// 	else if(validPara == diff)	// call
// 	{
// 		strcpy(actionCmd, " call  \n");
// 		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;	
// 	}
// 	else if(validPara > diff)	// raise
// 	{
// 		if(!allUserInfo.riverRaised)		 // 该局在转牌圈还没有加注过
// 		{
// 			allUserInfo.riverRaised = true;	// 置为true 表示该局在转牌圈已加注过
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
** 获取河牌圈响应指令的核心函数
*/
void GetRiverActionCommandCore(char *allCard, CardType type, char *actionCmd,int diff)
{
	int betStage1,betStage2,betStage3,betStage4,betStage5;
	int betValid;

	switch (type)
	{
		case CT_TONG_HUA_SHUN:	// 同花顺
		{
			CardType typeOfCommonCard;
			typeOfCommonCard = GetFiveCardsType(allCard + 2, 5);

			if (typeOfCommonCard == CT_TONG_HUA_SHUN)	// 公牌中的5张组成同花顺
			{	
				betDiff_0		 = diff;
				betDiff_0_200	 = diff;
				betDiff_200_500  = diff;
				betDiff_500_1000 = diff;
				betDiff_GT1000   = diff;
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
					betDiff_0		 = diff + 4 * BlindBet;
					betDiff_0_200	 = diff + 3 * BlindBet;
					betDiff_200_500  = diff + 2 * BlindBet;
					betDiff_500_1000 = diff + 1 * BlindBet;
					betDiff_GT1000   = diff;
				}
			}
			break;	
		}
		case CT_FOUR_TIAO:		// 四条
		{
			// 五张公共牌中的四张组成四条
			//if ((allCard[2] & 0x0F) == (allCard[5] & 0x0F) || (allCard[3] & 0x0F) == (allCard[6] & 0x0F))
			if ((allCard[2] & 0x0F) == (allCard[5] & 0x0F))		// 公牌中的前四张组成四条
			{
				if ((allCard[2] & 0x0F) == 0x01)	// 公牌中的前四张为四条A
				{
					if ((allCard[0] & 0x0F) == 0x0D || (allCard[6] & 0x0F) == 0x0D)
					{
						betDiff_0		 = Bet4AllIn;
						betDiff_0_200	 = Bet4AllIn;
						betDiff_200_500  = Bet4AllIn;
						betDiff_500_1000 = Bet4AllIn;
						betDiff_GT1000   = Bet4AllIn;
					}
					else 
					{
						betDiff_0		 = Bet4Check;
						betDiff_0_200	 = Bet4Check;
						betDiff_200_500	 = Bet4Check;

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
				}
				else		// 公牌中的前四张不是四条A
				{
					if ((allCard[0] & 0x0F) == 0x01 || (allCard[6] & 0x0F) == 0x01)
					{
						betDiff_0		 = Bet4AllIn;
						betDiff_0_200	 = Bet4AllIn;
						betDiff_200_500  = Bet4AllIn;
						betDiff_500_1000 = Bet4AllIn;
						betDiff_GT1000   = Bet4AllIn;
					}
					else 
					{
						betDiff_0		 = Bet4Check;
						betDiff_0_200	 = Bet4Check;
						betDiff_200_500	 = Bet4Check;
						
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
				}
			}
			else if ((allCard[3] & 0x0F) == (allCard[6] & 0x0F))		// 公牌中的后四张组成四条
			{
				if ((allCard[0] & 0x0F) == 0x01 || (allCard[2] & 0x0F) == 0x01)
				{
					betDiff_0		 = Bet4AllIn;
					betDiff_0_200	 = Bet4AllIn;
					betDiff_200_500  = Bet4AllIn;
					betDiff_500_1000 = Bet4AllIn;
					betDiff_GT1000   = Bet4AllIn;
				}
				else 
				{
					betDiff_0		 = Bet4Check;
					betDiff_0_200	 = Bet4Check;
					betDiff_200_500	 = Bet4Check;
					
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
			// 5张公牌组成葫芦，其他人可能也有四条的情况 
			// 公牌中的五张就是葫芦 并且对子在后两张 
			if ((allCard[2] & 0x0F) == (allCard[4] & 0x0F) && (allCard[5] & 0x0F) == (allCard[6] & 0x0F))
			{
				// 手牌也是对子 并且手牌为对A或手牌的对子大于公牌中的对子
				if ((allCard[0] & 0x0F) == (allCard[1] & 0x0F) && 
					((allCard[0] & 0x0F) == 0x01 || (allCard[0] & 0x0F) > (allCard[5] & 0x0F)))
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
						betDiff_0		 = diff + 5 * BlindBet;
						betDiff_0_200	 = diff + 3 * BlindBet;
						betDiff_200_500  = diff + 1 * BlindBet;
						betDiff_500_1000 = diff;
						betDiff_GT1000   = diff;
					}
				}
				else
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
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					}
				}
			}			
			// 公牌中的五张就是葫芦 并且对子在前两张 
			else if ((allCard[2] & 0x0F) == (allCard[3] & 0x0F) && (allCard[4] & 0x0F) == (allCard[6] & 0x0F))
			{
				// 手牌也是对子 并且手牌为对A或手牌的对子大于公牌中的对子
				if ((allCard[0] & 0x0F) == (allCard[1] & 0x0F) && 
					((allCard[0] & 0x0F) == 0x01 || (((allCard[2] & 0x0F) != 0x01) && (allCard[0] & 0x0F) > (allCard[2] & 0x0F))))
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
						betDiff_0		 = diff + 5 * BlindBet;
						betDiff_0_200	 = diff + 3 * BlindBet;
						betDiff_200_500  = diff + 1 * BlindBet;
						betDiff_500_1000 = diff;
						betDiff_GT1000   = diff;
					}
				}
				else
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
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					}
				}
			}
			else	// 公牌中为两对或三条或其他形式 这里还需要再具体一点
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
					betDiff_0		 = diff + 5 * BlindBet;
					betDiff_0_200	 = diff + 3 * BlindBet;
					betDiff_200_500  = diff + 1 * BlindBet;
					betDiff_500_1000 = diff;
					betDiff_GT1000   = diff;
				}
			}
			break;
		}
		case CT_TONG_HUA:		// 同花  还需要再细化
		{
			CardType typeOfCommonCard;
			typeOfCommonCard = GetFiveCardsType(allCard + 2, 5);

			char commonCard[5] = {allCard[2], allCard[3], allCard[4], allCard[5], allCard[6]};
			SortCardListByColor(commonCard, 5);			

			if (typeOfCommonCard == CT_TONG_HUA)	// 公牌中的5张组成同花顺
			{	
				int maxGoalCard = 0x01 | (commonCard[0] & 0xF0);	//the first part is A, the second part is the color
				
				for (int i = 0; i < 5; i++)
				{
					if (commonCard[i] != maxGoalCard)
					{
						break;
					}
					else
					{
						maxGoalCard = (maxGoalCard & 0x0F == 0x01) ? (0x0D | (commonCard[0] & 0xF0)) : (maxGoalCard - 0x01);
					}
				}
				
				if ((allCard[0] == maxGoalCard) || (allCard[1] == maxGoalCard))	//the hold card contains the maxCard
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
						betDiff_0		 = diff + 5 * BlindBet;
						betDiff_0_200	 = diff + 3 * BlindBet;
						betDiff_200_500  = diff + 1 * BlindBet;
						betDiff_500_1000 = diff;
						betDiff_GT1000   = diff;
					}
				}
				else
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
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_GT1000   = Bet4Fold;
					}
				}
			}
			// 五张公牌中的其中四张花色一致
			else if (((commonCard[0] & 0xF0) == (commonCard[3] & 0xF0))	|| ((commonCard[0] & 0xF0) == (commonCard[3] & 0xF0)))
			{
				int commomColorStart;

				if((commonCard[0] & 0xF0) == (commonCard[3] & 0xF0))
				{
					commomColorStart = 0;
				}
				else
					commomColorStart = 1;

				int maxGoalCard = 0x01 | (commonCard[commomColorStart] & 0xF0);	//the first part is A, the second part is the color
				
				for (int i = commomColorStart; i < commomColorStart + 4; i++)
				{
					if (commonCard[i] != maxGoalCard)
					{
						break;
					}
					else
					{
						maxGoalCard = (maxGoalCard & 0x0F == 0x01) ? (0x0D | (commonCard[commomColorStart] & 0xF0)) : (maxGoalCard - 0x01);
					}
				}
				
				if ((allCard[0] == maxGoalCard) || (allCard[1] == maxGoalCard))	//the hold card contains the maxCard
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
						betDiff_0		 = diff + 5 * BlindBet;
						betDiff_0_200	 = diff + 3 * BlindBet;
						betDiff_200_500  = diff + 1 * BlindBet;
						betDiff_500_1000 = diff;
						betDiff_GT1000   = diff;
					}
				}
				else
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
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_GT1000   = Bet4Fold;
					}
				}
			}
			else		// 同花中的三张与手牌中的两张组成同花顺 这里是肯定的 无需再判断
			{
				int commomColorStart;
				
				if((commonCard[0] & 0xF0) == (commonCard[2] & 0xF0))
				{
					commomColorStart = 0;
				}
				else if((commonCard[1] & 0xF0) == (commonCard[3] & 0xF0))
				{
					commomColorStart = 1;
				}
				else
				{
					commomColorStart = 2;
				}
		
				int maxGoalCard = 0x01 | (commonCard[commomColorStart] & 0xF0);	//the first part is A, the second part is the color

				for (int i = commomColorStart; i < commomColorStart + 3; i++)
				{
					if (commonCard[i] != maxGoalCard)
					{
						break;
					}
					else
					{
						maxGoalCard = (maxGoalCard & 0x0F == 0x01) ? (0x0D | (commonCard[commomColorStart] & 0xF0)) : (maxGoalCard - 0x01);
					}
				}

				if ((allCard[0] == maxGoalCard) || (allCard[1] == maxGoalCard))	//the hold card contains the maxCard
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
						betDiff_0		 = diff + 5 * BlindBet;
						betDiff_0_200	 = diff + 3 * BlindBet;
						betDiff_200_500  = diff + 1 * BlindBet;
						betDiff_500_1000 = diff;
						betDiff_GT1000   = diff;
					}
				}
				else
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
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;		// 3 + 2这种情况下再弃牌是不是太可惜了啊？
						betDiff_GT1000   = Bet4Fold;
					}
				}
			}
			break;	
		}
		case CT_SHUN_ZI:		// 顺子 这里可能还不够详细  需要考虑不同的组合情况
		{
			CardType typeOfCommonCard;
			typeOfCommonCard = GetFiveCardsType(allCard + 2, 5);
			
			if (typeOfCommonCard == CT_SHUN_ZI)	// 五张公牌本身就是顺子
			{
				bool isA = (allCard[2] & 0x0F) == 0x01;	// 顺子中的最大牌是A

				bool isFlush = MayFormFlush(allCard + 2, 5);		// 判断5张公牌中的4张是否有组成同花的可能性

				bool isUpAdjacent = (allCard[0] & 0x0F) == ((allCard[2] & 0x0F) + 0x01) // 跟顺子的最大牌相邻
					|| (allCard[1] & 0x0F) == ((allCard[2] & 0x0F) + 0x01);
				
				if (!isA && !isFlush && isUpAdjacent)
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
						betDiff_0		 = diff + 5 * BlindBet;
						betDiff_0_200	 = diff + 3 * BlindBet;
						betDiff_200_500  = diff + 1 * BlindBet;
						betDiff_500_1000 = diff;
						betDiff_GT1000   = diff;
					}
				}
				else
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
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;		
						betDiff_GT1000   = Bet4Fold;
					}
				}
			}
			else	
			{
				bool isStraight = MayFormStraight(allCard + 2, 5);	// 判断这5张公牌中的4张是否有组成顺子的可能性
				bool isFlush    = MayFormFlush(allCard + 2, 5);		// 判断这5张公牌中的4张是否有组成同花的可能性
				
				bool isSmallStraight = true;
				int i;

				if (isStraight)
				{
					int numberOfDiffEquals1 = 0;
					for (i = 2; i < 7; i++)//2-6
					{
						int j = i;
						numberOfDiffEquals1 = 0;
						while (j < 7 - 1)//
						{
							if( ((allCard[j] & 0x0F) - (allCard[j+1] &0x0F) == 1) || (( (allCard[j] & 0x0F) == 0x01) && ( (allCard[j+1] & 0x0F) == 0x0D)) )
							{
								numberOfDiffEquals1++;
							}
							else if ((allCard[j] & 0x0F) == (allCard[j+1] & 0x0F))
							{
								j = j;		// 不处理
							}
							else
								break;
							
							if (numberOfDiffEquals1 == 2)
							{
								break;
							}
							j++;
						}
						if (numberOfDiffEquals1 == 2)
						{
							break;
						}						
					}
					int maxStraightNumber = allCard[i];

					if ((maxStraightNumber & 0x0F) == 0x01)
					{
						isSmallStraight = false;
					}
					else if ((allCard[0] & 0x0F) == ((maxStraightNumber & 0x0F) + 1) ||
						(allCard[1] & 0x0F) == ((maxStraightNumber & 0x0F) + 1))
					{
						isSmallStraight = false;
					}
				}

				// 五张公牌中的四张既不是同花并且跟手牌组成的顺子是大顺子（也就是组成两头顺左端的顺子 从大到小）
				if (!(isStraight && isSmallStraight) && !isFlush)	
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
						betDiff_0		 = diff + 5 * BlindBet;
						betDiff_0_200	 = diff + 3 * BlindBet;
						betDiff_200_500  = diff + 1 * BlindBet;
						betDiff_500_1000 = diff;
						betDiff_GT1000   = diff;
					}
				}
				else
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
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;		
						betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
					}
				}
			}
			break;	
		}
		case CT_THREE_TIAO:		// 三条
		{
			// 公牌就是三条
			if (((allCard[2] & 0x0F) == (allCard[4] & 0x0F)) ||
				((allCard[3] & 0x0F) == (allCard[5] & 0x0F)) ||
				((allCard[4] & 0x0F) == (allCard[6] & 0x0F)))
			{		
				betDiff_0        = Bet4Check;
				//betDiff_0_200    = diff;
				betDiff_0_200    = Bet4Fold;
				betDiff_200_500  = Bet4Fold;
				betDiff_500_1000 = Bet4Fold;
				betDiff_GT1000   = Bet4Fold;
			}
			else
			{
				// 手牌对子和一张公牌组成三条
				if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
				{					
					CardType typeOfCommonCard;
					typeOfCommonCard = GetFiveCardsType(allCard + 2, 5);

					bool isStraight = MayFormStraight(allCard + 2, 5);	// 判断这五张公牌中的四张是否有组成顺子的可能性
					bool isFlush    = MayFormFlush(allCard + 2, 5);		// 判断这五张公牌中的四张是否有组成同花的可能性

					// 五张公牌中的的四张不组成顺子或同花且类型为高牌
					if (!isStraight && !isFlush && typeOfCommonCard == CT_SINGLE)	
					{
						betDiff_0		 = Bet4AllIn;
						betDiff_0_200	 = Bet4AllIn;
						betDiff_200_500  = Bet4AllIn;
						betDiff_500_1000 = Bet4AllIn;
						betDiff_GT1000   = Bet4AllIn;
					}
					// 五张公牌中的的四张不组成顺子或同花且类型为对子并且手牌中的对子大于公牌中的对子
					else if (!isStraight && !isFlush && ((typeOfCommonCard == CT_ONE_LONG)))	
					{
						int pairIndex;
						for (int i = 2; i < 5; i++)
							if ((allCard[i] & 0x0F) == (allCard[i+1] & 0x0F))
							{
								pairIndex = i;
								break;
							}
						
						if ( ((allCard[0] & 0x0F) == 0x01) ||
							((allCard[pairIndex] & 0x0F) != 0x01) && ((allCard[0] & 0x0F) > (allCard[pairIndex] & 0x0F)) )
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
								betDiff_0		 = diff + 5 * BlindBet;
								betDiff_0_200	 = diff + 3 * BlindBet;
								betDiff_200_500  = diff + 1 * BlindBet;
								betDiff_500_1000 = diff;
								betDiff_GT1000   = diff;
							}
						}
						else
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
								betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;		
								betDiff_GT1000   = Bet4Fold;
							}
						}
					}
					else
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
							betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;		
							betDiff_GT1000   = Bet4Fold;
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
			CardType typeOfCommonCard;
			typeOfCommonCard = GetFiveCardsType(allCard + 2, 5);
			
			bool isStraight = MayFormStraight(allCard + 2, 5);	// 判断这五张公牌中的四张是否有组成顺子的可能性
			bool isFlush    = MayFormFlush(allCard + 2, 5);		// 判断这五张公牌中的四张是否有组成同花的可能性

			// 五张公牌中的四张既不能组成同花也不同组成顺子 并且当前有效人数<=3
			if (! ((isStraight || isFlush) && allUserInfo.validUserCount > 3))
			{
				// 一对手牌和一对公牌组成两对
				if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
				{
					// 公牌中就有两对
					//if ( (allCard[2] & 0x0F) == (allCard[3] & 0x0F) || (allCard[4] & 0x0F) == (allCard[5] & 0x0F))
					if ( (allCard[2] & 0x0F) == (allCard[3] & 0x0F) && (allCard[4] & 0x0F) == (allCard[5] & 0x0F))
					{
						betDiff_0		 = Bet4Check;
						betDiff_0_200	 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_200_500  = Bet4Fold;
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;	
					}
					else
					{
						betDiff_0		 = Bet4Check;
						betDiff_0_200	 = Bet4Check;
						betDiff_200_500  = Bet4Check;
						//betDiff_500_1000 = Bet4Check;
						//betDiff_GT1000   = Bet4Check;		

						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_GT1000   = Bet4Fold;	
					}
				}				
				else 
				{   	
					// 公牌中的四张组成两对
					if ((allCard[2] & 0x0F) == (allCard[3] & 0x0F) && (allCard[4] & 0x0F) == (allCard[5] & 0x0F))
					{
						betDiff_0 = Bet4Check;
						
						// 手牌手是否有A或K
						if ((allCard[0] & 0x0F) == 0x01 || (allCard[1] & 0x0F) == 0x0D )
						{
							betDiff_0_200 = Bet4Check;
						}
						else
						{
							betDiff_0_200 = Bet4Fold;
						}
						betDiff_200_500  = Bet4Fold;
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;
					}
					else 
					{
						// 手牌的两个单张和公牌的两个单张组成两对 或者公牌的一对和公牌的一个单张和手牌的一个单张组成一对
						betDiff_0		 = Bet4Check;
						betDiff_0_200	 = Bet4Check;
						betDiff_200_500  = Bet4Check;
						//betDiff_500_1000 = Bet4Check;
						//betDiff_GT1000   = Bet4Check;	

						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
						betDiff_GT1000   = Bet4Fold;	
					}			
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
			break;
		}
		case CT_ONE_LONG:		// 一对
		{
			bool isStraight = MayFormStraight(allCard + 2, 5);	// 判断这五张公牌中的四张是否有组成顺子的可能性
			bool isFlush    = MayFormFlush(allCard + 2, 5);		// 判断这五张公牌中的四张是否有组成同花的可能性
			
			// 五张公牌中的四张既不能组成同花也不同组成顺子 并且当前有效人数<=3
			if (! ((isStraight || isFlush) && allUserInfo.validUserCount > 3))
			{	
				// 只有手牌是一对
				if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
				{
					betDiff_0 = Bet4Check;

					// 如果组成对子的那张牌小于公牌中的第二大的牌 则弃牌
					if ((allCard[0] & 0x0F) < (allCard[3] & 0x0F))
					{
						betDiff_0_200	 = diff;
						betDiff_200_500  = Bet4Fold;
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;	

					}
					else
					{
						betDiff_0_200	 = diff;
						//betDiff_200_500  = diff;
						betDiff_200_500  = Bet4Fold;
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;
					}
				}
				else 
				{  					
					// 只有公牌中有一对					
					if ( ((allCard[2] & 0x0F) == (allCard[3] & 0x0F)) ||
						 ((allCard[3] & 0x0F) == (allCard[4] & 0x0F)) ||
						 ((allCard[4] & 0x0F) == (allCard[5] & 0x0F)) ||
						 ((allCard[5] & 0x0F) == (allCard[6] & 0x0F)) )
					{
						betDiff_0		 = Bet4Check;
						betDiff_0_200	 = Bet4Fold;
						betDiff_200_500	 = Bet4Fold;
						betDiff_500_1000 = Bet4Fold;
						betDiff_GT1000   = Bet4Fold;
					}
					else 
					{  
						//公牌中的最大一张和手牌组成一个对子
						if ((allCard[0] & 0x0F) == (allCard[2] & 0x0F) ||
							(allCard[1] & 0x0F) == (allCard[2] & 0x0F) )
						{
							betDiff_0		 = Bet4Check;
							betDiff_0_200	 = diff;
							//betDiff_200_500  = diff;
							betDiff_200_500  = Bet4Fold;
							betDiff_500_1000 = Bet4Fold;
							betDiff_GT1000   = Bet4Fold;
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
			break;
		}
		case CT_SINGLE:	// 高牌
		{
			betDiff_0		 = Bet4Check;
			if (allUserInfo.validUserCount <= 2)
			{
				betDiff_0_200 = Bet4Check;
			}
			betDiff_0_200	 = Bet4Fold;
			betDiff_200_500  = Bet4Fold;
			betDiff_500_1000 = Bet4Fold;
			betDiff_GT1000	 = Bet4Fold;
		}
	}
																// 根据diff值选取某个输入作为下一轮的下注额
	betValid = GetValidBet(betDiff_0, betDiff_0_200, betDiff_200_500, betDiff_500_1000, betDiff_GT1000, diff);
	
	River_LatestBet_ActionCmd(betValid,diff,actionCmd);			// 根据validPara和diff值来获取响应指令	
}
