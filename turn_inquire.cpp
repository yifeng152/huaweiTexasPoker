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
** ��ȡת��Ȧ����Ӧָ��
*/
void GetActionCommandWithTurn(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard)
{
	CardType type;
	type = GetGreatestTypeFromSix(allCard,6);
	
	SortCardList(allCard, 2);			// �����ƴӴ�С���� A->K->...->2
	SortCardList(allCard + 2, 4);		// �����Ź����ƴ�С���� A->K->...->2

	int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;

	GetTurnActionCommandCore(allCard, type,actionCmd,diff);
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
void Turn_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
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
// void Turn_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
// {
// 	//���ݲ�����ȷ��ָ��
// 	if(validPara == Bet4AllIn)				// all_in
// 	{
// 		//strcpy(actionCmd, " all_in  \n");
// 		strcpy(actionCmd, "raise 2000\n");
// 
// 		allUserInfo.turnRaised = true;	// ��Ϊtrue ��ʾ�þ���ת��Ȧ�Ѽ�ע��
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
// 		if(!allUserInfo.turnRaised)			// �þ���ת��Ȧ��û�м�ע��
// 		{
// 			allUserInfo.turnRaised = true;	// ��Ϊtrue ��ʾ�þ���ת��Ȧ�Ѽ�ע��
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
** ��ȡת��Ȧ��Ӧָ��ĺ��ĺ���
*/
void GetTurnActionCommandCore(char *allCard, CardType type, char *actionCmd,int diff)
{
	int betStage1,betStage2,betStage3,betStage4,betStage5;
	int betValid;

	switch (type)
	{
		case CT_TONG_HUA_SHUN:	// ͬ��˳
		{
			betDiff_0		 = Bet4AllIn;
			betDiff_0_200	 = Bet4AllIn;
			betDiff_200_500  = Bet4AllIn;
			betDiff_500_1000 = Bet4AllIn;
			betDiff_GT1000   = Bet4AllIn;
			break;	
		}
		case CT_FOUR_TIAO:		// ����
		{
			// ���Ź������������
			if ( (allCard[2] & 0x0F) == (allCard[3] & 0x0F) &&
				 (allCard[2] & 0x0F) == (allCard[4] & 0x0F) &&
				 (allCard[2] & 0x0F) == (allCard[5] & 0x0F) )
			{
				if ((allCard[0] & 0x0F) == 0x01 ||		// ������A ��������Ϊ�ĸ�A������K
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
		case CT_HU_LU:			// ��«
		{
			// ������������
			if ((allCard[2] & 0x0F) == (allCard[3] & 0x0F) && (allCard[4] & 0x0F) == (allCard[5] & 0x0F))
			{
				if ( (allCard[2] & 0x0F) == (allCard[0] & 0x0F) ||
					 (allCard[2] & 0x0F) == (allCard[1] & 0x0F) )	// �����е�����һ���빫���еĽϴ�Ķ�����ɺ�«�е����� 
				{
					betDiff_0		 = Bet4AllIn;
					betDiff_0_200	 = Bet4AllIn;
					betDiff_200_500  = Bet4AllIn;
					betDiff_500_1000 = Bet4AllIn;
					betDiff_GT1000   = Bet4AllIn;
					
				}
				else// �����е�����һ���빫���еĽ�С������ɺ�«�е����� 
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
			// ������Ϊһ��������һ������
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
			{	// ���������ƺ͹�����ʽ��ɺ�«
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
		case CT_TONG_HUA:		// ͬ��
		{
			// ���Ź����ƵĻ�ɫһ��
			if ((allCard[2] & 0xF0) == (allCard[3] & 0xF0) &&
				(allCard[2] & 0xF0) == (allCard[4] & 0xF0) &&
				(allCard[2] & 0xF0) == (allCard[5] & 0xF0) )
			{
				// ����Ĵ�С��û��Ӱ��ģ�ֻ�ǲ�֪���������ƻ�ɫһ�£��ʽ���if-else�ж�

				// �����нϴ�������ƵĻ�ɫ�빫���е����Ż�ɫһ��
				if ((allCard[0] & 0xF0) == (allCard[2] & 0xF0))
				{
					// �����нϴ��������ΪA��K
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
					else // �����нϴ�������Ʋ���A��K
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
				else	// �����н�С�������ƵĻ�ɫ�빫���е����Ż�ɫһ��
				{					
					// �����н�С��������ΪA��K
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
					{	// �����нϴ�������Ʋ���A��K
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
			else	// ���Ź����ƺ������������ͬ��
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
		case CT_SHUN_ZI:		// ˳�� ������ܻ�������ϸ  ��Ҫ���ǲ�ͬ��������
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
		case CT_THREE_TIAO:		// ����
		{
			// ���ƾ�������
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
				// �������ƺ�һ�Ź����������
				if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
				{					
					bool isStraight = MayFormStraight(allCard+2, 4);	// �ж����������е�4���Ƿ������˳�ӵĿ�����
					bool isFlush = MayFormFlush(allCard+2,4);			// �ж����������е�4���Ƿ������ͬ���Ŀ�����

					if (isStraight || isFlush)	// ���ŵĵ����������˳�ӻ�ͬ��
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
			// һ�����ƺ�һ�Թ����������
			if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
			{
				// �����о�������
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
					// �����е������������
					betDiff_0		 = Bet4Check;
					betDiff_0_200	 = diff;
					//betDiff_200_500  = diff;
					betDiff_200_500  = Bet4Fold;
					betDiff_500_1000 = Bet4Fold;
					betDiff_GT1000 = Bet4Fold;
					// ���������ٻ���4�����������µĳ������ٻ���300ʱ������ ��Ϊ���е�ĳһ�����п����Ѿ���ɺ�«
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
					 // ���Ƶ��������ź͹��Ƶ���������������� ���߹��Ƶ�һ�Ժ͹��Ƶ�һ�����ź����Ƶ�һ���������һ��
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
		case CT_ONE_LONG:		// һ��
		{
			// ֻ��������һ��
			if( (allCard[0] & 0x0F) == (allCard[1] & 0x0F))
			{
				betDiff_0 = Bet4Check;

				// �����ɶ��ӵ�������С�ڹ����еĵڶ������ ������
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
				// ֻ�й�������һ��					
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

						bool isStraight = MayFormStraight(allCard, 6);	// �ж���6�����е�4���Ƿ������˳�ӵĿ�����
						bool isFlush = MayFormFlush(allCard, 6);		// �ж���6�����е�4���Ƿ������ͬ���Ŀ�����

						if (isStraight || isFlush)		// ���������ͬ��������˳��
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
						
						bool isStraight = MayFormStraight(allCard, 6);	// �ж���6�����е�4���Ƿ������˳�ӵĿ�����
						bool isFlush = MayFormFlush(allCard, 6);		// �ж���6�����е�4���Ƿ������ͬ���Ŀ�����
						
						if (isStraight || isFlush)		// ���������ͬ��������˳��
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
					// �����е����һ�ź��������һ������
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
		case CT_SINGLE:	// ����
		{
			betDiff_0 = Bet4Check;
			
			bool isStraight = MayFormStraight(allCard, 6);	// �ж���6�����е�4���Ƿ������˳�ӵĿ�����
			bool isFlush = MayFormFlush(allCard, 6);		// �ж���6�����е�4���Ƿ������ͬ���Ŀ�����

			if (isStraight || isFlush)		// ���������ͬ��������˳��
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
																// ����diffֵѡȡĳ��������Ϊ��һ�ֵ���ע��
	betValid = GetValidBet(betDiff_0, betDiff_0_200, betDiff_200_500, betDiff_500_1000, betDiff_GT1000, diff);

	Turn_LatestBet_ActionCmd(betValid,diff,actionCmd);			// ����validPara��diffֵ����ȡ��Ӧָ��		
}
