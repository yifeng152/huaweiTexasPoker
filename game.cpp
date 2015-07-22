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
#define MAXLINE 4096  // 原来的值为300 太小了 导致有时候会出现乱码 2015-05-17
					  // 注意："raise num\n"中raise前面一定不能有空格

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

//在hold_inquire.cpp中
extern bool scareEnabled;
extern bool scareFailed;
extern int scareTryNum;		//进行唬人的总次数
extern int scareFailedNum;	//唬人成功的总次数
FILE *fp4ScarePlan;


bool getInitialRoundUserBehaviorDone = false;
bool userHobbyDetectionEnabled = true;		//初始时，默认可以进行用户习惯检测。当发现人数小于等于3时，关闭该标志
bool needCheckInFlop = false;				//该标志位true，代表需要检测flop中的inquire或者notify
bool flopNotComes;							//flop阶段还未来临
bool flopBegins;							//初次进入flop阶段


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

	// 设置socket选项，地址重复使用，防止程序非正常退出，下次启动时bind失败  linaijun 2015-05-26
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

			scareEnabled = false;		// 重置唬人使能标志位
			scareFailed = false;		// 重置唬人失败标志位
			
			allUserInfo.currentRound += 1;	// 当前进行的局数加1
			allUserBehavior.currentRound += 1;

 			GetInitialUserInfo(buf, argv[5]);				// 获取每一局刚开始时的各个用户信息
		
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
			GetHoldCard(head4Hold, holdCard);		// 获取手牌
			strncpy(allCard, holdCard, 2);
		}
		else if ((head4Flop = strstr(buf, "flop")) != NULL)	
		{
			status = Flop;
			GetFlopCard(head4Flop, flopCard);		// 获取公牌
			strncpy(allCard + 2, flopCard, 3);
		}
		else if ((head4Turn = strstr(buf, "turn")) != NULL)	
		{
			status = Turn;
			GetTurnCard(head4Turn, turnCard);		// 获取转牌
			strncpy(allCard + 5, turnCard, 1);
		}
		else if ((head4River = strstr(buf, "river")) != NULL)	
		{
			status = River;
			GetRiverCard(head4River, riverCard);	// 获取河牌
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
				//判断是否需要查看flop阶段的inquire或者notify
				//当ownIndex在buttonIndex与holdFirstActionIndex之间时需要对flop手牌进行判断
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
			char actionCmd[20];						// 获取询问时的作答指令
			
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
** 获取每一局刚开始时的各个用户信息
** 该函数是为了给AllUserBehavior的各个EachUserBehavior提供信息，并为AllUserBehavior提供部分信息
*/
// void GetInitialUserInfo_UserBehavior(char *buf, char *ownerID)
// {
// 	static char *pSeatInfo[10];	
// 	static int mallocFlag = 0;
// 	int i;
// 	
// 	if (mallocFlag == 0)
// 	{
// 		mallocFlag = 1;									// 分配内存空间用于保存各个用户信息，只分配一次
// 		for (i = 0; i < 10; i++)	pSeatInfo[i] = (char *)malloc(sizeof(char) * 100);
// 	}
// 	char *bufTemp, *bufTempNext;
// 	bufTemp = strstr(buf, "seat/");
// 	int index = 0;
// 
// 	// 以“\n”为界保存各个用户的信息到一系列pSeatInfo所指的数组中
// 	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)//获取\n的位置
// 	{
// 		memset(pSeatInfo[index], 0, 100);
// 		strncpy(pSeatInfo[index], bufTemp,bufTempNext - bufTemp);//从bufTemp开始，读取若干个字节，提供给pSeatInfo[index]
// 		*(pSeatInfo[index] + (bufTempNext - bufTemp)) = '\0';//为pSeatInfo[index]置结束符
// 		if (strstr(pSeatInfo[index], "/seat") != NULL)	// 此时已经读取完毕，退出
// 			break;
// 
// 		bufTemp = bufTempNext + 1;
// 		index++;										// 表示已经读取的用户数
// 	}
// 	
// 	allUserBehavior.allUserCount = index - 1;				// 所有有效的用户数（即没有输完的）
// 	for (i = 1; i < index; i++)
// 	{
// 		allUserBehavior.allUser[i-1].
// 		allUserInfo.allUser[i-1].isValid = true;		// 刚开始时各个用户均为有效（即没有弃牌）
// 		
// 		if (strstr(pSeatInfo[i], "button") != NULL)		// 庄家信息
// 		{
// 			sscanf(pSeatInfo[i], "button: %s %d %d",	// 信息包括用户ID、在该局的筹码、剩下的金币和当前押注
// 				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
// 				&allUserInfo.allUser[i-1].money);		
// 			allUserInfo.allUser[i-1].bet = 0;			
// 		}												// 小盲注信息
// 		else if (strstr(pSeatInfo[i], "small blind") != NULL)
// 		{
// 			sscanf(pSeatInfo[i], "small blind: %s %d %d",
// 				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
// 				&allUserInfo.allUser[i-1].money);
// 			//allUserInfo.allUser[i-1].bet = 50;
// 			allUserInfo.allUser[i-1].bet = BlindBet / 2;
// 		}												// 大盲注信息
// 		else if (strstr(pSeatInfo[i], "big blind") != NULL)
// 		{
// 			sscanf(pSeatInfo[i], "big blind: %s %d %d",
// 				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
// 				&allUserInfo.allUser[i-1].money);
// 			//allUserInfo.allUser[i-1].bet = 100;
// 			allUserInfo.allUser[i-1].bet = BlindBet;
// 		}
// 		else											// 其他玩家的信息
// 		{
// 			sscanf(pSeatInfo[i], "%s %d %d",
// 				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
// 				&allUserInfo.allUser[i-1].money);
// 			allUserInfo.allUser[i-1].bet = 0;
// 		}
// 	}
// 	
// 	for (i = 0; i < index - 1; i++)						// 用户自己对应哪一条信息
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
** 获取每一局刚开始时的各个用户信息
*/
void GetInitialUserInfo(char *buf, char *ownerID)
{
	static char *pSeatInfo[10];	
	static int mallocFlag = 0;
	int i;

	if (mallocFlag == 0)
	{
		mallocFlag = 1;									// 分配内存空间用于保存各个用户信息，只分配一次
		for (i = 0; i < 10; i++)	pSeatInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "seat/");
	int index = 0;
														// 以“\n”为界保存各个用户的信息
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pSeatInfo[index], 0, 100);
		strncpy(pSeatInfo[index], bufTemp,bufTempNext - bufTemp);
		*(pSeatInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pSeatInfo[index], "/seat") != NULL)	// 此时已经读取完毕，退出
			break;
		bufTemp = bufTempNext + 1;
		index++;										// 表示已经读取的用户数
	}

	allUserInfo.allUserCount = index - 1;				// 总共的用户数（包括已经输完的和没有输完的）
	allUserInfo.validUserCount = index - 1;				// 所有有效的用户数（即没有输完的）
	for (i = 1; i < index; i++)
	{
		allUserInfo.allUser[i-1].isValid = true;		// 刚开始时各个用户均为有效（即没有弃牌）

		if (strstr(pSeatInfo[i], "button") != NULL)		// 庄家信息
		{
			sscanf(pSeatInfo[i], "button: %s %d %d",	// 信息包括用户ID、在该局的筹码、剩下的金币和当前押注
			allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
			&allUserInfo.allUser[i-1].money);		
			allUserInfo.allUser[i-1].bet = 0;			
		}												// 小盲注信息
		else if (strstr(pSeatInfo[i], "small blind") != NULL)
		{
			sscanf(pSeatInfo[i], "small blind: %s %d %d",
			allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
			&allUserInfo.allUser[i-1].money);
			//allUserInfo.allUser[i-1].bet = 50;
			allUserInfo.allUser[i-1].bet = BlindBet / 2;
		}												// 大盲注信息
		else if (strstr(pSeatInfo[i], "big blind") != NULL)
		{
			sscanf(pSeatInfo[i], "big blind: %s %d %d",
			allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
			&allUserInfo.allUser[i-1].money);
			//allUserInfo.allUser[i-1].bet = 100;
			allUserInfo.allUser[i-1].bet = BlindBet;
		}
		else											// 其他玩家的信息
		{
			sscanf(pSeatInfo[i], "%s %d %d",
			allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
			&allUserInfo.allUser[i-1].money);
			allUserInfo.allUser[i-1].bet = 0;
		}
	}

	for (i = 0; i < index - 1; i++)						// 用户自己对应哪一条信息
	{
		if (strcmp(allUserInfo.allUser[i].userID, ownerID) == 0)
		{
			allUserInfo.ownIndex = i;
			break;
		}
	}

	allUserInfo.holdRaised  = false;	// 每局开始时置为false，表示该圈没有加注
	allUserInfo.flopRaised  = false;
	allUserInfo.turnRaised  = false;
	allUserInfo.riverRaised = false;
}


