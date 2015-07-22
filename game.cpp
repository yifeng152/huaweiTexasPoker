/* File Name: game.cpp */  

#define Linux  1

#include "getcard.h"
#include "cardtype.h"
#include "hold_inquire.h"
#include "flop_inquire.h"
#include "turn_inquire.h"
#include "river_inquire.h"
#include "notify.h"
#include "game.h"

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  

#if Linux
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
  
#include <arpa/inet.h>  
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#define MAXLINE 4096  // ԭ����ֵΪ300 ̫С�� ������ʱ���������� 2015-05-17
					  // ע�⣺"raise num\n"��raiseǰ��һ�������пո�

struct AllUserInfo allUserInfo;
struct AllUserBehavior allUserBehavior;

HoldType GetHoldCardType(char holdData[], char cbCardCount);
void GetActionCommand(char *buf, char *allCard, char *actionCmd);
void GetInitialUserInfo(char *buf, char *ownerID);
//void GetInitialUserBehavior_UserID(char *buf);
void GetActionCommandWithFlop(Status4Inquire status4Inqu, int latestUserBet, char *actionCmd, char *allCard);


void GetInitialRoundUserBehavior_UserID(char *buf,char *ownerID);
void GetEveryRoundUserBehavior_UserID(char *buf);
void GetPlayerHoldActionFromInquire(char *buf);
void GetPlayerHoldActionFromNotify(char *buf);
void GetPlayerHoldActionFromFlopInquire(char *buf);
void GetPlayerHoldActionFromFlopNotify(char *buf);
bool isIncluded(int buttonIndex,int ownIndex,int holdFirstActionIndex);
void GetAllUserBahaviorDangerScore(char *buf);

static Status status;		//
int currentBet = 0;

//��hold_inquire.cpp��
extern bool scareEnabled;
extern bool scareFailed;
extern int scareTryNum;		//���л��˵��ܴ���
extern int scareFailedNum;	//���˳ɹ����ܴ���
FILE *fp4ScarePlan;


bool getInitialRoundUserBehaviorDone = false;
bool userHobbyDetectionEnabled = true;		//��ʼʱ��Ĭ�Ͽ��Խ����û�ϰ�߼�⡣����������С�ڵ���3ʱ���رոñ�־
bool needCheckInFlop = false;				//�ñ�־λtrue��������Ҫ���flop�е�inquire����notify
bool flopNotComes;							//flop�׶λ�δ����
bool flopBegins;							//���ν���flop�׶�


