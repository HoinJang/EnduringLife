#pragma once
#include "cObject.h"

class cPlayer : public cObject
{
private:
	SOCKET sock;

	BYTE packet_size;
	BYTE saved_size;
	BYTE packet_buf[MAX_PACKET_SIZE];
	BYTE recv_buf[MAX_BUFF_SIZE];

	unordered_set<UINT> viewlist;
	mutex viewMutex;
	BYTE level;
	float damage;
	WORD exp;

	bool MaxLevel;
	BYTE quest_now;
	bool quest_1;
	bool quest_2;
	bool quest_3;
	bool quest_end;

	char* game_id;


public:
	const SOCKET& GetSock() const { return sock; }
	unordered_set<UINT>& GetViewlist() { return viewlist; }
public:
	void SetSock(const SOCKET& s) { sock = s; }
	void SetLevel(BYTE lev);
	void SetExp(WORD e) { exp = e; }
	void SetQuestNow(BYTE q) { quest_now = q; }
	void SetQuest1(bool quest) { quest_1 = true; }
	void SetQuest2(bool quest) { quest_2 = true; }
	void SetQuest3(bool quest) { quest_3 = true; }
	void SetQuestEnd(bool quest) { quest_end = true; }
	void SetGameID(char* name) { 
		strcpy(game_id, name);
	}
public:
	BYTE GetLevel() { return level; }
	WORD GetExp() { return exp; }
	BYTE GetQuestNow() { return quest_now; }
	char* GetGameID() { return game_id; }
	bool GetQuest1() { return quest_1; }
	bool GetQuest2() { return quest_2; }
	bool GetQuest3() { return quest_3; }
public:
	void RecvPacket();
	void ReadPacket(const UINT id, const int transferred);
	void UpdateViewList();
	void AttackMonster();
	void AttackedMonster(float dam);
	void ExpPlus(WORD num);
	void SkillSworder();
	void SkillAssasin();
public:
	void Clear();
public:
	cPlayer();
	virtual ~cPlayer();
};

