#ifndef GAME_H
#define  GAME_H

#include "cardtype.h"
enum Status { Hold, Flop, Turn, River };
enum Status4Inquire { Allout, Blind, Handle };
enum HoldType { HighPair, LowPair, HighCardA, MiddleCardKQJT, HighCardKQ,LowCard7,Other };

//enum Behavior {Aggress = 1, Moder = 2, Conser = 3};	// �ֱ��Ӧ�������еȺͱ���
// HighPair: Pair 9 ~ Pair A  LowPair: Pair 2 ~ Pair 8 HighCardA: a A SameColor: 

#define Bet4AllIn -2
#define Bet4Fold  -1
#define Bet4Survive -3 
#define Bet4Check 0

#define BlindBet  40

#define RiskCount 4

struct UserInfo {
	char userID[10];
	int jetton;
	int money;
	int bet;
	bool isValid;
};

struct AllUserInfo {
	struct UserInfo allUser[8];		// �����8���û�
	int allUserCount;				// �þֵ��û����������Ѿ����ƺͻ�û�����Ƶģ�
	int validUserCount;				// �þ���Ч�û�������û�����Ƶģ�
	int foldUserCount;				// �����Ѿ�fold���û���
	int ownIndex;					// ���Լ�����Ϣ��allUser�����ж�Ӧ���±�
	int totalPot;					// ��������
	int currentRound;				//����

	bool holdRaised;				// holdȦ�Ƿ��Ѿ��ӹ�ע false: �� true: ��
	bool flopRaised;
	bool turnRaised;
	bool riverRaised;
};

struct EachUserBehavior {
	char userID[10];
	//int  holdFoldTimes;
	//int  holdAllinTimes;
	//int  holdRaiseTimes;
	//int  holdCheckTimes;

	unsigned char  holdFoldTimesArray[50];
	unsigned char  holdAllinTimesArray[50];
	unsigned char  holdRaiseTimesArray[50];
	unsigned char  holdCheckTimesArray[50];

	char holdAction[20];		//
	int  bet[600];			//ÿһ��Ѻע�����ֵ
	int	 money[600];		//���ֿ�ʼʱ���
	int  jetton[600];		//���ֿ�ʼʱ����
	bool isOver;			//��¼�Ƿ��Ѿ�����
	bool isFold;			//��¼�Ƿ��Ѿ�����
	int  dangerScore;		// 1�֣����� 2�֣��е� 5�֣�����
};

struct AllUserBehavior {
	struct EachUserBehavior allUser[8];		// �����8���û�
	
	int allUserCount;						// ��Ϸ�տ�ʼʱ���û���
	int roundUserCount;						// ���ֿ�ʼʱ���û���
	int roundValidUserCount;				// ���ֿ�ʼ��ǰʱ����Ч���û�������û�����Ƶģ�
	
	int ownIndex;							// ���Լ�����Ϣ��struct EachUserBehavior allUser�����ж�Ӧ���±�
	int holdFirstActionIndex;				// �����˼�ע�����ä��ĵ�һ����������action��ѡ��;
											//�����˼�ע�������һ�μ�ע���˿��Կ�����һ������action��ѡ��
											//(�ü�ע��Ӱ���ƾֵ�inquire�ķ�Χ)
	int buttonIndex;						// ����ׯ�ҵ���Ϣ��Ȼ����ÿ�ֻ����seat�������Ϣ�����ׯ�����ǵ�һ��(index:
	
	int totalPot;							// ��������
	int currentRound;						// ��ǰ�ľ���

	int totalDangerScore;					// �ܵ�Σ��ϵ��
	bool needConser;						// �Ƿ���Ҫ����  
};

HoldType GetHoldCardType(char holdData[], char cbCardCount);

#endif