int main(int argc, char** argv)  
{
	int	sockfd, n,rec_len;  
    char    recvline[4096], sendline[4096];  
    char    buf[MAXLINE];  

	static FILE *fp,*fp4CardInfo,*fp4Status, *fp4UserAction;
#if Linux
	struct	sockaddr_in    servaddr;  


	static int flag = 0;
	char file_name[50];
	char file_name4CardInfo[40];
	char file_name4Status[40];
	char file_name4ScarePlan[40];
	char file_name4UserAction[40];

	strcpy(file_name, argv[5]);
	strcat(file_name, ".txt");

	strcpy(file_name4CardInfo, argv[5]);	
	strcat(file_name4CardInfo, "4CardInfo");
	strcat(file_name4CardInfo, ".txt");
	
	strcpy(file_name4Status, argv[5]);	
	strcat(file_name4Status, "4Status");
	strcat(file_name4Status, ".txt");

	strcpy(file_name4ScarePlan, argv[5]);	
	strcat(file_name4ScarePlan, "_scarePlan");
	strcat(file_name4ScarePlan, ".txt");
	
	strcpy(file_name4UserAction, argv[5]);	
	strcat(file_name4UserAction, "_UserAction");
	strcat(file_name4UserAction, ".txt");

	if (flag == 0)
	{
		flag = 1;
		fp = fopen(file_name, "w");
		fp4CardInfo = fopen(file_name4CardInfo, "w");
		fp4Status = fopen(file_name4Status, "w");
		fp4ScarePlan = fopen(file_name4ScarePlan, "w");
		fp4UserAction = fopen(file_name4UserAction, "w");
	}
	if (argc != 6)
    {
       	printf("Please input the IP address of the server \n", argv[0]);
       	exit(1);
	}
	
	struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));			
	client_addr.sin_family = AF_INET;				   
    inet_pton(AF_INET, argv[3], &client_addr.sin_addr) ;
    client_addr.sin_port = htons(atoi(argv[4]));	
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

	// ����socketѡ���ַ�ظ�ʹ�ã���ֹ����������˳����´�����ʱbindʧ��  linaijun 2015-05-26
	int is_reuse_addr = 1;
	setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&is_reuse_addr, sizeof(is_reuse_addr));

 	bind(client_socket, (struct sockaddr*) &client_addr, sizeof(client_addr));
 
 	memset(&servaddr, 0, sizeof(servaddr));  
 	servaddr.sin_family = AF_INET;  
 	servaddr.sin_port = htons(atoi(argv[2]));  
 	inet_pton(AF_INET, argv[1], &servaddr.sin_addr) ;
 
 	while (connect(client_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0);
 
 	strcpy(sendline, "reg: ");
 	strcat(sendline, argv[5]);
 	//strcat(sendline, " linaijun \n");
	strcat(sendline, " linaijun need_notify\n");	// linaijun 2015-05-27
 
 	send(client_socket, sendline, strlen(sendline), 0);

	allUserInfo.currentRound = 0;
	
	scareTryNum = 0;
	scareFailedNum = 0;

#endif

	while (1)
	{
		static int count = 1;
		static char allCard[7];
		int i;
#if Linux
 		memset(buf, 0, sizeof(buf));
 		while ((rec_len = recv(client_socket, buf, MAXLINE,0)) <= 0) ;
#endif
 		if (strstr(buf, "seat/") != NULL)		// linaijun 2015-05-14
 		{
			//int arrayIndex = 0;		

			scareEnabled = false;		// ���û���ʹ�ܱ�־λ
			scareFailed = false;		// ���û���ʧ�ܱ�־λ
			
			allUserInfo.currentRound += 1;	// ��ǰ���еľ�����1
			allUserBehavior.currentRound += 1;

 			GetInitialUserInfo(buf, argv[5]);				// ��ȡÿһ�ָտ�ʼʱ�ĸ����û���Ϣ
		
			if (true == userHobbyDetectionEnabled)
			{
				

				for (i = 0; i < allUserBehavior.allUserCount; i++)
				{
					if(i == allUserBehavior.ownIndex)
						continue;
					if (allUserBehavior.allUser[i].isOver == true)
					{
						continue;
					}
					if (strcmp(allUserBehavior.allUser[i].holdAction, "raise") == 0)
					{
						//allUserBehavior.allUser[i].holdRaiseTimes++;
						allUserBehavior.allUser[i].holdRaiseTimesArray[(allUserBehavior.currentRound-1) % 50] = 1;
					}
					else
					{
						allUserBehavior.allUser[i].holdRaiseTimesArray[(allUserBehavior.currentRound-1) % 50] = 0;
					}
					
					if (strcmp(allUserBehavior.allUser[i].holdAction, "check") == 0 ||
						strcmp(allUserBehavior.allUser[i].holdAction, "call") == 0)
					{
						//allUserBehavior.allUser[i].holdCheckTimes++;
						allUserBehavior.allUser[i].holdCheckTimesArray[(allUserBehavior.currentRound-1)%50] = 1;
					}
					else
					{
						allUserBehavior.allUser[i].holdCheckTimesArray[(allUserBehavior.currentRound-1)%50] = 0;
					}

					if  (strcmp(allUserBehavior.allUser[i].holdAction, "fold") == 0)
					{
						//allUserBehavior.allUser[i].holdFoldTimes++;
						allUserBehavior.allUser[i].holdFoldTimesArray[(allUserBehavior.currentRound-1)%50] = 1;
					}
					else
					{
						allUserBehavior.allUser[i].holdFoldTimesArray[(allUserBehavior.currentRound-1)%50] = 0;
					}
					
					if (strcmp(allUserBehavior.allUser[i].holdAction, "all_in") == 0)
					{
						//allUserBehavior.allUser[i].holdAllinTimes++;
						allUserBehavior.allUser[i].holdAllinTimesArray[(allUserBehavior.currentRound-1)%50] = 1;
					}
					else
					{
						allUserBehavior.allUser[i].holdAllinTimesArray[(allUserBehavior.currentRound-1)%50] = 0;
					}
				}

				if (allUserBehavior.currentRound > 60)
				{
					for (i = 0; i < allUserBehavior.allUserCount; i++)
					{
						if(i == allUserBehavior.ownIndex)
							continue;
						if (allUserBehavior.allUser[i].isOver == true)
						{
							continue;
						}
						int foldSum=0;
						int checkSum=0;
						int raiseSum=0;
						int allinSum=0;
						for (int j = 0; j < 50; ++j)
						{
							foldSum += allUserBehavior.allUser[i].holdFoldTimesArray[j];					
							checkSum += allUserBehavior.allUser[i].holdCheckTimesArray[j];					
							raiseSum += allUserBehavior.allUser[i].holdRaiseTimesArray[j];					
							allinSum += allUserBehavior.allUser[i].holdAllinTimesArray[j];					
						}
						
						if (foldSum > 34)
						{
							allUserBehavior.allUser[i].dangerScore = 5;
						}
						else if (raiseSum + 2 * allinSum >= 5)
						{
							allUserBehavior.allUser[i].dangerScore = 1;
						}
						else
						{
							allUserBehavior.allUser[i].dangerScore = 2;
						}
					}
				}
				
// 				fprintf(fp4UserAction, "currentRound: %d\n", allUserBehavior.currentRound);
// 				for (i = 0; i < allUserBehavior.allUserCount; i++)
// 				{
// 					fprintf(fp4UserAction, "%s: RaiseTimes: %d,  CheckTimes: %d,  FoldTimes: %d, AllinTimes: %d\n", 
// 						allUserBehavior.allUser[i].userID, allUserBehavior.allUser[i].holdRaiseTimes,
// 						allUserBehavior.allUser[i].holdCheckTimes, allUserBehavior.allUser[i].holdFoldTimes,
// 						allUserBehavior.allUser[i].holdAllinTimes);
// 				}
// 
// 				fprintf(fp4UserAction, "\n");

				fprintf(fp4UserAction, "currentRound: %d\n", allUserBehavior.currentRound);

				for (i = 0; i < allUserBehavior.allUserCount; i++)
				{
					fprintf(fp4UserAction, "%s: DangerScore: %d\n", 
						allUserBehavior.allUser[i].userID, allUserBehavior.allUser[i].dangerScore);
				}

				fprintf(fp4UserAction, "totalDangerScore: %d\n", 
						allUserBehavior.totalDangerScore);

				fprintf(fp4UserAction, "\n");


				//				
				if (getInitialRoundUserBehaviorDone == false)
				{
					GetInitialRoundUserBehavior_UserID(buf, argv[5]);
					getInitialRoundUserBehaviorDone = true;
				}

				GetEveryRoundUserBehavior_UserID(buf);
				
				flopNotComes = true;
				needCheckInFlop = false;
			}			
 		}
		
		char *head4Hold; char holdCard[2];
		char *head4Flop; char flopCard[3];
		char *head4Turn; char turnCard[1];
		char *head4River; char riverCard[1];

		if ((head4Hold = strstr(buf, "hold")) != NULL)	
		{
			status = Hold;
			GetHoldCard(head4Hold, holdCard);		// ��ȡ����
			strncpy(allCard, holdCard, 2);
		}
		else if ((head4Flop = strstr(buf, "flop")) != NULL)	
		{
			status = Flop;
			GetFlopCard(head4Flop, flopCard);		// ��ȡ����
			strncpy(allCard + 2, flopCard, 3);
		}
		else if ((head4Turn = strstr(buf, "turn")) != NULL)	
		{
			status = Turn;
			GetTurnCard(head4Turn, turnCard);		// ��ȡת��
			strncpy(allCard + 5, turnCard, 1);
		}
		else if ((head4River = strstr(buf, "river")) != NULL)	
		{
			status = River;
			GetRiverCard(head4River, riverCard);	// ��ȡ����
			strncpy(allCard + 6, riverCard, 1);
		}

		if (true == userHobbyDetectionEnabled)
		{
			if (true == flopNotComes)
			{				
				if (Flop == status)
				{
					flopBegins = true;
					flopNotComes = false;  //linaijun 2015-06-10
				}
				//flopNotComes = false;
			}
			if (true == flopBegins)
			{
				//�ж��Ƿ���Ҫ�鿴flop�׶ε�inquire����notify
				//��ownIndex��buttonIndex��holdFirstActionIndex֮��ʱ��Ҫ��flop���ƽ����ж�
				needCheckInFlop = isIncluded(allUserBehavior.buttonIndex, allUserBehavior.ownIndex, allUserBehavior.holdFirstActionIndex);
	
				flopBegins = false;
			}//end if 
		}
		
#if Linux
		fprintf(fp, "%d %s\n", count++, buf);

		//if (count > 200) 		{  close(sockfd);   exit(0); }
#endif		
		if (strstr(buf, "inquire/") != NULL)	
		{
			if (scareEnabled == true)
			{
				scareFailed = true;
				scareFailedNum += 1;
				scareEnabled = false;
			}
			if (scareFailed == true)
			{
				//record 
				char str4ScarePlan[20];
				sprintf(str4ScarePlan, "Round%d ScareFailed",allUserInfo.currentRound);
				fprintf(fp4ScarePlan, "  %s", str4ScarePlan);

				scareFailed = false;
			}
			
			if (true == userHobbyDetectionEnabled)
			{
				if (Hold == status)
				{
					GetPlayerHoldActionFromInquire(buf);
				}
				if (needCheckInFlop == true)
				{
					GetPlayerHoldActionFromFlopInquire(buf);
					needCheckInFlop = false;
				}
			}			

			// if you have fold, you will still reveive the inquire, but you should not send the actionCmd.
			char actionCmd[20];						// ��ȡѯ��ʱ������ָ��
			
			if (allUserBehavior.currentRound >= 61)
			{
				GetAllUserBahaviorDangerScore(buf);
			}
			else
			{
				allUserBehavior.needConser = true;
			}
			

			GetActionCommand(buf, allCard, actionCmd);

			strcpy(sendline, actionCmd);
#if Linux
			//if (allUserInfo.allUser[allUserInfo.ownIndex].isValid == true)	
			send(client_socket, sendline, strlen(sendline), 0);			
#endif						
		}

		//notify
		if (strstr(buf, "notify/") != NULL)	
		{
			if (true == userHobbyDetectionEnabled)
			{
				if (Hold == status)
				{
					GetPlayerHoldActionFromNotify(buf);
				}
				if (needCheckInFlop == true)
				{
					GetPlayerHoldActionFromFlopNotify(buf);
					needCheckInFlop = false;
				}
			}																												
		}

		if (strstr(buf, "game-over"))	
		{
			#if Linux
			close(sockfd);  
			#endif
		    exit(0);  
		}
	}
	#if Linux
    close(sockfd);  
	#endif
    exit(0);  
	return 0;
}


