#include "game.h"
#include "hold_inquire.h"
#include <string.h>
#include <stdio.h>

#define betDiff_0			betStage1
#define betDiff_0_200		betStage2
#define betDiff_200_500		betStage3
#define betDiff_500_1000	betStage4
#define betDiff_GT1000		betStage5

void GetActionCommandWithHold(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard);

static void GetHoldActionCommandCore(char *allCard, HoldType type, char *actionCmd,int diff, int latestUserBet);
static int  GetValidBet(int firstPara,int secondPara,int thirdPara,int fourthPara,int fifthPara,int diff);
static void Hold_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd);

extern struct AllUserInfo allUserInfo;
extern struct AllUserBehavior allUserBehavior;

bool isScareProper = true;	// 是否在手牌圈满足一定的条件下进行唬人
bool scareEnabled;			//只在hold圈
bool scareFailed;
int scareTryNum;			//进行唬人的总次数
int scareFailedNum;			//唬人成功的总次数

extern FILE *fp4ScarePlan;

/*
** 获取手牌圈的响应指令
*/
void GetActionCommandWithHold(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard)
{
	HoldType holdtype;

	//在执行该函数之后，已经排序
	holdtype = GetHoldCardType(allCard, 2);

	switch(status4Inqu)		// status4Inqu: 当前用户的押注状态标志位
	{		
		case Blind:	//前面无人加注		
		{
			//达到某一条件后，对唬人的成功率进行判断,如果失败率高于1/3，则将其关闭
			if( (allUserInfo.allUser[allUserInfo.ownIndex].money + allUserInfo.allUser[allUserInfo.ownIndex].jetton) <= 3000 )
			//if(allUserInfo.currentRound >= 100)
			{
				if (scareFailedNum * 3 >= scareTryNum)
				{
					isScareProper = false;
				}
			}
			
			//如果前面所有人均弃牌，后面人数小于2，则在牌小的时候开启唬人模式
			//如果是大盲，处于该状态，就说明已经赢了，只要不弃牌即可。
			//如果是小盲，处于该状态，后面只有一个大盲
			//如果不是盲注，处于该状态，后面有大盲，小盲，可以看做一个人
			//因而将 开局人数 减去 fold的人数 减去一个盲注 减去一个自己  小于等于2即可
			if ( ( (allUserInfo.allUserCount - allUserInfo.foldUserCount - 1 - 1) <= 2 )
				&& ( (holdtype == MiddleCardKQJT) || (holdtype == HighCardKQ) || (holdtype == LowCard7) || (holdtype == Other) )
				&& isScareProper )
			{
				scareTryNum += 1;
				scareEnabled = true;
				char str4ScarePlan[20];
				sprintf(str4ScarePlan, "Round%d ScareEnabled", allUserInfo.currentRound);
				fprintf(fp4ScarePlan, "\n%s", str4ScarePlan);

				int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;
				int raiseNumber;

				raiseNumber = 2 * BlindBet - 1;
				sprintf(actionCmd, "raise %d\n", raiseNumber);
				allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
				allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过	
			}
			else
			{
				int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;
				
				if (holdtype == HighPair)			// 两张手牌是大对子
				{
					//strcpy(actionCmd, "raise 400\n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + 400;
					
					//int raiseNumber = 1 * BlindBet;
					//sprintf(actionCmd, "raise %d\n", raiseNumber);
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
					//allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过					
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 4 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
					}
					else
					{
						int raiseNumber = 1 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
					}
				}
				else if (holdtype == LowPair)		// 两张手牌是小对子
				{
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 2 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
					}
					else
					{
						strcpy(actionCmd, "check \n");
						allUserInfo.allUser[allUserInfo.ownIndex].bet = BlindBet;
					}
					//strcpy(actionCmd, "check \n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet = BlindBet;
										
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + 200;
					//int raiseNumber = 2 * BlindBet;
					//sprintf(actionCmd, "raise %d\n", raiseNumber);
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
					//allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
				}
				else if (holdtype == HighCardA)		// 两张手牌中有一个A
				{
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 1 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
					}
					else
					{
						strcpy(actionCmd, " call \n");
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
					}
					//strcpy(actionCmd, " call \n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				//else if (holdtype == SameColor)		// 两张手牌花色一致
				//{
				//	strcpy(actionCmd, " call \n");
				//	allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;//100
				//}
				else if (holdtype == MiddleCardKQJT)	// 两张手牌至少大于等于10
				{
					strcpy(actionCmd, " call \n");
					allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;//100
				}
				else if (diff == 0)//为解决大盲也弃牌的情况 wq 2015-05-30 10-48
				{
					strcpy(actionCmd, " check \n");
					allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				// 对应自己是小盲注 并且最新的押注为盲注大小 并且当前有效用户数<=3
				else if (allUserInfo.allUser[allUserInfo.ownIndex].bet == BlindBet / 2 &&
					latestUserBet == BlindBet && allUserInfo.validUserCount <= 3)	
				{
					strcpy(actionCmd, " check \n");
					allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				else 
				{
					strcpy(actionCmd, " fold \n");
					allUserInfo.allUser[allUserInfo.ownIndex].isValid = false;
				}
			}			
			break;
		}
		case Allout:		// 所有人均已经弃牌的情况，这里系统可能不会再通知我们了
		{
			strcpy(actionCmd, " check \n");
			break;
		}
		case Handle:
		{
			int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;
			
			//enum HoldType { HighPair, LowPair, HighCardA, SameColor, Other };
			// HighPair: Pair 9 ~ Pair A  LowPair: Pair 2 ~ Pair 8 HighCardA: a A SameColor: 
			// 当前最大下注额为BlindBet
			if (latestUserBet == BlindBet)	//前面无人加注
			{
				int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;
				
				if (holdtype == HighPair)			// 两张手牌是大对子
				{
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 4 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
					}
					else
					{
						int raiseNumber = 1 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
					}
					//strcpy(actionCmd, "raise 400\n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + 400;

					//int raiseNumber = 1 * BlindBet;
					//sprintf(actionCmd, "raise %d\n", raiseNumber);
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
					//allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
				}
				else if (holdtype == LowPair)		// 两张手牌是小对子
				{
					//strcpy(actionCmd, "raise 200\n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + 200;
					
					//int raiseNumber = 2 * BlindBet;
					//sprintf(actionCmd, "raise %d\n", raiseNumber);
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
					//allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
	
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 2 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
					}
					else
					{
						strcpy(actionCmd, "check \n");
						allUserInfo.allUser[allUserInfo.ownIndex].bet = BlindBet;
					}
					//strcpy(actionCmd, "call \n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				else if (holdtype == HighCardA)		// 两张手牌中有一个A
				{
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 1 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
					}
					else
					{
						strcpy(actionCmd, " call \n");
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
					}
					//strcpy(actionCmd, "call\n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				//else if (holdtype == SameColor)		// 两张手牌花色一致
				//{
				//	strcpy(actionCmd, " check \n");
				//	//allUserInfo.allUser[allUserInfo.ownIndex].bet = 100;
				//	allUserInfo.allUser[allUserInfo.ownIndex].bet = BlindBet;
				//}
				else if (holdtype == MiddleCardKQJT)	// 两张手牌都大于等于10
				{
					strcpy(actionCmd, " check \n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet = 100;
					allUserInfo.allUser[allUserInfo.ownIndex].bet = BlindBet;
				}
				else if (diff == 0)	// 对应自己是大盲注的情况
				{
					strcpy(actionCmd, " check \n");
					allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				// 对应自己是小盲注 并且当前有效用户数<=3
				else if (allUserInfo.allUser[allUserInfo.ownIndex].bet == BlindBet / 2 &&
					allUserInfo.validUserCount <= 3)	
				{
					strcpy(actionCmd, " check \n");
					allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				else
				{
					strcpy(actionCmd, " fold \n");
					allUserInfo.allUser[allUserInfo.ownIndex].isValid = false;
				}
			}
			else
			{
				GetHoldActionCommandCore(allCard, holdtype, actionCmd,diff, latestUserBet);
			}						
			break;
		}
		default:
		{
		 	// any other choice still exist?
			strcpy(actionCmd, " check \n");
			break;
		}
	}
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

// static int GetValidBet(int firstPara,int secondPara,int thirdPara,int fourthPara,int fifthPara,int diff)
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
void Hold_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
{
	//根据参数，确定指令
	if(validPara == Bet4AllIn)				// all_in
	{
		int raiseNumber = 40 * BlindBet;
		sprintf(actionCmd, "raise %d\n", raiseNumber);
		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
		
		//strcpy(actionCmd, " all_in  \n");
		//strcpy(actionCmd, "raise 2000\n");
		
		allUserInfo.holdRaised = true;	// 置为true 表示该局在转牌圈已加注过
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
		if(!allUserInfo.holdRaised)			// 该局在公牌圈还没有加注过
		{
			allUserInfo.holdRaised = true;	// 置为true 表示该局在公牌圈已加注过
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

// static void Hold_LatestBet_ActionCmd(int validPara, int diff, char *actionCmd)
// {
// 	//根据参数，确定指令
// 	if(validPara == Bet4AllIn)				// all_in
// 	{
// 		//strcpy(actionCmd, " all_in  \n");
// 		strcpy(actionCmd, "raise 2000\n");
// 		
// 		allUserInfo.holdRaised = true;	// 置为true 表示该局在转牌圈已加注过
// 		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff+ 2000;
// 		
// 	}
// // 	if (validPara == Bet4AllIn)				// all_in
// // 	{
// // 		strcpy(actionCmd, " all_in  \n");
// // 	}
// 	else if (validPara == Bet4Fold)			// fold
// 	{		
// 		strcpy(actionCmd, " fold  \n");
// 	}
// 	else if (validPara == Bet4Check)		// check
// 	{		
// 		strcpy(actionCmd, " check  \n");
// 		allUserInfo.allUser[allUserInfo.ownIndex].bet += 0;	
// 	}
// 	else if (validPara == diff)				// call
// 	{
// 		strcpy(actionCmd, " call  \n");
// 		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;	
// 	}
// 	else if (validPara > diff)				// raise
// 	{
// 		if (!allUserInfo.holdRaised)		// 该局在手牌圈还没有加注过
// 		{
// 			allUserInfo.holdRaised = true;	// 置为true 表示该局在手牌圈已加注过
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
** 获取手牌圈响应指令的核心函数
*/
//void GetHoldActionCommandCore(char *allCard, HoldType type, char *actionCmd,int diff)
void GetHoldActionCommandCore(char *allCard, HoldType type, char *actionCmd,int diff, int latestUserBet)
{
	int betStage1, betStage2, betStage3, betStage4, betStage5;
	int betValid;

	switch(type)
	{
	case HighPair:
		//betDiff_0		 = 0 + 500; 
		//betDiff_0_200	 = diff + 400;
		//betDiff_200_500	 = diff + 200;

		if (allUserInfo.allUserCount <= RiskCount)
		{
			betDiff_0        = 5 * BlindBet;
			betDiff_0_200    = diff + 4 * BlindBet;
			betDiff_200_500  = diff + 2 * BlindBet;
			betDiff_500_1000 = diff;
			betDiff_GT1000	 = diff;
		}
		else
		{
			betDiff_0        = 1 * BlindBet;
			betDiff_0_200    = diff + 0 * BlindBet;
			betDiff_200_500  = diff + 0 * BlindBet;
			betDiff_500_1000 = diff;
			betDiff_GT1000	 = diff;
		}
		break;
	case LowPair:
		//betDiff_0        = 0 + 300; 
		//betDiff_0_200    = diff + 200;
		//betDiff_200_500  = diff + 200;

		if (allUserInfo.allUserCount <= RiskCount)
		{
			betDiff_0        = 0 + 3 * BlindBet; 
			betDiff_0_200    = diff + 2 * BlindBet;
			betDiff_200_500  = diff;
			betDiff_500_1000 = diff;
			betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
		}
		else 
		{
			if (latestUserBet <= 5 * BlindBet)
			{
				betDiff_0        = 0 + 0 * BlindBet; 
				betDiff_0_200    = diff + 0 * BlindBet;
				betDiff_200_500  = diff;
				//betDiff_500_1000 = Bet4Fold;
				//betDiff_GT1000   = Bet4Fold;

				betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				betDiff_GT1000   = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
			}
			else
			{
				betDiff_0        = 0 + 0 * BlindBet; 
// 				betDiff_0_200    = Bet4Fold;
// 				betDiff_200_500  = Bet4Fold;
// 				betDiff_500_1000 = Bet4Fold;
// 				betDiff_GT1000   = Bet4Fold;

				betDiff_0_200    = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				betDiff_200_500  = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				betDiff_GT1000   = Bet4Fold;
			}
		}
		break;
	case HighCardA:
		//betDiff_0		 = 0 + 100; 
// 		betDiff_0		 = 0 + 1 * BlindBet; 
// 		betDiff_0_200	 = diff;
// 		betDiff_200_500	 = Bet4Fold;
// 		betDiff_500_1000 = Bet4Fold;
// 		betDiff_GT1000	 = Bet4Fold;

		if (allUserInfo.allUserCount <= RiskCount)
		{
			betDiff_0		 = 0 + 1 * BlindBet; 
			betDiff_0_200	 = diff;
			//betDiff_200_500	 = Bet4Fold;
			//betDiff_500_1000 = Bet4Fold;
			//betDiff_GT1000	 = Bet4Fold;
			betDiff_200_500	 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
			betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
			betDiff_GT1000	 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
		}
		else 
		{
			if (latestUserBet <= 3 * BlindBet)
			{
				betDiff_0		 = 0 + 0 * BlindBet; 
				betDiff_0_200	 = diff;
// 				betDiff_200_500	 = Bet4Fold;
// 				betDiff_500_1000 = Bet4Fold;
// 				betDiff_GT1000	 = Bet4Fold;

				betDiff_200_500	 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				betDiff_GT1000	 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
			}
			else
			{
				betDiff_0        = 0 + 0 * BlindBet; 
// 				betDiff_0_200    = Bet4Fold;
// 				betDiff_200_500  = Bet4Fold;
// 				betDiff_500_1000 = Bet4Fold;
// 				betDiff_GT1000   = Bet4Fold;

				betDiff_0_200    = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				betDiff_200_500  = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
				betDiff_500_1000 = Bet4Fold;
				betDiff_GT1000   = Bet4Fold;
			}
		}
		break;
	//case SameColor:
		//betDiff_0		 = Bet4Check; 
		//betDiff_0_200	 = diff;
		//betDiff_200_500	 = Bet4Fold;
		//betDiff_500_1000 = Bet4Fold;
		//betDiff_GT1000	 = Bet4Fold;
		//break;
	case MiddleCardKQJT:
		betDiff_0		 = Bet4Check; 
		betDiff_0_200	 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
		betDiff_200_500	 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
		betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;
		betDiff_GT1000	 = Bet4Fold;
		break;
	default:
		betDiff_0		 = Bet4Check; 
		betDiff_0_200	 = Bet4Fold;
		betDiff_200_500	 = Bet4Fold;
		betDiff_500_1000 = Bet4Fold;
		betDiff_GT1000	 = Bet4Fold;
		break;
	}

																// 根据diff值选取某个输入作为下一轮的下注额
	betValid = GetValidBet(betDiff_0, betDiff_0_200, betDiff_200_500, betDiff_500_1000, betDiff_GT1000, diff);
	
	Hold_LatestBet_ActionCmd(betValid,diff,actionCmd);			// 根据validPara和diff值来获取响应指令	
}


