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
** ��ȡ����Ȧ����Ӧָ��
*/
void GetActionCommandWithFlop(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard)
{
	CardType type;
	type = GetFiveCardsType(allCard, 5);

	SortCardList(allCard, 2);			// �����ƴӴ�С���� A->K->...->2
	SortCardList(allCard + 2, 3);		// �����Ź����ƴ�С���� A->K->...->2

	int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;

	GetFlopActionCommandCore(allCard, type, actionCmd, diff);
}

/*
** ����diffֵѡȡĳ��������Ϊ��һ�ֵ���ע��
*/
int GetValidBet(int firstPara,int secondPara,int thirdPara,int fourthPara,int fifthPara,int diff)
{
	int vaildPara;		// ���Լ�����һ�ֵĻ����ϼӵ�Ǯ
	
	//����diff,ѡ����ʵĲ���
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
** ����validPara��diffֵ����ȡ��Ӧָ��
*/
void Flop_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
{
	//���ݲ�����ȷ��ָ��
	if(validPara == Bet4AllIn)				// all_in
	{
		int raiseNumber = 40 * BlindBet;
		sprintf(actionCmd, "raise %d\n", raiseNumber);
		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;

		//strcpy(actionCmd, " all_in  \n");
		//strcpy(actionCmd, "raise 2000\n");
		
		allUserInfo.flopRaised = true;	// ��Ϊtrue ��ʾ�þ���ת��Ȧ�Ѽ�ע��
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
		if(!allUserInfo.flopRaised)			// �þ��ڹ���Ȧ��û�м�ע��
		{
			allUserInfo.flopRaised = true;	// ��Ϊtrue ��ʾ�þ��ڹ���Ȧ�Ѽ�ע��
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
** ��ȡ����Ȧ��Ӧָ��ĺ��ĺ���
*/
void GetFlopActionCommandCore(char *allCard, CardType type, char *actionCmd,int diff)
{
	// -2: all_in -1: fold positive: ��ע��ע 0: ��
	// betStage1: diff=0 betStage2: diff<= 200 betStage3: diff<=500 betStage4: diff <= 1000 betStage5:diff > 1000
	int betStage1, betStage2, betStage3, betStage4, betStage5;
	int betValid;

	switch (type)
	{
		case CT_TONG_HUA_SHUN:	// ͬ��˳
		case CT_FOUR_TIAO:		// ����
		case CT_HU_LU:			// ��«
		case CT_TONG_HUA:		// ͬ��
		case CT_SHUN_ZI:		// ˳��
		{
			betDiff_0 = betDiff_0_200 = betDiff_200_500 = betDiff_500_1000 = betDiff_GT1000 = Bet4AllIn; 			
			break;	
		}
		case CT_THREE_TIAO:		// ����
		{
			if ((allCard[2] & 0x0F) == (allCard[4] & 0x0F))		// ���ƾ�������
			{	
				betDiff_0 = Bet4Check;
				betDiff_0_200 = diff;

				// ���ƾ�������  ������һ��A ����������һ���ƴ��ڵ���Q ��ע
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
				else	// ����
				{
					betDiff_200_500  = Bet4Fold;
					betDiff_500_1000 = Bet4Fold;
					betDiff_GT1000   = Bet4Fold;
				}
			}
			else
			{
				// �������ƺ�һ�Ź����������
				if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
				{
					betDiff_0 = Bet4AllIn;
				
					// ��ǰ����ע�����1000 ������ע���Ѵ�������ʣ�µĳ��� ��ע
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
				else // һ�����ƺ�һ�Թ����������
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
		case CT_TWO_LONG:		// ����
		{			
			if ((allCard[0] & 0x0F) == (allCard[1] & 0x0F))	// һ�����ƺ�һ�Թ����������
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
			else // ���Ƶ��������ź͹��Ƶ���������������� ���߹��Ƶ�һ�Ժ͹��Ƶ�һ�����ź����Ƶ�һ���������һ��
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
		case CT_ONE_LONG:		// һ��
		{
			// ֻ��������һ��
			if ((allCard[0] & 0x0F) == (allCard[1] & 0x0F))
			{
				bool holdGTFlop;	// ���Ʊȹ���������ƻ���
				if ((allCard[0] & 0x0F) == 0x01)	// ������AA,��Ȼ�ȹ���������ƻ��󣬿��Կ��Ǽ�ע
				{
					holdGTFlop = true;
				}
				else if ((allCard[2] & 0x0F) == 0x01)	// ��������AA
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

				if (holdGTFlop == true)	// ���Ʊȹ���������ƻ��󣬿��Կ��Ǽ�ע
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
				// ֻ�й�������һ��					
				if ( ((allCard[2] & 0x0F) == (allCard[3] & 0x0F)) ||
					 ((allCard[3] & 0x0F) == (allCard[4] & 0x0F)) ||
					 ((allCard[2] & 0x0F) == (allCard[4] & 0x0F)) )
				{
					betDiff_0 = Bet4Check;

					bool isStraight = MayFormStraight(allCard, 5);	// �ж����������е�4���Ƿ������˳�ӵĿ�����
								
					bool isFlush = MayFormFlush(allCard, 5);		// �ж����������е�4���Ƿ������ͬ���Ŀ�����
					
					if (isStraight || isFlush)	// ���ŵĵ����������˳�ӻ�ͬ��
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
				{  	// һ�����ƺ�һ�Ź������һ��

					// �����е������ǡ�����������һ��
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
		case CT_SINGLE:	// ����
		{
			betDiff_0 = Bet4Check;

			bool isStraight = MayFormStraight(allCard, 5);	// �ж����������е�4���Ƿ������˳�ӵĿ�����
								
			bool isFlush = MayFormFlush(allCard, 5);		// �ж����������е�4���Ƿ������ͬ���Ŀ�����

			if (isStraight || isFlush)		// ���ŵĵ����������˳�ӻ�ͬ��
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

																// ����diffֵѡȡĳ��������Ϊ��һ�ֵ���ע��
	betValid = GetValidBet(betDiff_0, betDiff_0_200, betDiff_200_500, betDiff_500_1000, betDiff_GT1000, diff);
																
	Flop_LatestBet_ActionCmd(betValid,diff,actionCmd);			// ����validPara��diffֵ����ȡ��Ӧָ��	
}