bool isIncluded(int buttonIndex,int ownIndex,int holdFirstActionIndex)
{
	bool needCheckInFlop;
	if (buttonIndex < holdFirstActionIndex)
	{
		if((buttonIndex < ownIndex) && (ownIndex < holdFirstActionIndex))
		{
			needCheckInFlop = true;
		}
		else
		{
			needCheckInFlop = false;
		}
	}
	else
	{					
		if ((holdFirstActionIndex <= ownIndex) && (ownIndex <= buttonIndex))
		{
			needCheckInFlop = false;
		}
		else
		{
			needCheckInFlop = true;
		}
	}
	return needCheckInFlop;
} 

/*
** ��ȡÿһ�ָտ�ʼʱ�ĸ����û���Ϣ
** �ú�����Ϊ�˸�AllUserBehavior�ĸ���EachUserBehavior�ṩ��Ϣ����ΪAllUserBehavior�ṩ������Ϣ
*/
// void GetInitialUserInfo_UserBehavior(char *buf, char *ownerID)
// {
// 	static char *pSeatInfo[10];	
// 	static int mallocFlag = 0;
// 	int i;
// 	
// 	if (mallocFlag == 0)
// 	{
// 		mallocFlag = 1;									// �����ڴ�ռ����ڱ�������û���Ϣ��ֻ����һ��
// 		for (i = 0; i < 10; i++)	pSeatInfo[i] = (char *)malloc(sizeof(char) * 100);
// 	}
// 	char *bufTemp, *bufTempNext;
// 	bufTemp = strstr(buf, "seat/");
// 	int index = 0;
// 
// 	// �ԡ�\n��Ϊ�籣������û�����Ϣ��һϵ��pSeatInfo��ָ��������
// 	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)//��ȡ\n��λ��
// 	{
// 		memset(pSeatInfo[index], 0, 100);
// 		strncpy(pSeatInfo[index], bufTemp,bufTempNext - bufTemp);//��bufTemp��ʼ����ȡ���ɸ��ֽڣ��ṩ��pSeatInfo[index]
// 		*(pSeatInfo[index] + (bufTempNext - bufTemp)) = '\0';//ΪpSeatInfo[index]�ý�����
// 		if (strstr(pSeatInfo[index], "/seat") != NULL)	// ��ʱ�Ѿ���ȡ��ϣ��˳�
// 			break;
// 
// 		bufTemp = bufTempNext + 1;
// 		index++;										// ��ʾ�Ѿ���ȡ���û���
// 	}
// 	
// 	allUserBehavior.allUserCount = index - 1;				// ������Ч���û�������û������ģ�
// 	for (i = 1; i < index; i++)
// 	{
// 		allUserBehavior.allUser[i-1].
// 		allUserInfo.allUser[i-1].isValid = true;		// �տ�ʼʱ�����û���Ϊ��Ч����û�����ƣ�
// 		
// 		if (strstr(pSeatInfo[i], "button") != NULL)		// ׯ����Ϣ
// 		{
// 			sscanf(pSeatInfo[i], "button: %s %d %d",	// ��Ϣ�����û�ID���ڸþֵĳ��롢ʣ�µĽ�Һ͵�ǰѺע
// 				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
// 				&allUserInfo.allUser[i-1].money);		
// 			allUserInfo.allUser[i-1].bet = 0;			
// 		}												// Сäע��Ϣ
// 		else if (strstr(pSeatInfo[i], "small blind") != NULL)
// 		{
// 			sscanf(pSeatInfo[i], "small blind: %s %d %d",
// 				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
// 				&allUserInfo.allUser[i-1].money);
// 			//allUserInfo.allUser[i-1].bet = 50;
// 			allUserInfo.allUser[i-1].bet = BlindBet / 2;
// 		}												// ��äע��Ϣ
// 		else if (strstr(pSeatInfo[i], "big blind") != NULL)
// 		{
// 			sscanf(pSeatInfo[i], "big blind: %s %d %d",
// 				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
// 				&allUserInfo.allUser[i-1].money);
// 			//allUserInfo.allUser[i-1].bet = 100;
// 			allUserInfo.allUser[i-1].bet = BlindBet;
// 		}
// 		else											// ������ҵ���Ϣ
// 		{
// 			sscanf(pSeatInfo[i], "%s %d %d",
// 				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
// 				&allUserInfo.allUser[i-1].money);
// 			allUserInfo.allUser[i-1].bet = 0;
// 		}
// 	}
// 	
// 	for (i = 0; i < index - 1; i++)						// �û��Լ���Ӧ��һ����Ϣ
// 	{
// 		if (strcmp(allUserInfo.allUser[i].userID, ownerID) == 0)
// 		{
// 			allUserInfo.ownIndex = i;
// 			break;
// 		}
// 	}
// 	
// }

