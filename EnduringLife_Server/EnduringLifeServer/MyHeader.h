#pragma once

const float VIEW_RADIUS = 128;
const float MONSTER_RADIUS = 64;
const float PLAYER_ATTACK_RANGE = 25.0f;
const float MONSTER_ATTACK_RANGE = 20.0f;
const float ASSASIN_SKILL_RANGE = 30.0f;
const float SWORDER_SKILL_RANGE = 30.0f;

const int E_RECV = 1;
const int E_SEND = 2;
const int NPC_MOVE = 3;
const int NPC_RECREATE = 4;
const int NPC_HIT_ON = 5;

const float NPC_MOVE_DISTANCE = 1;

const float OBJECT_SPEED = 1;

const float MONSTER_SPEED = 1;

typedef struct Npc_event
{
	UINT id; //오브젝트 ID
	int type; //EVENT의 타입w
	UINT time; //시간
}NPC_EVENT;

//priority_queue 사용을 위한 비교연산자.(NPC_EVENT를 비교하기 위함)
class Comparison
{
public:
	bool operator() (const NPC_EVENT& e1, const NPC_EVENT& e2)
	{
		return (e1.time > e2.time);
	}
};

enum SCENE
{
	TOWN = 0,
	FIELD = 1
};


struct PlayerList
{
	char *id;
	PLAYER_TYPE type;
	int level;
	int exp;
	bool quest_1;
	bool quest_2;
	bool quest_3;
};
