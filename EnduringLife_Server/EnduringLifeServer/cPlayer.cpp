#include "stdafx.h"
#include "cPlayer.h"
#include "cObject.h"
#include "cMonster.h"

#include "cThreadFunc.h"
#include "cMyNameSpace.h"

cPlayer::cPlayer() : packet_size(0), saved_size(0)
{
	ZeroMemory(recv_buf, sizeof(recv_buf));
	ZeroMemory(&exover.wsaover, sizeof(exover.wsaover));

	viewMutex.lock();
	viewlist.clear();
	viewMutex.unlock();
	exover.wsabuf.buf = reinterpret_cast<char*>(recv_buf);
	exover.wsabuf.len = sizeof(recv_buf);
	my_sector = nullptr;

	hp = 1000.0f;
	mp = 1000.0f;

	level = 1;
	exp = 0;
	MaxLevel = false;

	quest_now = 1;
	quest_1 = false;
	quest_2 = false;
	quest_3 = false;
	quest_end = false;

	game_id = new char[20];

	p_type = MALE;
}

cPlayer::~cPlayer()
{
}

void cPlayer::SetLevel(BYTE lev)
{
	level = lev;

	if (level == 1)
	{
		damage = 50.0f;
		hp = 200.0f;
		mp = 1000.0f;
	}
	else if (level == 2)
	{
		damage = 200.0f;
		hp = 1000.0f;
		mp = 1000.0f;
	}
	else if (level == 3)
	{
		damage = 1000.0f;
		hp = 3000.0f;
		mp = 1000.0f;
	}
}

void cPlayer::RecvPacket()
{
	DWORD flags = 0;

	int ret_val = WSARecv(sock, &exover.wsabuf, 1, NULL, &flags, &exover.wsaover, NULL);
	if (ret_val != 0)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
			cout << " [error] WSARecv() on " << id << " Player::RecvPacket() " << endl;
	}
}

void cPlayer::ReadPacket(const UINT id, const int transferred)
{
	BYTE* ptr = recv_buf;

	int remained = transferred;

	while (0 < remained)
	{
		if (0 == packet_size)
			packet_size = ptr[0];

		int required = packet_size - saved_size;

		if (remained >= required)
		{
			memcpy(packet_buf + saved_size, ptr, required);

			ProcessPacket(id, packet_buf);

			remained -= required;
			ptr += required;

			packet_size = 0;
			saved_size = 0;
		}

		else
		{
			memcpy(packet_buf + saved_size, ptr, remained);

			saved_size += remained;

			remained = 0;
		}
	}
}

void cPlayer::UpdateViewList()
{
	//sector_search가 업데이트 된 리스트
	//viewlist는 오브젝트가 사용하는 viewlist

	unordered_set<UINT> add_list;
	unordered_set<UINT> remove_list;
	unordered_set<UINT> exist_list;

	//새로 더해진 오브젝트를 add_list에 추가
	for (auto add_player : near_list)
	{
		if (0 == viewlist.count(add_player))
		{
			add_list.insert(add_player);
		}
	}
	//없어진 플레이어를 remove_list에 추가
	for (auto remove_player : viewlist)
	{
		if (0 == near_list.count(remove_player))
		{
			remove_list.insert(remove_player);
		}
	}
	//원래 있었고, 지금도 있는 플레이어들을 exist_list에 추가
	for (auto exist_player : near_list)
	{
		if (viewlist.count(exist_player))
		{
			exist_list.insert(exist_player);
		}
	}

	//1. 새로 들어온 플레이어 처리
	for (auto add : add_list)
	{
		SendPutObject(id, add);

		//viewlist에 없던 플레이어를 넣어준다.
		viewMutex.lock();
		viewlist.insert(add);
		viewMutex.unlock();
		
		//상대 뷰리스트에 없으면 넣어주고, 상대방이 플레이어라면 내 정보를 보내준다.
		if (add < MAX_USER)
		{
			cPlayer* other = dynamic_cast<cPlayer*>(objects[add]);

			other->viewMutex.lock();
			if (0 == other->GetViewlist().count(id))
			{
				other->GetViewlist().insert(id);
				other->viewMutex.unlock();
				SendPutObject(add, id);
			}
			//상대 뷰리스트에 있으면 내가 움직인 정보를 보내준다.
			else
			{
				other->viewMutex.unlock();
				SendMoveObject(add, id);
			}
		}
		else if (add >= MAX_USER && add < MAX_OBJECT_INDEX) //몬스터 이면 깨워준다.
		{
			cMonster* monster = dynamic_cast<cMonster*>(objects[add]);

			if(monster->GetIsUse())
				monster->WakeUpMonster();

			SendAnimateObject(id, add);
		}
	}
	//2. 이미 존재하던 플레이어들 처리
	for (auto exist : exist_list)
	{
		SendMoveObject(id, exist);

		if (exist < MAX_USER)
		{
			cPlayer* other = dynamic_cast<cPlayer*>(objects[exist]);
			//상대 뷰리스트에 없으면 상대 뷰리스트에 나를 집어 넣어주고, 내가 접속한것을 알린다.
			other->viewMutex.lock();
			if (0 == other->GetViewlist().count(id))
			{
				other->GetViewlist().insert(id);
				other->viewMutex.unlock();
				SendPutObject(exist, id);
			}
			//상대 뷰리스트에 있으면, 내가 움직인 정보를 보내준다.
			else
			{
				other->viewMutex.unlock();
				SendMoveObject(exist, id);
			}
		} 
	}
	//3. 제거할 플레이어들 처리
	for (auto remove : remove_list)
	{
		//내 뷰리스트에서 제거시키고, 나에게 그 플레이어를 제거시킨다는 메세지를 보낸다.
		viewMutex.lock();
		viewlist.erase(remove);
		viewMutex.unlock();
		SendRemoveObject(id, remove);

		if (remove < MAX_USER)
		{
			cPlayer* other = dynamic_cast<cPlayer*>(objects[remove]);

			other->viewMutex.lock();
			if (0 != other->GetViewlist().count(id))
			{
				other->GetViewlist().erase(id);
				other->viewMutex.unlock();
				SendRemoveObject(remove, id);
			}
			else
			{
				other->viewMutex.unlock();
			}
		}
	}


	// ViewList = Near_list
	// 내 뷰리스트는 업데이트 되었다.
}