/*
** ��ȡÿһ�ָտ�ʼʱ�ĸ����û���Ϣ
*/
void GetInitialUserInfo(char *buf, char *ownerID)
{
	static char *pSeatInfo[10];	
	static int mallocFlag = 0;
	int i;

	if (mallocFlag == 0)
	{
		mallocFlag = 1;									// �����ڴ�ռ����ڱ�������û���Ϣ��ֻ����һ��
		for (i = 0; i < 10; i++)	pSeatInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "seat/");
	int index = 0;
														// �ԡ�\n��Ϊ�籣������û�����Ϣ
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pSeatInfo[index], 0, 100);
		strncpy(pSeatInfo[index], bufTemp,bufTempNext - bufTemp);
		*(pSeatInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pSeatInfo[index], "/seat") != NULL)	// ��ʱ�Ѿ���ȡ��ϣ��˳�
			break;
		bufTemp = bufTempNext + 1;
		index++;										// ��ʾ�Ѿ���ȡ���û���
	}

	allUserInfo.allUserCount = index - 1;				// �ܹ����û����������Ѿ�����ĺ�û������ģ�
	allUserInfo.validUserCount = index - 1;				// ������Ч���û�������û������ģ�
	for (i = 1; i < index; i++)
	{
		allUserInfo.allUser[i-1].isValid = true;		// �տ�ʼʱ�����û���Ϊ��Ч����û�����ƣ�

		if (strstr(pSeatInfo[i], "button") != NULL)		// ׯ����Ϣ
		{
			sscanf(pSeatInfo[i], "button: %s %d %d",	// ��Ϣ�����û�ID���ڸþֵĳ��롢ʣ�µĽ�Һ͵�ǰѺע
			allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
			&allUserInfo.allUser[i-1].money);		
			allUserInfo.allUser[i-1].bet = 0;			
		}												// Сäע��Ϣ
		else if (strstr(pSeatInfo[i], "small blind") != NULL)
		{
			sscanf(pSeatInfo[i], "small blind: %s %d %d",
			allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
			&allUserInfo.allUser[i-1].money);
			//allUserInfo.allUser[i-1].bet = 50;
			allUserInfo.allUser[i-1].bet = BlindBet / 2;
		}												// ��äע��Ϣ
		else if (strstr(pSeatInfo[i], "big blind") != NULL)
		{
			sscanf(pSeatInfo[i], "big blind: %s %d %d",
			allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
			&allUserInfo.allUser[i-1].money);
			//allUserInfo.allUser[i-1].bet = 100;
			allUserInfo.allUser[i-1].bet = BlindBet;
		}
		else											// ������ҵ���Ϣ
		{
			sscanf(pSeatInfo[i], "%s %d %d",
			allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
			&allUserInfo.allUser[i-1].money);
			allUserInfo.allUser[i-1].bet = 0;
		}
	}

	for (i = 0; i < index - 1; i++)						// �û��Լ���Ӧ��һ����Ϣ
	{
		if (strcmp(allUserInfo.allUser[i].userID, ownerID) == 0)
		{
			allUserInfo.ownIndex = i;
			break;
		}
	}

	allUserInfo.holdRaised  = false;	// ÿ�ֿ�ʼʱ��Ϊfalse����ʾ��Ȧû�м�ע
	allUserInfo.flopRaised  = false;
	allUserInfo.turnRaised  = false;
	allUserInfo.riverRaised = false;
}


