#pragma once
#include "cObject.h"

class cMonster : public cObject
{
	float init_x;
	float init_z;
	volatile bool is_Active;
	MONSTER_TYPE m_type;

	int attack_count;
	int attack_speed;
	bool hit_on;
public:
	cMonster();
	virtual ~cMonster();
public:
	void HeartBeat();
	void WakeUpMonster();
	void AttackedPlayer(float dam, UINT target);
	void AttackPlayer(float dam, UINT player);
public:
	void SetInitX(float x) { init_x = x; }
	void SetInitZ(float z) { init_z = z; }
	void SetType(MONSTER_TYPE type) { m_type = type; }
	void SetHp(int set_hp) { hp = set_hp; }
	void SetRecreateXY() { x = init_x, z = init_z; }
	void SetAttackSpeed(int speed) { attack_speed = speed; }
	void SetHitFalse() { hit_on = false; }
public:
	int GetHp() { return hp; }
	MONSTER_TYPE GetMonsterType() { return m_type; }
};