void cPlayer::AttackMonster()
{
	unordered_set<UINT> mylist = viewlist;

	unordered_set<UINT> monster_list;

	for (auto monster : mylist)
	{
		if (monster >= MAX_USER)
		{
			float distance = (objects[monster]->GetX() - x)*(objects[monster]->GetX() - x) + (objects[monster]->GetZ() - z)*(objects[monster]->GetZ() - z);

			if (distance < PLAYER_ATTACK_RANGE * PLAYER_ATTACK_RANGE)
			{
				if(objects[monster]->GetIsUse())
					monster_list.insert(monster);
			}
		}
	}

	UINT near_monster = -1;

	float near_monster_distance = PLAYER_ATTACK_RANGE * PLAYER_ATTACK_RANGE;

	for (auto monster : monster_list)
	{
		float distance = (objects[monster]->GetX() - x)*(objects[monster]->GetX() - x) + (objects[monster]->GetZ() - z)*(objects[monster]->GetZ() - z);

		if (distance < near_monster_distance)
		{
			near_monster = monster;
			near_monster_distance = distance;
		}
	}

	if (near_monster != -1)
	{
		cMonster* a_monster = dynamic_cast<cMonster*>(objects[near_monster]);

		a_monster->AttackedPlayer(damage, id);

		float temp_x = a_monster->GetX() - x;
		float temp_z = a_monster->GetZ() - z;

		float temp_size = sqrt((temp_x * temp_x) + (temp_z * temp_z));

		sight_x = temp_x / temp_size;
		sight_z = temp_z / temp_size;


		if (p_type == MALE)
		{
			float move_x, move_z;

			move_x = a_monster->GetX();
			move_z = a_monster->GetZ();

			move_x = move_x + sight_x * 12;
			move_z = move_z + sight_z * 12;

			a_monster->SetX(move_x);
			a_monster->SetZ(move_z);
		}
	}

	SendSightObject(id, id);

	for(auto player : mylist)
	{
		if (player < MAX_USER)
		{
			SendSightObject(player, id);
		}
	}

	for (auto player : mylist)
	{
		if (player < MAX_USER)
		{
			if (p_type == MALE)
				SendEffectObject(player, id, SWORD_IDLE);
			else if (p_type == FEMALE)
				SendEffectObject(player, id, ASSASIN_IDLE);
		}
	}

	if (p_type == MALE)
		SendEffectObject(id, id, SWORD_IDLE);
	else if (p_type == FEMALE)
		SendEffectObject(id, id, ASSASIN_IDLE);
}