//ֻ��ȡһ�Σ���ȡ������
//��ȡ����(600��)��ʼʱ������(��ʽ������Ϊ8)���������û�IDд�������У���ȡ������index
void GetInitialRoundUserBehavior_UserID(char *buf, char *ownerID)
{
	static char *pSeatInfo[10];	
	static int mallocFlag = 0;
	int i;
	
	if (mallocFlag == 0)
	{
		mallocFlag = 1;									// �����ڴ�ռ����ڱ�������û���Ϣ��ֻ����һ��
		for (i = 0; i < 10; i++)	pSeatInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "seat/");
	int index = 0;
	// �ԡ�\n��Ϊ�籣������û�����Ϣ
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{		
		memset(pSeatInfo[index], 0, 100);
		strncpy(pSeatInfo[index], bufTemp,bufTempNext - bufTemp);
		*(pSeatInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pSeatInfo[index], "/seat") != NULL)	// ��ʱ�Ѿ���ȡ��ϣ��˳�
			break;
		bufTemp = bufTempNext + 1;
		index++;										// ��ʾ�Ѿ���ȡ���û���
	}
	
	allUserBehavior.allUserCount = index - 1;			// ��Ϸ�տ�ʼʱ���û���	

	for (i = 1; i < index; i++)
	{		
		if (strstr(pSeatInfo[i], "button") != NULL)		// ׯ����Ϣ
		{
			sscanf(pSeatInfo[i], "button: %s", allUserBehavior.allUser[i-1].userID);
		}												// Сäע��Ϣ
		else if (strstr(pSeatInfo[i], "small blind") != NULL)
		{
			sscanf(pSeatInfo[i], "small blind: %s", allUserBehavior.allUser[i-1].userID);
		}												// ��äע��Ϣ
		else if (strstr(pSeatInfo[i], "big blind") != NULL)
		{
			sscanf(pSeatInfo[i], "big blind: %s", allUserBehavior.allUser[i-1].userID);
		}
		else											// ������ҵ���Ϣ
		{
			sscanf(pSeatInfo[i], "%s", allUserBehavior.allUser[i-1].userID);
		}
		if (strcmp(allUserBehavior.allUser[i-1].userID,ownerID) == 0)
		{
			allUserBehavior.ownIndex = i - 1;
		}
	}
}

/*
** ��ȡÿ�ָտ�ʼʱ���û����������Ƿ���������ȡׯ�ҵ�Index
*/
void GetEveryRoundUserBehavior_UserID(char *buf)
{
	static char *pSeatInfo[10];	
	static int mallocFlag = 0;
	int i;
	
	if (mallocFlag == 0)
	{
		mallocFlag = 1;									// �����ڴ�ռ����ڱ�������û���Ϣ��ֻ����һ��
		for (i = 0; i < 10; i++)	pSeatInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "seat/");
	int index = 0;
	// �ԡ�\n��Ϊ�籣������û�����Ϣ
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{		
		memset(pSeatInfo[index], 0, 100);
		strncpy(pSeatInfo[index], bufTemp,bufTempNext - bufTemp);
		*(pSeatInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pSeatInfo[index], "/seat") != NULL)	// ��ʱ�Ѿ���ȡ��ϣ��˳�
			break;
		bufTemp = bufTempNext + 1;
		index++;										// ��ʾ�Ѿ���ȡ���û���
	}
	allUserBehavior.roundUserCount = index - 1;			//������Ϸ�տ�ʼʱ���û���

	if (allUserBehavior.roundUserCount <= 2)
	{
		userHobbyDetectionEnabled = false;		// ��ʱ���ٸ����û���ϰ���ж�����
	}
			
	for (int j = 0; j < allUserBehavior.allUserCount; j++)
	{
		allUserBehavior.allUser[j].isOver = true;	// �������û���ΪisOver
	}

	bool isButtonFound = false;
	bool isBigBlindFound = false;
	bool isNextBigBlindFound= false;
	for (i = 1; i < index; i++)
	{
		char tempUserID[20];
		if (strstr(pSeatInfo[i], "button") != NULL)		// ׯ����Ϣ
		{
			sscanf(pSeatInfo[i], "button: %s", tempUserID);
			isButtonFound = true;
		}												// Сäע��Ϣ
		else if (strstr(pSeatInfo[i], "small blind") != NULL)
		{
			sscanf(pSeatInfo[i], "small blind: %s",	tempUserID);
		}												// ��äע��Ϣ
		else if (strstr(pSeatInfo[i], "big blind") != NULL)
		{
			sscanf(pSeatInfo[i], "big blind: %s", tempUserID);
			isBigBlindFound = true;
		}
		else											// ������ҵ���Ϣ
		{
			if (true == isBigBlindFound)
			{
				isNextBigBlindFound = true;
				isBigBlindFound = false;
			}
			sscanf(pSeatInfo[i], "%s", tempUserID);
		}

		//tempUserID
		int j;
		for (j = 0; j < allUserBehavior.allUserCount; j++)
		{
			if (strcmp(allUserBehavior.allUser[j].userID, tempUserID) == 0)		// �ҵ���Ӧ��ID��
			{	
				break;						
			}
		}
		allUserBehavior.allUser[j].isOver = false;
		if (isButtonFound)
		{
			allUserBehavior.buttonIndex = j;
			isButtonFound = false;
		}
		if (isNextBigBlindFound)
		{
			allUserBehavior.holdFirstActionIndex = j;
			isNextBigBlindFound = false;
		}
	}	
}

