#include "game.h"
#include "notify.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern struct AllUserInfo allUserInfo;

void GetNotifyMessage(const char *buf)
{
	static char *pNotifyInfo[10];	
	static int mallocFlag = 0;
	int i;
	
	if (mallocFlag == 0)
	{
		mallocFlag = 1;									// 分配内存空间用于保存各个用户信息，只分配一次
		for (i = 0; i < 10; i++)	pNotifyInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "notify/");
	int index = 0;
	// 以“\n”为界保存各个用户的信息
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pNotifyInfo[index], 0, 100);
		strncpy(pNotifyInfo[index], bufTemp,bufTempNext - bufTemp);
		*(pNotifyInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pNotifyInfo[index], "/seat") != NULL)	// 此时已经读取完毕，退出
			break;
		bufTemp = bufTempNext + 1;
		index++;										// 表示已经读取的用户数
	}
	
	allUserInfo.allUserCount = index - 1;				// 总共的用户数（包括已经输完的和没有输完的）
	allUserInfo.validUserCount = index - 1;				// 所有有效的用户数（即没有输完的）
	for (i = 1; i < index; i++)
	{
		allUserInfo.allUser[i-1].isValid = true;		// 刚开始时各个用户均为有效（即没有弃牌）
		
		if (strstr(pNotifyInfo[i], "button") != NULL)		// 庄家信息
		{
			sscanf(pNotifyInfo[i], "button: %s %d %d",	// 信息包括用户ID、在该局的筹码、剩下的金币和当前押注
				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
				&allUserInfo.allUser[i-1].money);		
			allUserInfo.allUser[i-1].bet = 0;			
		}												// 小盲注信息
		else if (strstr(pNotifyInfo[i], "small blind") != NULL)
		{
			sscanf(pNotifyInfo[i], "small blind: %s %d %d",
				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
				&allUserInfo.allUser[i-1].money);
			//allUserInfo.allUser[i-1].bet = 50;
			allUserInfo.allUser[i-1].bet = BlindBet / 2;
		}												// 大盲注信息
		else if (strstr(pNotifyInfo[i], "big blind") != NULL)
		{
			sscanf(pNotifyInfo[i], "big blind: %s %d %d",
				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
				&allUserInfo.allUser[i-1].money);
			//allUserInfo.allUser[i-1].bet = 100;
			allUserInfo.allUser[i-1].bet = BlindBet;
		}
		else											// 其他玩家的信息
		{
			sscanf(pNotifyInfo[i], "%s %d %d",
				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
				&allUserInfo.allUser[i-1].money);
			allUserInfo.allUser[i-1].bet = 0;
		}
	}
	
// 	for (i = 0; i < index - 1; i++)						// 用户自己对应哪一条信息
// 	{
// 		if (strcmp(allUserInfo.allUser[i].userID, ownerID) == 0)
// 		{
// 			allUserInfo.ownIndex = i;
// 			break;
// 		}
// 	}
	
//	allUserInfo.holdRaised  = false;	// 每局开始时置为false，表示该圈没有加注
// 	allUserInfo.flopRaised  = false;
// 	allUserInfo.turnRaised  = false;
// 	allUserInfo.riverRaised = false;
}