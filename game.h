#ifndef GAME_H
#define  GAME_H

#include "cardtype.h"
enum Status { Hold, Flop, Turn, River };
enum Status4Inquire { Allout, Blind, Handle };
enum HoldType { HighPair, LowPair, HighCardA, MiddleCardKQJT, HighCardKQ,LowCard7,Other };

//enum Behavior {Aggress = 1, Moder = 2, Conser = 3};	// 分别对应激进、中等和保守
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
	struct UserInfo allUser[8];		// 最多有8个用户
	int allUserCount;				// 该局的用户数（包括已经弃牌和还没有弃牌的）
	int validUserCount;				// 该局有效用户数（即没有弃牌的）
	int foldUserCount;				// 本局已经fold的用户数
	int ownIndex;					// 我自己的信息在allUser变量中对应的下标
	int totalPot;					// 奖池总数
	int currentRound;				//局数

	bool holdRaised;				// hold圈是否已经加过注 false: 否 true: 是
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
	int  bet[600];			//每一句押注的最大值
	int	 money[600];		//本局开始时金币
	int  jetton[600];		//本局开始时筹码
	bool isOver;			//记录是否已经出局
	bool isFold;			//记录是否已经弃牌
	int  dangerScore;		// 1分：激进 2分：中等 5分：保守
};

struct AllUserBehavior {
	struct EachUserBehavior allUser[8];		// 最多有8个用户
	
	int allUserCount;						// 游戏刚开始时的用户数
	int roundUserCount;						// 本局开始时的用户数
	int roundValidUserCount;				// 本局开始后当前时刻有效的用户数（即没有弃牌的）
	
	int ownIndex;							// 我自己的信息在struct EachUserBehavior allUser变量中对应的下标
	int holdFirstActionIndex;				// 若无人加注，则大盲后的第一个人是最先action的选手;
											//若有人加注，则最后一次加注的人可以看做新一轮最先action的选手
											//(该加注会影响牌局的inquire的范围)
	int buttonIndex;						// 储存庄家的信息，然而在每轮会更新seat里面的信息，因而庄家总是第一个(index:
	
	int totalPot;							// 奖池总数
	int currentRound;						// 当前的局数

	int totalDangerScore;					// 总的危险系数
	bool needConser;						// 是否需要保守  
};

HoldType GetHoldCardType(char holdData[], char cbCardCount);

#endif