void GetPlayerHoldActionFromFlopInquire(char *buf)
{
	static char *pInquireInfo[15];   
	static int mallocFlag = 0;
	int i;
	
	if (mallocFlag == 0)
	{
		mallocFlag = 1;
		for (i = 0; i < 15; i++)						// �����ڴ�ռ����ڱ�������û���ǰ����ע��Ϣ��ֻ����һ��
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "inquire/");
	int index = 0;
	// �ԡ�\n��Ϊ�籣������û��ĵ�ǰ��ע��Ϣ
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/inquire") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// ��ʾ�Ѿ���ȡ����Ч�û���עָ�����������Ϣ
	}
	for (i = 1; i < index-1; i++)
	{	
		char userID[20];
		int userJetton,userMoney,userBet;
		char userAction[10];
		sscanf(pInquireInfo[i], "%s%d%d%d%s", userID,&userJetton, 
			&userMoney, &userBet,userAction);
		
		//�ҵ�userID��allUserBehavior.allUser[]�ж�Ӧ��index
		int j;
		for (j = 0; j < allUserBehavior.allUserCount; j++)
		{
			if (strcmp(allUserBehavior.allUser[j].userID, userID) == 0)		// �ҵ���Ӧ��ID��
			{	
				break;						
			}
		}
		
		//��¼��ѡ�ֵĶ���
		if (isIncluded(allUserBehavior.ownIndex, j, allUserBehavior.holdFirstActionIndex) == true)
		{		
			strcpy(allUserBehavior.allUser[j].holdAction, userAction);
		}
				
		//����ö���Ϊ��ע�����¼��index
		if (strcmp(userAction,"raise") == 0)
		{
			allUserBehavior.holdFirstActionIndex = j;
		}		
	}
}

void GetPlayerHoldActionFromFlopNotify(char *buf)
{
	static char *pInquireInfo[15];   
	static int mallocFlag = 0;
	int i;
	
	if (mallocFlag == 0)
	{
		mallocFlag = 1;
		for (i = 0; i < 15; i++)						// �����ڴ�ռ����ڱ�������û���ǰ����ע��Ϣ��ֻ����һ��
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "notify/");
	int index = 0;
	// �ԡ�\n��Ϊ�籣������û��ĵ�ǰ��ע��Ϣ
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/notify") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// ��ʾ�Ѿ���ȡ����Ч�û���עָ�����������Ϣ
	}
	for (i = 1; i < index-1; i++)
	{	
		char userID[20];
		int userJetton,userMoney,userBet;
		char userAction[10];
		sscanf(pInquireInfo[i], "%s%d%d%d%s", userID,&userJetton, 
			&userMoney, &userBet,userAction);
		
		//�ҵ�userID��allUserBehavior.allUser[]�ж�Ӧ��index
		int j;
		for (j = 0; j < allUserBehavior.allUserCount; j++)
		{
			if (strcmp(allUserBehavior.allUser[j].userID, userID) == 0)		// �ҵ���Ӧ��ID��
			{	
				break;						
			}
		}
	
		//��¼��ѡ�ֵĶ���
		if (isIncluded(allUserBehavior.ownIndex,j,allUserBehavior.holdFirstActionIndex) == true)
		{		
			strcpy(allUserBehavior.allUser[j].holdAction,userAction);
		}
		
		//����ö���Ϊ��ע�����¼��index
		if (strcmp(userAction, "raise") == 0)
		{
			allUserBehavior.holdFirstActionIndex = j;
		}	
	}
}

void GetPlayerHoldActionFromNotify(char *buf)
{
	static char *pInquireInfo[15];   
	static int mallocFlag = 0;
	int i;
	
	if (mallocFlag == 0)
	{
		mallocFlag = 1;
		for (i = 0; i < 15; i++)						// �����ڴ�ռ����ڱ�������û���ǰ����ע��Ϣ��ֻ����һ��
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "notify/");
	int index = 0;
	// �ԡ�\n��Ϊ�籣������û��ĵ�ǰ��ע��Ϣ
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/notify") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// ��ʾ�Ѿ���ȡ����Ч�û���עָ�����������Ϣ
	}
	for (i = 1; i < index-1; i++)
	{	
		char userID[20];
		int userJetton,userMoney,userBet;
		char userAction[10];
		sscanf(pInquireInfo[i], "%s%d%d%d%s", userID,&userJetton, 
			&userMoney, &userBet,userAction);
		
		//�ҵ�userID��allUserBehavior.allUser[]�ж�Ӧ��index
		int j;
		for (j = 0; j < allUserBehavior.allUserCount; j++)
		{
			if (strcmp(allUserBehavior.allUser[j].userID, userID) == 0)		// �ҵ���Ӧ��ID��
			{	
				break;						
			}
		}
		//��¼��ѡ�ֵĶ���
		strcpy(allUserBehavior.allUser[j].holdAction,userAction);
		
		//����ö���Ϊ��ע�����¼��index
		if (strcmp(userAction,"raise") == 0)
		{
			allUserBehavior.holdFirstActionIndex = j;
		}		
	}
}

void GetPlayerHoldActionFromInquire(char *buf)
{
	static char *pInquireInfo[15];   
	static int mallocFlag = 0;
	int i;
	
	if (mallocFlag == 0)
	{
		mallocFlag = 1;
		for (i = 0; i < 15; i++)						// �����ڴ�ռ����ڱ�������û���ǰ����ע��Ϣ��ֻ����һ��
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "inquire/");
	int index = 0;
	// �ԡ�\n��Ϊ�籣������û��ĵ�ǰ��ע��Ϣ
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/inquire") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// ��ʾ�Ѿ���ȡ����Ч�û���עָ�����������Ϣ
	}
	for (i = 1; i < index - 1; i++)
	{	
		char userID[20];
		int userJetton,userMoney,userBet;
		char userAction[10];
		sscanf(pInquireInfo[i], "%s%d%d%d%s", userID, &userJetton, 
				&userMoney, &userBet, userAction);

		//�ҵ�userID��allUserBehavior.allUser[]�ж�Ӧ��index
		int j;
		for (j = 0; j < allUserBehavior.allUserCount; j++)
		{
			if (strcmp(allUserBehavior.allUser[j].userID, userID) == 0)		// �ҵ���Ӧ��ID��
			{	
				break;						
			}
		}
		//��¼��ѡ�ֵĶ���
		strcpy(allUserBehavior.allUser[j].holdAction, userAction);
		
		//����ö���Ϊ��ע�����¼��index
		if (strcmp(userAction, "raise") == 0)
		{
			allUserBehavior.holdFirstActionIndex = j;
		}		
	}
}