//只读取一次，获取不变量
//获取本轮(600局)开始时总人数(正式比赛即为8)，将各个用户ID写进数组中，获取本机的index
void GetInitialRoundUserBehavior_UserID(char *buf, char *ownerID)
{
	static char *pSeatInfo[10];	
	static int mallocFlag = 0;
	int i;
	
	if (mallocFlag == 0)
	{
		mallocFlag = 1;									// 分配内存空间用于保存各个用户信息，只分配一次
		for (i = 0; i < 10; i++)	pSeatInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "seat/");
	int index = 0;
	// 以“\n”为界保存各个用户的信息
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{		
		memset(pSeatInfo[index], 0, 100);
		strncpy(pSeatInfo[index], bufTemp,bufTempNext - bufTemp);
		*(pSeatInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pSeatInfo[index], "/seat") != NULL)	// 此时已经读取完毕，退出
			break;
		bufTemp = bufTempNext + 1;
		index++;										// 表示已经读取的用户数
	}
	
	allUserBehavior.allUserCount = index - 1;			// 游戏刚开始时的用户数	

	for (i = 1; i < index; i++)
	{		
		if (strstr(pSeatInfo[i], "button") != NULL)		// 庄家信息
		{
			sscanf(pSeatInfo[i], "button: %s", allUserBehavior.allUser[i-1].userID);
		}												// 小盲注信息
		else if (strstr(pSeatInfo[i], "small blind") != NULL)
		{
			sscanf(pSeatInfo[i], "small blind: %s", allUserBehavior.allUser[i-1].userID);
		}												// 大盲注信息
		else if (strstr(pSeatInfo[i], "big blind") != NULL)
		{
			sscanf(pSeatInfo[i], "big blind: %s", allUserBehavior.allUser[i-1].userID);
		}
		else											// 其他玩家的信息
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
** 获取每局刚开始时的用户总人数及是否死亡，获取庄家的Index
*/
void GetEveryRoundUserBehavior_UserID(char *buf)
{
	static char *pSeatInfo[10];	
	static int mallocFlag = 0;
	int i;
	
	if (mallocFlag == 0)
	{
		mallocFlag = 1;									// 分配内存空间用于保存各个用户信息，只分配一次
		for (i = 0; i < 10; i++)	pSeatInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "seat/");
	int index = 0;
	// 以“\n”为界保存各个用户的信息
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{		
		memset(pSeatInfo[index], 0, 100);
		strncpy(pSeatInfo[index], bufTemp,bufTempNext - bufTemp);
		*(pSeatInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pSeatInfo[index], "/seat") != NULL)	// 此时已经读取完毕，退出
			break;
		bufTemp = bufTempNext + 1;
		index++;										// 表示已经读取的用户数
	}
	allUserBehavior.roundUserCount = index - 1;			//该轮游戏刚开始时的用户数

	if (allUserBehavior.roundUserCount <= 2)
	{
		userHobbyDetectionEnabled = false;		// 此时不再根据用户的习惯判断命令
	}
			
	for (int j = 0; j < allUserBehavior.allUserCount; j++)
	{
		allUserBehavior.allUser[j].isOver = true;	// 将所有用户置为isOver
	}

	bool isButtonFound = false;
	bool isBigBlindFound = false;
	bool isNextBigBlindFound= false;
	for (i = 1; i < index; i++)
	{
		char tempUserID[20];
		if (strstr(pSeatInfo[i], "button") != NULL)		// 庄家信息
		{
			sscanf(pSeatInfo[i], "button: %s", tempUserID);
			isButtonFound = true;
		}												// 小盲注信息
		else if (strstr(pSeatInfo[i], "small blind") != NULL)
		{
			sscanf(pSeatInfo[i], "small blind: %s",	tempUserID);
		}												// 大盲注信息
		else if (strstr(pSeatInfo[i], "big blind") != NULL)
		{
			sscanf(pSeatInfo[i], "big blind: %s", tempUserID);
			isBigBlindFound = true;
		}
		else											// 其他玩家的信息
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
			if (strcmp(allUserBehavior.allUser[j].userID, tempUserID) == 0)		// 找到对应的ID号
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
		for (i = 0; i < 15; i++)						// 分配内存空间用于保存各个用户当前的下注信息，只分配一次
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "inquire/");
	int index = 0;
	// 以“\n”为界保存各个用户的当前下注信息
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/inquire") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// 表示已经读取的有效用户下注指令，包括奖池信息
	}
	for (i = 1; i < index-1; i++)
	{	
		char userID[20];
		int userJetton,userMoney,userBet;
		char userAction[10];
		sscanf(pInquireInfo[i], "%s%d%d%d%s", userID,&userJetton, 
			&userMoney, &userBet,userAction);
		
		//找到userID在allUserBehavior.allUser[]中对应的index
		int j;
		for (j = 0; j < allUserBehavior.allUserCount; j++)
		{
			if (strcmp(allUserBehavior.allUser[j].userID, userID) == 0)		// 找到对应的ID号
			{	
				break;						
			}
		}
		
		//记录该选手的动作
		if (isIncluded(allUserBehavior.ownIndex, j, allUserBehavior.holdFirstActionIndex) == true)
		{		
			strcpy(allUserBehavior.allUser[j].holdAction, userAction);
		}
				
		//如果该动作为加注，则记录该index
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
		for (i = 0; i < 15; i++)						// 分配内存空间用于保存各个用户当前的下注信息，只分配一次
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "notify/");
	int index = 0;
	// 以“\n”为界保存各个用户的当前下注信息
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/notify") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// 表示已经读取的有效用户下注指令，包括奖池信息
	}
	for (i = 1; i < index-1; i++)
	{	
		char userID[20];
		int userJetton,userMoney,userBet;
		char userAction[10];
		sscanf(pInquireInfo[i], "%s%d%d%d%s", userID,&userJetton, 
			&userMoney, &userBet,userAction);
		
		//找到userID在allUserBehavior.allUser[]中对应的index
		int j;
		for (j = 0; j < allUserBehavior.allUserCount; j++)
		{
			if (strcmp(allUserBehavior.allUser[j].userID, userID) == 0)		// 找到对应的ID号
			{	
				break;						
			}
		}
	
		//记录该选手的动作
		if (isIncluded(allUserBehavior.ownIndex,j,allUserBehavior.holdFirstActionIndex) == true)
		{		
			strcpy(allUserBehavior.allUser[j].holdAction,userAction);
		}
		
		//如果该动作为加注，则记录该index
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
		for (i = 0; i < 15; i++)						// 分配内存空间用于保存各个用户当前的下注信息，只分配一次
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "notify/");
	int index = 0;
	// 以“\n”为界保存各个用户的当前下注信息
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/notify") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// 表示已经读取的有效用户下注指令，包括奖池信息
	}
	for (i = 1; i < index-1; i++)
	{	
		char userID[20];
		int userJetton,userMoney,userBet;
		char userAction[10];
		sscanf(pInquireInfo[i], "%s%d%d%d%s", userID,&userJetton, 
			&userMoney, &userBet,userAction);
		
		//找到userID在allUserBehavior.allUser[]中对应的index
		int j;
		for (j = 0; j < allUserBehavior.allUserCount; j++)
		{
			if (strcmp(allUserBehavior.allUser[j].userID, userID) == 0)		// 找到对应的ID号
			{	
				break;						
			}
		}
		//记录该选手的动作
		strcpy(allUserBehavior.allUser[j].holdAction,userAction);
		
		//如果该动作为加注，则记录该index
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
		for (i = 0; i < 15; i++)						// 分配内存空间用于保存各个用户当前的下注信息，只分配一次
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "inquire/");
	int index = 0;
	// 以“\n”为界保存各个用户的当前下注信息
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/inquire") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// 表示已经读取的有效用户下注指令，包括奖池信息
	}
	for (i = 1; i < index - 1; i++)
	{	
		char userID[20];
		int userJetton,userMoney,userBet;
		char userAction[10];
		sscanf(pInquireInfo[i], "%s%d%d%d%s", userID, &userJetton, 
				&userMoney, &userBet, userAction);

		//找到userID在allUserBehavior.allUser[]中对应的index
		int j;
		for (j = 0; j < allUserBehavior.allUserCount; j++)
		{
			if (strcmp(allUserBehavior.allUser[j].userID, userID) == 0)		// 找到对应的ID号
			{	
				break;						
			}
		}
		//记录该选手的动作
		strcpy(allUserBehavior.allUser[j].holdAction, userAction);
		
		//如果该动作为加注，则记录该index
		if (strcmp(userAction, "raise") == 0)
		{
			allUserBehavior.holdFirstActionIndex = j;
		}		
	}
}