void cPlayer::AttackedMonster(float dam)
{
	unordered_set<UINT> p_list = viewlist;

	//cout << id << "의 애니메이션 번호는 : " << (int)GetAnimationNum() << endl;

	if (GetAnimationNum() == IDLE)
	{
		SetAnimationNum(HIT);

		for (auto player : p_list)
		{
			if (player < MAX_USER)
			{
				SendAnimateObject(player, id);
			}
		}
		SendAnimateObject(id, id);
	}

	hp -= dam;

	//cout << id << "의 HP : " << hp << endl;

	if (hp <= 0)
	{
		Clear();

		scene = TOWN;

		x = (float)(BOARD_WIDTH / 2);
		z = (float)(BOARD_WIDTH / 2);
		sight_x = 0.0f;
		sight_z = 1.0f;
		is_use = true;
		animation_num = IDLE;

		float tempx, tempz;

		sc_packet_put_player p;
		p.id = id;
		p.size = sizeof(sc_packet_put_player);
		p.type = SC_PUT_PLAYER;

		SetLevel(level);

		tempx = (roundf(x * 10) / 10) * 10;
		tempz = (roundf(z * 10) / 10) * 10;

		p.x = tempx;
		p.y = tempz;
		p.animation_num = animation_num;
		p.hp = hp;
		p.mp = mp;
		p.exp = exp;

		SendPacket(id, &p);
		//초기화 한 정보를 자신에게 보내줌	

		//나의 뷰리스트를 작성, 내 뷰리스트에 들어온 플레이어들에게 내 정보를 전달.
		UpdataPlayerView(id);

		SendAnimateObject(id, id);

		SendSceneObject(id, scene);
	}
	SendMoveObject(id);
}

void cPlayer::ExpPlus(WORD num)
{
	exp += num;

	if (exp >= 1200)
	{
		if (level == 1)
		{
			SetLevel(2);
			SendLevelUpObject(id);
		}
		else if (level == 2)
		{
			SetLevel(3);
			SendLevelUpObject(id);
		}
		else
		{
			MaxLevel = true;
		}		
		exp = 0;
	}

	//cout <<id<< "의 경험치 : "<<(int)exp <<", level : " << (int)level <<endl;

	SendExpObejct(id);
}

void cPlayer::SkillSworder()
{
	unordered_set<UINT> mylist = viewlist;

	unordered_set<UINT> target_list;

	for (auto monster : mylist)
	{
		if (monster >= MAX_USER)
		{
			float temp_x, temp_z;
			temp_x = x + sight_x * 30;
			temp_z = z + sight_z * 30;

			float distance = (objects[monster]->GetX() - temp_x)*(objects[monster]->GetX() - temp_x) + (objects[monster]->GetZ() - temp_z)*(objects[monster]->GetZ() - temp_z);

			if (distance < SWORDER_SKILL_RANGE * SWORDER_SKILL_RANGE)
			{
				if (objects[monster]->GetIsUse())
					target_list.insert(monster);
			}
		}
	}

	for (auto target : target_list)
	{
		cMonster* a_monster = dynamic_cast<cMonster*>(objects[target]);

		a_monster->AttackedPlayer(damage, id);

		float move_x, move_z;

		move_x = a_monster->GetX();
		move_z = a_monster->GetZ();

		move_x = move_x + sight_x * 18;
		move_z = move_z + sight_z * 18;

		a_monster->SetX(move_x);
		a_monster->SetZ(move_z);
	}

	mp -= 10;
	SendMoveObject(id);
}

void cPlayer::SkillAssasin()
{
	unordered_set<UINT> mylist = viewlist;

	unordered_set<UINT> target_list;

	for (auto monster : mylist)
	{
		if (monster >= MAX_USER)
		{
			float distance = (objects[monster]->GetX() - x)*(objects[monster]->GetX() - x) + (objects[monster]->GetZ() - z)*(objects[monster]->GetZ() - z);

			if (distance < ASSASIN_SKILL_RANGE * ASSASIN_SKILL_RANGE)
			{
				if (objects[monster]->GetIsUse())
					target_list.insert(monster);
			}
		}
	}

	for (auto target : target_list)
	{
		cMonster* a_monster = dynamic_cast<cMonster*>(objects[target]);

		a_monster->AttackedPlayer(damage, id);
	}

	mp -= 10;
	SendMoveObject(id);
}

void cPlayer::Clear()
{
	unordered_set<UINT> temp = viewlist;

	for (auto send : temp)
	{
		if (send < MAX_USER)
		{

			cPlayer* other = dynamic_cast<cPlayer*>(objects[send]);

			if (other->GetIsUse())
			{
				other->viewMutex.lock();
				if (0 != other->GetViewlist().count(id))
				{
					other->GetViewlist().erase(id);
					SendRemoveObject(send, id);
				}
				other->viewMutex.unlock();
			}
		}
		else
		{
			cMonster* a_monster = dynamic_cast<cMonster*>(objects[send]);

			float distance = (objects[send]->GetX() - x)*(objects[send]->GetX() - x) + (objects[send]->GetZ() - z)*(objects[send]->GetZ() - z);

			if (distance < PLAYER_ATTACK_RANGE * PLAYER_ATTACK_RANGE)
			{
				if (a_monster->GetIsUse())
				{
					a_monster->SetAnimationNum(IDLE);
				}
			}
		}
		SendRemoveObject(id, send);
	}
	viewlist.clear();
	if (my_sector) {
		my_sector->Remove(id);
	}
}