/*
** ��ȡ�����û�Ѻע����Ϣ������Ӧ��ָ��
*/
void GetActionCommand(char *buf, char *allCard, char *actionCmd)
{
	Status4Inquire status4Inqu;
	static char *pInquireInfo[15];   
	static int mallocFlag = 0;
	int i;

	if (mallocFlag == 0)
	{
		mallocFlag = 1;
		for (i = 0; i < 15; i++)						// �����ڴ�ռ����ڱ�������û���ǰ����ע��Ϣ��ֻ����һ��
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "inquire/");
	int index = 0;
														// �ԡ�\n��Ϊ�籣������û��ĵ�ǰ��ע��Ϣ
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/inquire") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// ��ʾ�Ѿ���ȡ����Ч�û���עָ�����������Ϣ
	}
	
	char latestUserID[10];
	int  latestUserJetton, latestUserMoney, latestUserBet;
	char latestUserAction[10];

	for (i = 1; i < index; i++)
	{														
		if (strstr(pInquireInfo[i], "total") != NULL)	// �Ѷ���������Ϣ
		{
			status4Inqu = Allout;						// �����˾�����
			break;
		}												// ��ǰ�û����Ӵ�äע������߸��û�ǰ��ʹ�äע����֮����û�������
		else if(strstr(pInquireInfo[i], "blind") != NULL)
		{
			status4Inqu = Blind;				
			break;
		}												// ��ʾ���û��Ѿ�����
		else if(strstr(pInquireInfo[i], "fold") != NULL)
		{
			continue;
		}
		else  {											// �������
			sscanf(pInquireInfo[i], "%s%d%d%d%s", latestUserID,&latestUserJetton, 
			&latestUserMoney, &latestUserBet,latestUserAction);
			status4Inqu = Handle;						// ��Ҫ�������
			break;
		}
	}

	allUserInfo.validUserCount = 0;
	allUserInfo.foldUserCount = 0;
	latestUserBet = 0;

	for (i = 1; i < index; i++)
	{
		if(strstr(pInquireInfo[i], "blind") != NULL)		// äע
		{
			if(strstr(pInquireInfo[i], "20") != NULL)		// Сäע
			{
				char tempUserID[10];
				sscanf(pInquireInfo[i], "%s", tempUserID);	// �û�ID��
				for (int j = 0; j < allUserInfo.allUserCount; j++)
				{
					if (strcmp(allUserInfo.allUser[j].userID, tempUserID) == 0)		// �ҵ���Ӧ��ID��
					{	
						sscanf(pInquireInfo[i], "%s%d%d%d", allUserInfo.allUser[j].userID, &allUserInfo.allUser[j].jetton,
							&allUserInfo.allUser[j].money, &allUserInfo.allUser[j].bet);
						
						if (allUserInfo.allUser[j].bet > latestUserBet)		// ��ȡ��ǰѺעȦ����Ѻע��
							latestUserBet = allUserInfo.allUser[j].bet;

						allUserInfo.allUser[j].isValid = true;
						allUserInfo.validUserCount += 1;
						break;						
					}
				}
			}
			else 
			{											// ��äע
				char tempUserID[10];
				sscanf(pInquireInfo[i], "%s", tempUserID);	// �û�ID��
				for (int j = 0; j < allUserInfo.allUserCount; j++)
				{
					if (strcmp(allUserInfo.allUser[j].userID, tempUserID) == 0)		// �ҵ���Ӧ��ID��
					{	
						sscanf(pInquireInfo[i], "%s%d%d%d", allUserInfo.allUser[j].userID, &allUserInfo.allUser[j].jetton,
							&allUserInfo.allUser[j].money, &allUserInfo.allUser[j].bet);		
						
						if (allUserInfo.allUser[j].bet > latestUserBet)		// ��ȡ��ǰѺעȦ����Ѻע��
							latestUserBet = allUserInfo.allUser[j].bet;

						allUserInfo.allUser[j].isValid = true;
						allUserInfo.validUserCount += 1;
						break;						
					}
				}
			}
		}
		else if (strstr(pInquireInfo[i], "fold") != NULL)		// ����
		{
			char tempUserID[10];
			sscanf(pInquireInfo[i], "%s", tempUserID);	// �û�ID��
			for (int j = 0; j < allUserInfo.allUserCount; j++)
			{
				if (strcmp(allUserInfo.allUser[j].userID, tempUserID) == 0)		// �ҵ���Ӧ��ID��
				{	
					sscanf(pInquireInfo[i], "%s%d%d%d", allUserInfo.allUser[j].userID, &allUserInfo.allUser[j].jetton,
						&allUserInfo.allUser[j].money, &allUserInfo.allUser[j].bet);	
					
					if (allUserInfo.allUser[j].bet > latestUserBet)		// ��ȡ��ǰѺעȦ����Ѻע��
						latestUserBet = allUserInfo.allUser[j].bet;

					allUserInfo.allUser[j].isValid = false;
					//allUserInfo.validUserCount += 1;
					allUserInfo.foldUserCount += 1;
					break;						
				}				
			}
		}
		else if (strstr(pInquireInfo[i], "pot") != NULL)		// �����ܶ�
		{
			sscanf(pInquireInfo[i], "total pot: %d", &allUserInfo.totalPot);
		}
		else			// ��������� ���� check��call��raise��
		{
			char tempUserID[10];
			sscanf(pInquireInfo[i], "%s", tempUserID);			// �û�ID��
			for (int j = 0; j < allUserInfo.allUserCount; j++)
			{
				if (strcmp(allUserInfo.allUser[j].userID, tempUserID) == 0)		// �ҵ���Ӧ��ID��
				{	
					sscanf(pInquireInfo[i], "%s%d%d%d", allUserInfo.allUser[j].userID, &allUserInfo.allUser[j].jetton,
						&allUserInfo.allUser[j].money, &allUserInfo.allUser[j].bet);	
					
					if (allUserInfo.allUser[j].bet > latestUserBet)		// ��ȡ��ǰѺעȦ����Ѻע��
						latestUserBet = allUserInfo.allUser[j].bet;

					allUserInfo.allUser[j].isValid = true;
					allUserInfo.validUserCount += 1;
					break;						
				}
			}
		}
	}

	switch(status)
	{			
		case Hold:										// ����Ѻע�׶�
		{
			//strcpy(actionCmd, " check \n");
			GetActionCommandWithHold(status4Inqu, latestUserBet, actionCmd, allCard);
			break;
		}			
		case Flop:	
		{
			//strcpy(actionCmd, " check \n");
			GetActionCommandWithFlop(status4Inqu, latestUserBet, actionCmd, allCard);				
			break;
		}
		case Turn:
		{
			//strcpy(actionCmd, " check \n");
			GetActionCommandWithTurn(status4Inqu, latestUserBet, actionCmd, allCard);							
			break;
		}
		case River:
		{
			//strcpy(actionCmd, " check \n");
			GetActionCommandWithRiver(status4Inqu, latestUserBet, actionCmd, allCard);				
			break;
		}
		default:
		{
			strcpy(actionCmd, " check \n");
			break;
		}
	}
}

