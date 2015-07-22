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
		mallocFlag = 1;									// �����ڴ�ռ����ڱ�������û���Ϣ��ֻ����һ��
		for (i = 0; i < 10; i++)	pNotifyInfo[i] = (char *)malloc(sizeof(char) * 100);
	}
	char *bufTemp, *bufTempNext;
	bufTemp = strstr(buf, "notify/");
	int index = 0;
	// �ԡ�\n��Ϊ�籣������û�����Ϣ
	while ((bufTempNext = strstr(bufTemp, "\n")) != NULL)
	{
		memset(pNotifyInfo[index], 0, 100);
		strncpy(pNotifyInfo[index], bufTemp,bufTempNext - bufTemp);
		*(pNotifyInfo[index] + (bufTempNext - bufTemp)) = '\0';
		if (strstr(pNotifyInfo[index], "/seat") != NULL)	// ��ʱ�Ѿ���ȡ��ϣ��˳�
			break;
		bufTemp = bufTempNext + 1;
		index++;										// ��ʾ�Ѿ���ȡ���û���
	}
	
	allUserInfo.allUserCount = index - 1;				// �ܹ����û����������Ѿ�����ĺ�û������ģ�
	allUserInfo.validUserCount = index - 1;				// ������Ч���û�������û������ģ�
	for (i = 1; i < index; i++)
	{
		allUserInfo.allUser[i-1].isValid = true;		// �տ�ʼʱ�����û���Ϊ��Ч����û�����ƣ�
		
		if (strstr(pNotifyInfo[i], "button") != NULL)		// ׯ����Ϣ
		{
			sscanf(pNotifyInfo[i], "button: %s %d %d",	// ��Ϣ�����û�ID���ڸþֵĳ��롢ʣ�µĽ�Һ͵�ǰѺע
				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
				&allUserInfo.allUser[i-1].money);		
			allUserInfo.allUser[i-1].bet = 0;			
		}												// Сäע��Ϣ
		else if (strstr(pNotifyInfo[i], "small blind") != NULL)
		{
			sscanf(pNotifyInfo[i], "small blind: %s %d %d",
				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
				&allUserInfo.allUser[i-1].money);
			//allUserInfo.allUser[i-1].bet = 50;
			allUserInfo.allUser[i-1].bet = BlindBet / 2;
		}												// ��äע��Ϣ
		else if (strstr(pNotifyInfo[i], "big blind") != NULL)
		{
			sscanf(pNotifyInfo[i], "big blind: %s %d %d",
				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
				&allUserInfo.allUser[i-1].money);
			//allUserInfo.allUser[i-1].bet = 100;
			allUserInfo.allUser[i-1].bet = BlindBet;
		}
		else											// ������ҵ���Ϣ
		{
			sscanf(pNotifyInfo[i], "%s %d %d",
				allUserInfo.allUser[i-1].userID, &allUserInfo.allUser[i-1].jetton,  
				&allUserInfo.allUser[i-1].money);
			allUserInfo.allUser[i-1].bet = 0;
		}
	}
	
// 	for (i = 0; i < index - 1; i++)						// �û��Լ���Ӧ��һ����Ϣ
// 	{
// 		if (strcmp(allUserInfo.allUser[i].userID, ownerID) == 0)
// 		{
// 			allUserInfo.ownIndex = i;
// 			break;
// 		}
// 	}
	
//	allUserInfo.holdRaised  = false;	// ÿ�ֿ�ʼʱ��Ϊfalse����ʾ��Ȧû�м�ע
// 	allUserInfo.flopRaised  = false;
// 	allUserInfo.turnRaised  = false;
// 	allUserInfo.riverRaised = false;
}