/*
** 获取各个用户押注的信息及返回应答指令
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
		for (i = 0; i < 15; i++)						// 分配内存空间用于保存各个用户当前的下注信息，只分配一次
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "inquire/");
	int index = 0;
														// 以“\n”为界保存各个用户的当前下注信息
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/inquire") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// 表示已经读取的有效用户下注指令，包括奖池信息
	}
	
	char latestUserID[10];
	int  latestUserJetton, latestUserMoney, latestUserBet;
	char latestUserAction[10];

	for (i = 1; i < index; i++)
	{														
		if (strstr(pInquireInfo[i], "total") != NULL)	// 已读到奖池信息
		{
			status4Inqu = Allout;						// 所有人均弃牌
			break;
		}												// 当前用户紧接大盲注后面或者该用户前面和大盲注后面之间的用户均弃牌
		else if(strstr(pInquireInfo[i], "blind") != NULL)
		{
			status4Inqu = Blind;				
			break;
		}												// 表示该用户已经弃牌
		else if(strstr(pInquireInfo[i], "fold") != NULL)
		{
			continue;
		}
		else  {											// 其他情况
			sscanf(pInquireInfo[i], "%s%d%d%d%s", latestUserID,&latestUserJetton, 
			&latestUserMoney, &latestUserBet,latestUserAction);
			status4Inqu = Handle;						// 主要处理对象
			break;
		}
	}

	allUserInfo.validUserCount = 0;
	allUserInfo.foldUserCount = 0;
	latestUserBet = 0;

	for (i = 1; i < index; i++)
	{
		if(strstr(pInquireInfo[i], "blind") != NULL)		// 盲注
		{
			if(strstr(pInquireInfo[i], "20") != NULL)		// 小盲注
			{
				char tempUserID[10];
				sscanf(pInquireInfo[i], "%s", tempUserID);	// 用户ID号
				for (int j = 0; j < allUserInfo.allUserCount; j++)
				{
					if (strcmp(allUserInfo.allUser[j].userID, tempUserID) == 0)		// 找到对应的ID号
					{	
						sscanf(pInquireInfo[i], "%s%d%d%d", allUserInfo.allUser[j].userID, &allUserInfo.allUser[j].jetton,
							&allUserInfo.allUser[j].money, &allUserInfo.allUser[j].bet);
						
						if (allUserInfo.allUser[j].bet > latestUserBet)		// 读取当前押注圈最大的押注额
							latestUserBet = allUserInfo.allUser[j].bet;

						allUserInfo.allUser[j].isValid = true;
						allUserInfo.validUserCount += 1;
						break;						
					}
				}
			}
			else 
			{											// 大盲注
				char tempUserID[10];
				sscanf(pInquireInfo[i], "%s", tempUserID);	// 用户ID号
				for (int j = 0; j < allUserInfo.allUserCount; j++)
				{
					if (strcmp(allUserInfo.allUser[j].userID, tempUserID) == 0)		// 找到对应的ID号
					{	
						sscanf(pInquireInfo[i], "%s%d%d%d", allUserInfo.allUser[j].userID, &allUserInfo.allUser[j].jetton,
							&allUserInfo.allUser[j].money, &allUserInfo.allUser[j].bet);		
						
						if (allUserInfo.allUser[j].bet > latestUserBet)		// 读取当前押注圈最大的押注额
							latestUserBet = allUserInfo.allUser[j].bet;

						allUserInfo.allUser[j].isValid = true;
						allUserInfo.validUserCount += 1;
						break;						
					}
				}
			}
		}
		else if (strstr(pInquireInfo[i], "fold") != NULL)		// 弃牌
		{
			char tempUserID[10];
			sscanf(pInquireInfo[i], "%s", tempUserID);	// 用户ID号
			for (int j = 0; j < allUserInfo.allUserCount; j++)
			{
				if (strcmp(allUserInfo.allUser[j].userID, tempUserID) == 0)		// 找到对应的ID号
				{	
					sscanf(pInquireInfo[i], "%s%d%d%d", allUserInfo.allUser[j].userID, &allUserInfo.allUser[j].jetton,
						&allUserInfo.allUser[j].money, &allUserInfo.allUser[j].bet);	
					
					if (allUserInfo.allUser[j].bet > latestUserBet)		// 读取当前押注圈最大的押注额
						latestUserBet = allUserInfo.allUser[j].bet;

					allUserInfo.allUser[j].isValid = false;
					//allUserInfo.validUserCount += 1;
					allUserInfo.foldUserCount += 1;
					break;						
				}				
			}
		}
		else if (strstr(pInquireInfo[i], "pot") != NULL)		// 奖池总额
		{
			sscanf(pInquireInfo[i], "total pot: %d", &allUserInfo.totalPot);
		}
		else			// 其他的情况 包括 check、call、raise等
		{
			char tempUserID[10];
			sscanf(pInquireInfo[i], "%s", tempUserID);			// 用户ID号
			for (int j = 0; j < allUserInfo.allUserCount; j++)
			{
				if (strcmp(allUserInfo.allUser[j].userID, tempUserID) == 0)		// 找到对应的ID号
				{	
					sscanf(pInquireInfo[i], "%s%d%d%d", allUserInfo.allUser[j].userID, &allUserInfo.allUser[j].jetton,
						&allUserInfo.allUser[j].money, &allUserInfo.allUser[j].bet);	
					
					if (allUserInfo.allUser[j].bet > latestUserBet)		// 读取当前押注圈最大的押注额
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
		case Hold:										// 手牌押注阶段
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
** 判断两张手牌的类型
*/
HoldType GetHoldCardType(char holdData[], char cbCardCount)
{
	char cbCardData[2] = {holdData[0], holdData[1]};

	// 将两张手牌按点数从大到小排序
	SortCardList(cbCardData, 2);

	// 两张牌点数相等
	if ((cbCardData[0] & 0x0F) == (cbCardData[1] & 0x0F))
	{
		if ((cbCardData[0] & 0x0F) >= 0x02 &&	// 将2~Q范围内定义为小对子
			(cbCardData[0] & 0x0F) <= 0x0C)
			return LowPair;
		else
			return HighPair;					// 将不在2~Q范围内定义为大对子
	}
	else if ((cbCardData[0] & 0x0F) == 0x01)	// 有一张A的情况
	{
		return HighCardA;
	}											// 两张牌颜色一样
	else if ((cbCardData[0] & 0x0F) >= 0x0A && (cbCardData[1] & 0x0F) >= 0x0A)
	{
		return MiddleCardKQJT;					// 手牌至少大于等于10
	}
	else if (  (cbCardData[0] & 0x0F) >=0x0C  )//有一张K或者Q的情况，该情况包括了上面的情况，然而其优先级低
	{
		return HighCardKQ;
	}
	else if(   (cbCardData[1] & 0x0F) >= 0x07  )//小牌大于等于7
	{
		return LowCard7;


	}
	
// 	else if((cbCardData[0] & 0xF0) == (cbCardData[1] & 0xF0))
// 	{
// 		return SameColor;
// 	}
	else
		return Other;							// 不是上面出现的其他情况
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
		for (i = 0; i < 15; i++)						// 分配内存空间用于保存各个用户当前的下注信息，只分配一次
			pInquireInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "inquire/");
	int index = 0;
														// 以“\n”为界保存各个用户的当前下注信息
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pInquireInfo[index], 0, 100);
		strncpy(pInquireInfo[index], bufTemp, bufTempNext - bufTemp);
		*(pInquireInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pInquireInfo[index], "/inquire") != NULL)
			break;
		bufTemp = bufTempNext + 1;
		index++;										// 表示已经读取的有效用户下注指令，包括奖池信息
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
		if(strstr(pInquireInfo[i], "blind") != NULL)		// 盲注
		{
			if(strstr(pInquireInfo[i], "20") != NULL)		// 小盲注
			{
				sscanf(pInquireInfo[i], "%s", tempUserID);	// 用户ID号
				isFold = false;
			}
			else 
			{											// 大盲注
				sscanf(pInquireInfo[i], "%s", tempUserID);	// 用户ID号
				isFold = false;
			}
		}
		else if (strstr(pInquireInfo[i], "fold") != NULL)		// 弃牌
		{
			sscanf(pInquireInfo[i], "%s", tempUserID);	// 用户ID号
			isFold =true;
		}
		else if (strstr(pInquireInfo[i], "pot") != NULL)		// 奖池总额
		{
			/*sscanf(pInquireInfo[i], "total pot: %d", &allUserInfo.totalPot);*/
			break;
		}
		else			// 其他的情况 包括 check、call、raise等
		{
			sscanf(pInquireInfo[i], "%s", tempUserID);			// 用户ID号
			isFold  = false;
		}

		//tempUserID
		int j;
		for (j = 0; j < allUserBehavior.allUserCount; j++)
		{
			if (strcmp(allUserBehavior.allUser[j].userID, tempUserID) == 0)		// 找到对应的ID号
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

