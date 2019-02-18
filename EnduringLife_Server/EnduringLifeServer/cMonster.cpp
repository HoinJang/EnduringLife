#include "stdafx.h"
#include "cMonster.h"
#include "cMyNamespace.h"
#include "cThreadFunc.h"
#include "cPlayer.h"

cMonster::cMonster()
{
	my_sector = nullptr;
	is_Active = false;
	hp = 500.0f;
	mp = 500.0f;
	damage = 0.0f;
	m_type = TYPE1;
	attack_count = 0;
	attack_speed = 0;
	hit_on = false;
}

cMonster::~cMonster()
{
	
}

void cMonster::HeartBeat()
{
	bool is_move = false;
	bool search_near_player = false;
	int near_player = -1;
	float distance = 100.0f;
	int new_animation_type = -1;

	UpdatesSectorSearch();
	// near_list ������Ʈ

	unordered_set<UINT> player_list = near_list;

	for (auto player : player_list)
	{
		if (player < MAX_USER)
		{
			float temp = sqrtf((x - objects[player]->GetX())*(x - objects[player]->GetX()) +
				(z - objects[player]->GetZ())*(z - objects[player]->GetZ()));

			if (temp < MONSTER_RADIUS && temp < distance)
			{
				search_near_player = true;
				distance = temp;
				near_player = player;
			}
		}
	}

	//cout << "hit on : " << hit_on << endl;


	float move_x = x;
	float move_z = z;

	if (search_near_player)
	{
		if (hit_on)
		{
			new_animation_type = HIT;
		}
		else
		{
			float temp_x = objects[near_player]->GetX() - x;
			float temp_z = objects[near_player]->GetZ() - z;
			float size_vector = sqrtf((temp_x * temp_x) + (temp_z * temp_z));

			sight_x = temp_x / size_vector;
			sight_z = temp_z / size_vector;

			if (distance > MONSTER_ATTACK_RANGE)
			{
				attack_count = 0;
				move_x = move_x + sight_x * MONSTER_SPEED;
				move_z = move_z + sight_z * MONSTER_SPEED;

				new_animation_type = WALK;
			}
			else
			{
				attack_count++;
				if (attack_count >= attack_speed)
				{
					AttackPlayer(damage, near_player);
					attack_count = 0;
				}
				new_animation_type = ATTACK;
			}
		}
		is_move = true;
	}
	else
	{
		new_animation_type = IDLE;

		is_move = false;
	}


	if (move_x >= 0.0f && move_x < BOARD_WIDTH && move_z >= 0.0f && move_z < BOARD_HEIGHT && is_move)
	{
		x = move_x;
		z = move_z;
	}
	else
	{
		if (m_type == TYPE1)
		{
			sight_x = -1.0f;
			sight_z = 0.0f;
		}
		else if (m_type == TYPE2)
		{
			sight_x = 1.0f;
			sight_z = 0.0f;
		}
		else
		{
			sight_x = -1.0f;
			sight_z = 0.0f;
		}
	}

	unordered_set<UINT> view;

	//�̵� �� �� ���͸� ã�´�
	GetCurrZone()->UpdateSector(id);

	//view�� ���� near_list���� �ֺ� �÷��̾ �ִ´�.
	view.clear();

	for (auto prev_player : near_list)
	{
		if (prev_player < MAX_USER)
		{
			view.insert(prev_player);
		}
	}

	// near_list ������Ʈ�� �Ѵ�.
	UpdatesSectorSearch();

	//temp�� ������Ʈ�� near_list���� �ֺ� �÷��̾ �ִ´�.
	unordered_set<UINT> temp;
	
	temp.clear();
	
	for (auto curr_player : near_list)
	{
		if (curr_player < MAX_USER)
			temp.insert(curr_player);
	}

	//view : �̵� �� near_list���� �ֺ� �÷��̾ ���� �����̳�
	//temp : �̵� �� near_list���� �ֺ� �÷��̾ ���� �����̳�

	bool animation_change = false;

	if (new_animation_type != animation_num)
	{
		animation_num = new_animation_type;
		animation_change = true;
	}

	//view�� Ž�� //�̵���
	for (auto prev : view)
	{
		cPlayer* player = dynamic_cast<cPlayer*>(objects[prev]);
		
		if (player->GetIsUse() == false)
			continue;
		//temp�� ������ - �̵��Ŀ�
		if (temp.count(prev))
		{
			SendMoveObject(prev, id);

			if (animation_change)
				SendAnimateObject(prev, id);
		}
		//temp�� ������ - �̵��Ŀ�
		if(temp.count(prev) == 0)
		{
			SendRemoveObject(prev, id);
		}
	}
	//���� ���� �÷��̾�ó��
	for (auto curr : temp)
	{
		cPlayer* player = dynamic_cast<cPlayer*>(objects[curr]);

		if (player->GetIsUse() == false)
			continue;

		if (0 == view.count(curr))
		{
			SendPutObject(curr, id);
			if (animation_change)
				SendAnimateObject(curr, id);
		}
	}

	//�ֺ��� �÷��̾ ������
	if (temp.empty() == false && is_use)
	{
		if (hit_on)
		{
			if (m_type == TYPE1)
			{
				AddTimer(id, NPC_HIT_ON, 820);
			}
			else if (m_type == TYPE2)
			{
				AddTimer(id, NPC_HIT_ON, 640);
			}
			else if (m_type == TYPE3)
			{
				AddTimer(id, NPC_HIT_ON, 640);
			}
			return;
		}
		AddTimer(id, NPC_MOVE, 250);
		return;
	}
	is_Active = false;
}

void cMonster::WakeUpMonster()
{
	if (true == is_Active) 
		return;
	is_Active = true;

	AddTimer(id, NPC_MOVE, 250);
}

void cMonster::AttackedPlayer(float dam, UINT target)
{

	//cout << id << "�� ���Ͱ�" << (int)target << "���� ���ݴ���" << endl;


	hp -= dam;
	
	if (attack_count > 0)
	{
		attack_count = 0;
	}
	animation_num = HIT;
	hit_on = true;

	unordered_set<UINT> player_list = near_list;

	for (auto player : player_list)
	{
		if (player < MAX_USER)
		{
			SendAnimateObject(player, id);
		}
	}

	//hp�� 0���� ������ �ֺ��÷��̾� ���� ���� �����϶�� ��Ŷ�� �������ش�.

	if (hp <= 0)
	{
		cPlayer* a_player = dynamic_cast<cPlayer*>(objects[target]);

		a_player->ExpPlus(100);

		is_use = false;

		unordered_set<UINT> player_list = near_list;

		for (auto player : player_list)
		{
			if (player < MAX_USER)
			{
				SendRemoveObject(player, id);
			}
		}
		is_Active = false;
		AddTimer(id, NPC_RECREATE, 30000);
	}
}

void cMonster::AttackPlayer(float dam, UINT player)
{
	cPlayer* a_player = dynamic_cast<cPlayer*>(objects[player]);

	a_player->AttackedMonster(dam);

}
