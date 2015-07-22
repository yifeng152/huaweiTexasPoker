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

bool isScareProper = true;	// �Ƿ�������Ȧ����һ���������½��л���
bool scareEnabled;			//ֻ��holdȦ
bool scareFailed;
int scareTryNum;			//���л��˵��ܴ���
int scareFailedNum;			//���˳ɹ����ܴ���

extern FILE *fp4ScarePlan;

/*
** ��ȡ����Ȧ����Ӧָ��
*/
void GetActionCommandWithHold(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard)
{
	HoldType holdtype;

	//��ִ�иú���֮���Ѿ�����
	holdtype = GetHoldCardType(allCard, 2);

	switch(status4Inqu)		// status4Inqu: ��ǰ�û���Ѻע״̬��־λ
	{		
		case Blind:	//ǰ�����˼�ע		
		{
			//�ﵽĳһ�����󣬶Ի��˵ĳɹ��ʽ����ж�,���ʧ���ʸ���1/3������ر�
			if( (allUserInfo.allUser[allUserInfo.ownIndex].money + allUserInfo.allUser[allUserInfo.ownIndex].jetton) <= 3000 )
			//if(allUserInfo.currentRound >= 100)
			{
				if (scareFailedNum * 3 >= scareTryNum)
				{
					isScareProper = false;
				}
			}
			
			//���ǰ�������˾����ƣ���������С��2��������С��ʱ��������ģʽ
			//����Ǵ�ä�����ڸ�״̬����˵���Ѿ�Ӯ�ˣ�ֻҪ�����Ƽ��ɡ�
			//�����Сä�����ڸ�״̬������ֻ��һ����ä
			//�������äע�����ڸ�״̬�������д�ä��Сä�����Կ���һ����
			//����� �������� ��ȥ fold������ ��ȥһ��äע ��ȥһ���Լ�  С�ڵ���2����
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
				allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��	
			}
			else
			{
				int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;
				
				if (holdtype == HighPair)			// ���������Ǵ����
				{
					//strcpy(actionCmd, "raise 400\n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + 400;
					
					//int raiseNumber = 1 * BlindBet;
					//sprintf(actionCmd, "raise %d\n", raiseNumber);
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
					//allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��					
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 4 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
					}
					else
					{
						int raiseNumber = 1 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
					}
				}
				else if (holdtype == LowPair)		// ����������С����
				{
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 2 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
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
					//allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
				}
				else if (holdtype == HighCardA)		// ������������һ��A
				{
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 1 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
					}
					else
					{
						strcpy(actionCmd, " call \n");
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
					}
					//strcpy(actionCmd, " call \n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				//else if (holdtype == SameColor)		// �������ƻ�ɫһ��
				//{
				//	strcpy(actionCmd, " call \n");
				//	allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;//100
				//}
				else if (holdtype == MiddleCardKQJT)	// �����������ٴ��ڵ���10
				{
					strcpy(actionCmd, " call \n");
					allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;//100
				}
				else if (diff == 0)//Ϊ�����äҲ���Ƶ���� wq 2015-05-30 10-48
				{
					strcpy(actionCmd, " check \n");
					allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				// ��Ӧ�Լ���Сäע �������µ�ѺעΪäע��С ���ҵ�ǰ��Ч�û���<=3
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
		case Allout:		// �����˾��Ѿ����Ƶ����������ϵͳ���ܲ�����֪ͨ������
		{
			strcpy(actionCmd, " check \n");
			break;
		}
		case Handle:
		{
			int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;
			
			//enum HoldType { HighPair, LowPair, HighCardA, SameColor, Other };
			// HighPair: Pair 9 ~ Pair A  LowPair: Pair 2 ~ Pair 8 HighCardA: a A SameColor: 
			// ��ǰ�����ע��ΪBlindBet
			if (latestUserBet == BlindBet)	//ǰ�����˼�ע
			{
				int diff = latestUserBet - allUserInfo.allUser[allUserInfo.ownIndex].bet;
				
				if (holdtype == HighPair)			// ���������Ǵ����
				{
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 4 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
					}
					else
					{
						int raiseNumber = 1 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
					}
					//strcpy(actionCmd, "raise 400\n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + 400;

					//int raiseNumber = 1 * BlindBet;
					//sprintf(actionCmd, "raise %d\n", raiseNumber);
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
					//allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
				}
				else if (holdtype == LowPair)		// ����������С����
				{
					//strcpy(actionCmd, "raise 200\n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + 200;
					
					//int raiseNumber = 2 * BlindBet;
					//sprintf(actionCmd, "raise %d\n", raiseNumber);
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
					//allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
	
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 2 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
					}
					else
					{
						strcpy(actionCmd, "check \n");
						allUserInfo.allUser[allUserInfo.ownIndex].bet = BlindBet;
					}
					//strcpy(actionCmd, "call \n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				else if (holdtype == HighCardA)		// ������������һ��A
				{
					if (allUserInfo.allUserCount <= RiskCount)
					{
						int raiseNumber = 1 * BlindBet;
						sprintf(actionCmd, "raise %d\n", raiseNumber);
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
						allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
					}
					else
					{
						strcpy(actionCmd, " call \n");
						allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
					}
					//strcpy(actionCmd, "call\n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				//else if (holdtype == SameColor)		// �������ƻ�ɫһ��
				//{
				//	strcpy(actionCmd, " check \n");
				//	//allUserInfo.allUser[allUserInfo.ownIndex].bet = 100;
				//	allUserInfo.allUser[allUserInfo.ownIndex].bet = BlindBet;
				//}
				else if (holdtype == MiddleCardKQJT)	// �������ƶ����ڵ���10
				{
					strcpy(actionCmd, " check \n");
					//allUserInfo.allUser[allUserInfo.ownIndex].bet = 100;
					allUserInfo.allUser[allUserInfo.ownIndex].bet = BlindBet;
				}
				else if (diff == 0)	// ��Ӧ�Լ��Ǵ�äע�����
				{
					strcpy(actionCmd, " check \n");
					allUserInfo.allUser[allUserInfo.ownIndex].bet += diff;
				}
				// ��Ӧ�Լ���Сäע ���ҵ�ǰ��Ч�û���<=3
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

// static int GetValidBet(int firstPara,int secondPara,int thirdPara,int fourthPara,int fifthPara,int diff)
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
void Hold_LatestBet_ActionCmd(int validPara,int diff,char *actionCmd)
{
	//���ݲ�����ȷ��ָ��
	if(validPara == Bet4AllIn)				// all_in
	{
		int raiseNumber = 40 * BlindBet;
		sprintf(actionCmd, "raise %d\n", raiseNumber);
		allUserInfo.allUser[allUserInfo.ownIndex].bet += diff + raiseNumber;
		
		//strcpy(actionCmd, " all_in  \n");
		//strcpy(actionCmd, "raise 2000\n");
		
		allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ���ת��Ȧ�Ѽ�ע��
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
		if(!allUserInfo.holdRaised)			// �þ��ڹ���Ȧ��û�м�ע��
		{
			allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ��ڹ���Ȧ�Ѽ�ע��
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
// 	//���ݲ�����ȷ��ָ��
// 	if(validPara == Bet4AllIn)				// all_in
// 	{
// 		//strcpy(actionCmd, " all_in  \n");
// 		strcpy(actionCmd, "raise 2000\n");
// 		
// 		allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ���ת��Ȧ�Ѽ�ע��
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
// 		if (!allUserInfo.holdRaised)		// �þ�������Ȧ��û�м�ע��
// 		{
// 			allUserInfo.holdRaised = true;	// ��Ϊtrue ��ʾ�þ�������Ȧ�Ѽ�ע��
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

																// ����diffֵѡȡĳ��������Ϊ��һ�ֵ���ע��
	betValid = GetValidBet(betDiff_0, betDiff_0_200, betDiff_200_500, betDiff_500_1000, betDiff_GT1000, diff);
	
	Hold_LatestBet_ActionCmd(betValid,diff,actionCmd);			// ����validPara��diffֵ����ȡ��Ӧָ��	
}


