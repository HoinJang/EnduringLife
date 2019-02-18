#pragma once
#include "stdafx.h"

#define WM_SOCKET   WM_USER + 1

#define MY_SERVER_PORT  4000

#define MAX_BUFF_SIZE   4000
#define BUF_SIZE 4000

#define MAX_PACKET_SIZE  255

#define BOARD_WIDTH   1024
#define BOARD_HEIGHT  1024

#define MAX_USER 10
#define NPC_START  10
#define MAX_OBJECT_INDEX 51

#define MAX_STR_SIZE  100

#define CS_UP				1
#define CS_DOWN				2
#define CS_LEFT				3
#define CS_RIGHT			4
#define CS_TELEPORT			5
#define CS_ATTACK			6	
#define CS_SIGHT			7
#define CS_ANIMATE			8
#define CS_QUEST_OK			9
#define CS_SKILL				10
#define CS_SKILL_MOVE      11

#define SC_POS				1
#define SC_PUT_PLAYER		2
#define SC_REMOVE_PLAYER	3
#define SC_SCENE			4
#define SC_ANIMATE			5
#define SC_EXP				6
#define SC_LEVELUP			7
#define SC_INFO				8
#define SC_SIGHT			9
#define SC_EFFECT			10

enum PlantType {
	FieldTree,
	FieldFlower,
	FieldGrass,
	TownTree,
	TownFlower,
	TownGrass,
	HPBar
};
enum SCENE
{
	TOWN = 0,
	FIELD = 1
};
enum ANIMATE
{
	IDLE = 0,
	WALK = 1,
	ATTACK = 2,
	HIT = 3,
	DIE = 4,
	SKILL = 5
};
enum QUESTNUM
{
	SPIDER = 1,
	CRAB = 2,
	GOLEM = 3
};
enum PLAYERTYPE
{
	SWORDER = 0,
	ASSASSIN = 1,
	OTHER
};
enum EFFECT_TYPE
{
	SWORD_DEFAULT = 0,
	SWORD_SKILL = 1,
	ASSASIN_DEFAULT = 2,
	ASSASIN_SKILL = 3
};

#define SworderDefault 0		//검사 일반공격
#define SworderSkill 1			//검사 광역스킬
#define AssassinDefault 2		//마법사 일반공격
#define AssassinSkill	 3			//마법사 광역스킬

///////////////////////////////////////////////////////////////////////////////
// 쉐이더
#define HP 1  
#define MP 2  
#define EXP 3
#define MAXBAR 0
#define LEVEL 4
#define QUEST 5
#define QUSET_PROGRESS 6
#define NUM 7
#define WATER 18
#define SKILL 8 
/////////////////////////////////////////////////////////////////////////////
//
#define PI            3.141592
#define RADIAN         ( PI / 180.0 )
#define DEGREE         ( 180.0 / PI )
#define RAD2DEG(Rad)   ( Rad * DEGREE )

#pragma pack (push, 1)
struct cs_packet_move {
	BYTE size;
	BYTE type;
	char dir_x;
	char dir_z;
};
struct cs_packet_sight {
	BYTE size;
	BYTE type;
	char dir_x;
	char dir_z;
};
struct cs_packet_attack
{
	BYTE size;
	BYTE type;
};

struct cs_packet_animate
{
	BYTE size;
	BYTE type;
	BYTE animate_num;
};

struct cs_packet_quest_ok
{
	BYTE size;
	BYTE type;
	BYTE quest_num;
};

struct cs_packet_skill
{
	BYTE size;
	BYTE type;
	BYTE skill;
};


struct sc_packet_pos {
	BYTE size;
	BYTE type;
	WORD id;
	short x;
	short y;
	short hp;
	short mp;
	char sight_x;
	char sight_z;
};

struct sc_packet_put_player {
	BYTE size;
	BYTE type;
	WORD id;
	short x;
	short y;
	short hp;
	short mp;
	WORD exp;
	char sight_x;
	char sight_z;
	BYTE animation_num;
	BYTE player_type;
};

struct sc_packet_remove_player {
	BYTE size;
	BYTE type;
	WORD id;
	short hp;
};

struct sc_packet_scene_player
{
	BYTE size;
	BYTE type;
	WORD id;
	BYTE scene;
};

struct sc_packet_animate
{
	BYTE size;
	BYTE type;
	WORD id;
	BYTE animate_num;
};

struct sc_packet_exp
{
	BYTE size;
	BYTE type;
	WORD id;
	WORD exp;
};

struct sc_packet_levelup
{
	BYTE size;
	BYTE type;
	WORD id;
	BYTE level;
};

struct sc_packet_info
{
	BYTE size;
	BYTE type;
	WORD id;
	BYTE level;
	WORD exp;
	bool Q1;
	bool Q2;
	bool Q3;
};

struct sc_packet_sight
{
	BYTE size;
	BYTE type;
	WORD id;
	char sight_x;
	char sight_z;
};

struct sc_packet_effect
{
	BYTE size;
	BYTE type;
	WORD id;
	BYTE effect;
};
#pragma pack (pop)