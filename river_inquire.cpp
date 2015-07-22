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
** ��ȡ����Ȧ����Ӧָ��
*/
void GetActionCommandWithRiver(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard)
{
	CardType type;
	type = GetGreatestTypeFromSeven(allCard, 7);
	
	SortCardList(allCard, 2);			// �����ƴӴ�С���� A->K->...->2
	SortCardList(allCard + 2, 5);		// �����Ź����ƴ�С���� A->K->...->2

	int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;

	GetRiverActionCommandCore(allCard, type,actionCmd,diff);
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
// int GetValidBet(int firstPara,int secondPara,int thirdPara,int fourthPara,int fifthPara,int diff)
// {
// 	int vaildPara;		// ���Լ�����һ�ֵĻ����ϼӵ�Ǯ
// 	
// 	//����diff,ѡ����ʵĲ���
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
** ����validPara��diffֵ����ȡ��Ӧָ��
*/
void River_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
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

// void River_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
// {
// 	//���ݲ�����ȷ��ָ��
// 	if(validPara == Bet4AllIn)				// all_in
// 	{
// 		//strcpy(actionCmd, " all_in  \n");
// 		strcpy(actionCmd, "raise 2000\n");
// 		
// 		allUserInfo.riverRaised = true;	// ��Ϊtrue ��ʾ�þ���ת��Ȧ�Ѽ�ע��
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
// 		if(!allUserInfo.riverRaised)		 // �þ���ת��Ȧ��û�м�ע��
// 		{
// 			allUserInfo.riverRaised = true;	// ��Ϊtrue ��ʾ�þ���ת��Ȧ�Ѽ�ע��
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
** ��ȡ����Ȧ��Ӧָ��ĺ��ĺ���
*/
void GetRiverActionCommandCore(char *allCard, CardType type, char *actionCmd,int diff)
{
	int betStage1,betStage2,betStage3,betStage4,betStage5;
	int betValid;

	switch (type)
	{
		case CT_TONG_HUA_SHUN:	// ͬ��˳
		{
			CardType typeOfCommonCard;
			typeOfCommonCard = GetFiveCardsType(allCard + 2, 5);

			if (typeOfCommonCard == CT_TONG_HUA_SHUN)	// �����е�5�����ͬ��˳
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
		case CT_FOUR_TIAO:		// ����
		{
			// ���Ź������е������������
			//if ((allCard[2] & 0x0F) == (allCard[5] & 0x0F) || (allCard[3] & 0x0F) == (allCard[6] & 0x0F))
			if ((allCard[2] & 0x0F) == (allCard[5] & 0x0F))		// �����е�ǰ�����������
			{
				if ((allCard[2] & 0x0F) == 0x01)	// �����е�ǰ����Ϊ����A
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

						if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
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
				else		// �����е�ǰ���Ų�������A
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
						
						if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
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
			else if ((allCard[3] & 0x0F) == (allCard[6] & 0x0F))		// �����еĺ������������
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
					
					if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
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
		case CT_HU_LU:			// ��«
		{
			// 5�Ź�����ɺ�«�������˿���Ҳ����������� 
			// �����е����ž��Ǻ�« ���Ҷ����ں����� 
			if ((allCard[2] & 0x0F) == (allCard[4] & 0x0F) && (allCard[5] & 0x0F) == (allCard[6] & 0x0F))
			{
				// ����Ҳ�Ƕ��� ��������Ϊ��A�����ƵĶ��Ӵ��ڹ����еĶ���
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

					if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
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
			// �����е����ž��Ǻ�« ���Ҷ�����ǰ���� 
			else if ((allCard[2] & 0x0F) == (allCard[3] & 0x0F) && (allCard[4] & 0x0F) == (allCard[6] & 0x0F))
			{
				// ����Ҳ�Ƕ��� ��������Ϊ��A�����ƵĶ��Ӵ��ڹ����еĶ���
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
					
					if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
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
			else	// ������Ϊ���Ի�������������ʽ ���ﻹ��Ҫ�پ���һ��
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
		case CT_TONG_HUA:		// ͬ��  ����Ҫ��ϸ��
		{
			CardType typeOfCommonCard;
			typeOfCommonCard = GetFiveCardsType(allCard + 2, 5);

			char commonCard[5] = {allCard[2], allCard[3], allCard[4], allCard[5], allCard[6]};
			SortCardListByColor(commonCard, 5);			

			if (typeOfCommonCard == CT_TONG_HUA)	// �����е�5�����ͬ��˳
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
					
					if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
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
			// ���Ź����е��������Ż�ɫһ��
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
					
					if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
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
			else		// ͬ���е������������е��������ͬ��˳ �����ǿ϶��� �������ж�
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
					
					if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
					{
						betDiff_500_1000 = Bet4Check;
						betDiff_GT1000   = Bet4Check;
					}
					else
					{		
						betDiff_500_1000 = allUserBehavior.needConser ? Bet4Fold : Bet4Survive;		// 3 + 2����������������ǲ���̫��ϧ�˰���
						betDiff_GT1000   = Bet4Fold;
					}
				}
			}
			break;	
		}
		case CT_SHUN_ZI:		// ˳�� ������ܻ�������ϸ  ��Ҫ���ǲ�ͬ��������
		{
			CardType typeOfCommonCard;
			typeOfCommonCard = GetFiveCardsType(allCard + 2, 5);
			
			if (typeOfCommonCard == CT_SHUN_ZI)	// ���Ź��Ʊ������˳��
			{
				bool isA = (allCard[2] & 0x0F) == 0x01;	// ˳���е��������A

				bool isFlush = MayFormFlush(allCard + 2, 5);		// �ж�5�Ź����е�4���Ƿ������ͬ���Ŀ�����

				bool isUpAdjacent = (allCard[0] & 0x0F) == ((allCard[2] & 0x0F) + 0x01) // ��˳�ӵ����������
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
					
					if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
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
				bool isStraight = MayFormStraight(allCard + 2, 5);	// �ж���5�Ź����е�4���Ƿ������˳�ӵĿ�����
				bool isFlush    = MayFormFlush(allCard + 2, 5);		// �ж���5�Ź����е�4���Ƿ������ͬ���Ŀ�����
				
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
								j = j;		// ������
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

				// ���Ź����е����żȲ���ͬ�����Ҹ�������ɵ�˳���Ǵ�˳�ӣ�Ҳ���������ͷ˳��˵�˳�� �Ӵ�С��
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
					
					if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
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
		case CT_THREE_TIAO:		// ����
		{
			// ���ƾ�������
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
				// ���ƶ��Ӻ�һ�Ź����������
				if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
				{					
					CardType typeOfCommonCard;
					typeOfCommonCard = GetFiveCardsType(allCard + 2, 5);

					bool isStraight = MayFormStraight(allCard + 2, 5);	// �ж������Ź����е������Ƿ������˳�ӵĿ�����
					bool isFlush    = MayFormFlush(allCard + 2, 5);		// �ж������Ź����е������Ƿ������ͬ���Ŀ�����

					// ���Ź����еĵ����Ų����˳�ӻ�ͬ��������Ϊ����
					if (!isStraight && !isFlush && typeOfCommonCard == CT_SINGLE)	
					{
						betDiff_0		 = Bet4AllIn;
						betDiff_0_200	 = Bet4AllIn;
						betDiff_200_500  = Bet4AllIn;
						betDiff_500_1000 = Bet4AllIn;
						betDiff_GT1000   = Bet4AllIn;
					}
					// ���Ź����еĵ����Ų����˳�ӻ�ͬ��������Ϊ���Ӳ��������еĶ��Ӵ��ڹ����еĶ���
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
							
							if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
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
						
						if (allUserInfo.allUser[allUserInfo.ownIndex].jetton <= 500)	// �����еĳ����Ѳ���500ʱ
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
					// һ�����ƺ�һ�Թ����������
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
		case CT_TWO_LONG:		// ����
		{
			CardType typeOfCommonCard;
			typeOfCommonCard = GetFiveCardsType(allCard + 2, 5);
			
			bool isStraight = MayFormStraight(allCard + 2, 5);	// �ж������Ź����е������Ƿ������˳�ӵĿ�����
			bool isFlush    = MayFormFlush(allCard + 2, 5);		// �ж������Ź����е������Ƿ������ͬ���Ŀ�����

			// ���Ź����е����żȲ������ͬ��Ҳ��ͬ���˳�� ���ҵ�ǰ��Ч����<=3
			if (! ((isStraight || isFlush) && allUserInfo.validUserCount > 3))
			{
				// һ�����ƺ�һ�Թ����������
				if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
				{
					// �����о�������
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
					// �����е������������
					if ((allCard[2] & 0x0F) == (allCard[3] & 0x0F) && (allCard[4] & 0x0F) == (allCard[5] & 0x0F))
					{
						betDiff_0 = Bet4Check;
						
						// �������Ƿ���A��K
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
						// ���Ƶ��������ź͹��Ƶ���������������� ���߹��Ƶ�һ�Ժ͹��Ƶ�һ�����ź����Ƶ�һ���������һ��
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
		case CT_ONE_LONG:		// һ��
		{
			bool isStraight = MayFormStraight(allCard + 2, 5);	// �ж������Ź����е������Ƿ������˳�ӵĿ�����
			bool isFlush    = MayFormFlush(allCard + 2, 5);		// �ж������Ź����е������Ƿ������ͬ���Ŀ�����
			
			// ���Ź����е����żȲ������ͬ��Ҳ��ͬ���˳�� ���ҵ�ǰ��Ч����<=3
			if (! ((isStraight || isFlush) && allUserInfo.validUserCount > 3))
			{	
				// ֻ��������һ��
				if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
				{
					betDiff_0 = Bet4Check;

					// �����ɶ��ӵ�������С�ڹ����еĵڶ������ ������
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
					// ֻ�й�������һ��					
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
						//�����е����һ�ź��������һ������
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
		case CT_SINGLE:	// ����
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
																// ����diffֵѡȡĳ��������Ϊ��һ�ֵ���ע��
	betValid = GetValidBet(betDiff_0, betDiff_0_200, betDiff_200_500, betDiff_500_1000, betDiff_GT1000, diff);
	
	River_LatestBet_ActionCmd(betValid,diff,actionCmd);			// ����validPara��diffֵ����ȡ��Ӧָ��	
}