/*
** �ж��������Ƶ�����
*/
HoldType GetHoldCardType(char holdData[], char cbCardCount)
{
	char cbCardData[2] = {holdData[0], holdData[1]};

	// ���������ư������Ӵ�С����
	SortCardList(cbCardData, 2);

	// �����Ƶ������
	if ((cbCardData[0] & 0x0F) == (cbCardData[1] & 0x0F))
	{
		if ((cbCardData[0] & 0x0F) >= 0x02 &&	// ��2~Q��Χ�ڶ���ΪС����
			(cbCardData[0] & 0x0F) <= 0x0C)
			return LowPair;
		else
			return HighPair;					// ������2~Q��Χ�ڶ���Ϊ�����
	}
	else if ((cbCardData[0] & 0x0F) == 0x01)	// ��һ��A�����
	{
		return HighCardA;
	}											// ��������ɫһ��
	else if ((cbCardData[0] & 0x0F) >= 0x0A && (cbCardData[1] & 0x0F) >= 0x0A)
	{
		return MiddleCardKQJT;					// �������ٴ��ڵ���10
	}
	else if (  (cbCardData[0] & 0x0F) >=0x0C  )//��һ��K����Q����������������������������Ȼ�������ȼ���
	{
		return HighCardKQ;
	}
	else if(   (cbCardData[1] & 0x0F) >= 0x07  )//С�ƴ��ڵ���7
	{
		return LowCard7;


	}
	
// 	else if((cbCardData[0] & 0xF0) == (cbCardData[1] & 0xF0))
// 	{
// 		return SameColor;
// 	}
	else
		return Other;							// ����������ֵ��������
}

void GetAllUserBahaviorDangerScore(char *buf)
{
	static char *pInquireInfo[15];   
	static int mallocFlag = 0;
	int i, ii;
	char tempUserID[10];

	if (mallocFlag == 0)
	{
		mallocFlag = 1;
		for (i = 0; i < 15; i++)						// �����ڴ�ռ����ڱ�������û���ǰ����ע��Ϣ��ֻ����һ��
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "inquire/");
	int index = 0;
														// �ԡ�\n��Ϊ�籣������û��ĵ�ǰ��ע��Ϣ
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/inquire") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// ��ʾ�Ѿ���ȡ����Ч�û���עָ�����������Ϣ
	}

	for (ii = 0; ii < allUserBehavior.allUserCount; ii++)
	{
		if (allUserBehavior.allUser[ii].isOver == false)
		{
			allUserBehavior.allUser[ii].isFold = false;
		}
	}
	
	for (i = 1; i < index; i++)
	{
		bool isFold = false;
		if(strstr(pInquireInfo[i], "blind") != NULL)		// äע
		{
			if(strstr(pInquireInfo[i], "20") != NULL)		// Сäע
			{
				sscanf(pInquireInfo[i], "%s", tempUserID);	// �û�ID��
				isFold = false;
			}
			else 
			{											// ��äע
				sscanf(pInquireInfo[i], "%s", tempUserID);	// �û�ID��
				isFold = false;
			}
		}
		else if (strstr(pInquireInfo[i], "fold") != NULL)		// ����
		{
			sscanf(pInquireInfo[i], "%s", tempUserID);	// �û�ID��
			isFold =true;
		}
		else if (strstr(pInquireInfo[i], "pot") != NULL)		// �����ܶ�
		{
			/*sscanf(pInquireInfo[i], "total pot: %d", &allUserInfo.totalPot);*/
			break;
		}
		else			// ��������� ���� check��call��raise��
		{
			sscanf(pInquireInfo[i], "%s", tempUserID);			// �û�ID��
			isFold  = false;
		}

		//tempUserID
		int j;
		for (j = 0; j < allUserBehavior.allUserCount; j++)
		{
			if (strcmp(allUserBehavior.allUser[j].userID, tempUserID) == 0)		// �ҵ���Ӧ��ID��
			{	
				break;						
			}
		}
		if (isFold)
		{
			allUserBehavior.allUser[j].isFold = true;
		}
		else
		{
			allUserBehavior.allUser[j].isFold = false;
		}
	}

	allUserBehavior.totalDangerScore = 0;
	for (ii = 0; ii < allUserBehavior.allUserCount; ii++)
	{
		if (allUserBehavior.allUser[ii].isOver == false)
		{
			if (allUserBehavior.allUser[ii].isFold == false)
			{
				allUserBehavior.totalDangerScore += allUserBehavior.allUser[ii].dangerScore;
			}
		}
	}

	if (allUserBehavior.totalDangerScore >= 4)
	{
		allUserBehavior.needConser = true;
	}
	else
	{
		allUserBehavior.needConser = false;
	}
}

