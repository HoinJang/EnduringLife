#include "stdafx.h"
#include "cThreadFunc.h"

#include "Overlapped_Ex.h"
#include "cMyNamespace.h"

#include "cObject.h"
#include "cPlayer.h"
#include "cMonster.h"

void AcceptThread()
{
	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	auto g_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN bind_addr;
	ZeroMemory(&bind_addr, sizeof(SOCKADDR_IN));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(MY_SERVER_PORT);
	bind_addr.sin_addr.s_addr = INADDR_ANY;

	::bind(g_socket, reinterpret_cast<sockaddr *>(&bind_addr), sizeof(SOCKADDR_IN));

	listen(g_socket, SOMAXCONN);

	UINT new_id = -1;

	while (true) {
		SOCKADDR_IN c_addr;
		ZeroMemory(&bind_addr, sizeof(SOCKADDR_IN));
		c_addr.sin_family = AF_INET;
		c_addr.sin_port = htons(MY_SERVER_PORT);
		c_addr.sin_addr.s_addr = INADDR_ANY;
		int c_length = sizeof(SOCKADDR_IN);

		SOCKET new_socket = WSAAccept(g_socket, reinterpret_cast<sockaddr *>(&c_addr), &c_length, NULL, NULL);

		if (thread_running == false)
		{
			closesocket(new_socket);
			break;
		}

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (objects[i]->GetIsUse() == false)
			{
				new_id = i;

				break;
			}
		}

		if (new_id == -1)
		{
			cout << "설정된 인원 이상으로 접속하여 차단 하였습니다." << endl;
			closesocket(new_socket);
			continue;
		}

		char buf[20];

		int retval = recv(new_socket, buf, 20, 0);
		if (retval == SOCKET_ERROR)
		{
			cout << "Not Recv Game ID : " << new_id << endl;
			closesocket(new_socket);
			continue;
		}
		buf[retval] = '\0';

		char gameid[20];

		strcpy(gameid, buf);

		cout << "Login Game Id : " << gameid << ",  objects_id : " << new_id << endl;

		//ID를 입력받음.
		//case 1. 미접속중인 ID로 로그인 하는경우 => Okay
		//case 2. ID를 새로 만드는경우 => Newid
		//case 3. 이미 ID가 사용중이거나, 접속인원이 가득 차 있을경우 => False
	
		int casenum = 2;
		bool is_ingame = false;
		bool is_inlist = false;
		PLAYER_TYPE playertype;

		for (auto p : Player_List)
		{
			if (strcmp(gameid, p->id) == 0)
			{
				for (int i = 0; i < MAX_USER; ++i)
				{
					cPlayer* player = dynamic_cast<cPlayer*>(objects[i]);

					//cout << gameid << endl;
					//cout << player->GetGameID() << endl;

					if ((strcmp(gameid, player->GetGameID()) == 0) && player->GetIsUse())
					{
						//cout << "같은 아이디 발견" << endl;
						is_ingame = true;
						break;
					}
				}
				is_inlist = true;

				//cout << "같음 : " << gameid << ", " << p->id << endl;
			}
			//cout << "리스트 내 아이디 : " << p->id << endl;
		}

		if (is_ingame && is_inlist)
		{
			casenum = 3;
		}
		if (!is_ingame && is_inlist)
		{
			casenum = 1;
		}
		if (!is_ingame && !is_inlist)
		{
			casenum = 2;
		}

		cout << "case num : " << casenum << endl;

		if (casenum == 1)
		{
			strcpy(buf, "Okay");
			retval = send(new_socket, buf, strlen(buf), 0);
		}
		else if (casenum == 2)
		{
			strcpy(buf, "Newid");
			retval = send(new_socket, buf, strlen(buf), 0);

			retval = recv(new_socket, buf, 20, 0);
			if (retval == SOCKET_ERROR)
			{
				cout << "Not Recv Game ID : " << new_id << endl;
				closesocket(new_socket);
				continue;
			}
			buf[retval] = '\0';

			if (strcmp(buf, "Male") == 0)
			{
				cout << "검사 캐릭터 생성" << endl;
				playertype = MALE;
			}
			else if (strcmp(buf, "FeMale") == 0)
			{
				cout << "마법사 캐릭터 생성" << endl;
				playertype = FEMALE;
			}
		}
		else if (casenum == 3)
		{
			strcpy(buf, "False");
			retval = send(new_socket, buf, strlen(buf), 0);
			closesocket(new_socket);
			continue;
		}


		cPlayer* Player = dynamic_cast<cPlayer*>(objects[new_id]);

		bool IsInList = false;

		for (int i = 0; i < Player_List.size(); ++i)
		{
			if (strcmp(Player_List[i]->id, gameid) == 0)
			{
				char* playerid = new char[20];

				strcpy(playerid, gameid);

				Player->SetGameID(playerid);
				Player->SetSock(new_socket);
				Player->SetID(new_id);
				Player->SetScene(TOWN);
				Player->SetIsUse(true);
				Player->SetLevel(Player_List[i]->level);

				float tempx, tempz;
				float hp, mp;
				sc_packet_put_player p;
				p.id = new_id;
				p.size = sizeof(sc_packet_put_player);
				p.type = SC_PUT_PLAYER;

				tempx = (roundf(objects[new_id]->GetX() * 10) / 10) * 10;
				tempz = (roundf(objects[new_id]->GetZ() * 10) / 10) * 10;
				hp = (roundf(objects[new_id]->GetHp() * 10) / 10) * 10;
				mp = (roundf(objects[new_id]->GetMp() * 10) / 10) * 10;

				p.x = tempx;
				p.y = tempz;
				p.x = tempx;
				p.y = tempz;
				p.hp = hp;
				p.mp = mp;
				p.player_type = (BYTE)(Player_List[i]->type);

				SendPacket(new_id, &p);
				
				//Player->SetLevel(Player_List[i]->level);
				Player->SetPlayerType(Player_List[i]->type);
				Player->SetExp(Player_List[i]->exp);
				Player->SetQuest1(Player_List[i]->quest_1);
				Player->SetQuest2(Player_List[i]->quest_2);
				Player->SetQuest3(Player_List[i]->quest_3);

				cout << gameid << " 로그인 - 레벨 : " << (int)Player->GetLevel() << endl;

				sc_packet_info packet;
				packet.id = new_id;
				packet.size = sizeof(sc_packet_info);
				packet.type = SC_INFO;
				packet.level = Player_List[i]->level;
				packet.exp = Player_List[i]->exp;
				packet.Q1 = Player_List[i]->quest_1;
				packet.Q2 = Player_List[i]->quest_2;
				packet.Q3 = Player_List[i]->quest_3;

				SendPacket(new_id, &packet);


				IsInList = true;
				break;
			}
		}

		if (!IsInList)
		{
			cout << "목록에 없음 - 새로생성 " << endl;

			char* playerid = new char[20];

			strcpy(playerid, gameid);

			Player->SetGameID(playerid);
			Player->SetSock(new_socket);
			Player->SetPlayerType(playertype);
			Player->SetID(new_id);
			Player->SetScene(TOWN);
			Player->SetIsUse(true);
			Player->SetLevel(1);

			float tempx, tempz;
			float hp, mp;
			sc_packet_put_player p;
			p.id = new_id;
			p.size = sizeof(sc_packet_put_player);
			p.type = SC_PUT_PLAYER;

			tempx = (roundf(objects[new_id]->GetX() * 10) / 10) * 10;
			tempz = (roundf(objects[new_id]->GetZ() * 10) / 10) * 10;
			hp = (roundf(objects[new_id]->GetHp() * 10) / 10) * 10;
			mp = (roundf(objects[new_id]->GetMp() * 10) / 10) * 10;
		
			p.x = tempx;
			p.y = tempz;

			p.x = tempx;
			p.y = tempz;
			p.hp = hp;
			p.mp = mp;
			p.player_type = (BYTE)playertype;

			SendPacket(new_id, &p);

			Player->SetLevel(1);
			Player->SetExp(0);
			Player->SetQuest1(false);
			Player->SetQuest2(false);
			Player->SetQuest3(false);

			sc_packet_info packet;
			packet.id = new_id;
			packet.size = sizeof(sc_packet_info);
			packet.type = SC_INFO;
			packet.level = 1;
			packet.exp = 0;
			packet.Q1 = false;
			packet.Q2 = false;
			packet.Q3 = false;

			SendPacket(new_id, &packet);

			Player_List.push_back(new PlayerList{ playerid, playertype, 1, 0, false, false, false });
		}


		cout << new_id << " client enter" << endl;

		
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_socket), g_iocp, new_id, 0);

		//초기화 한 정보를 자신에게 보내줌	

		//나의 뷰리스트를 작성, 내 뷰리스트에 들어온 플레이어들에게 내 정보를 전달.
		UpdataPlayerView(new_id);

		Player->RecvPacket();
		++new_id;
		// 나에게 접속중인 다른 플레이어 정보를 전송
	}

	closesocket(g_socket);
	WSACleanup();

	cout << " exit accept thread " << endl;
}

void WorkerThread()
{
	while (true) {
		unsigned long data_size;
		unsigned long long key; //unsigend long long 
		EXOver *p_over;

		BOOL is_success = GetQueuedCompletionStatus(g_iocp, &data_size, &key, reinterpret_cast<LPWSAOVERLAPPED *>(&p_over), INFINITE);
		
		//cout << key << endl;
		//WorkerThread 종료
		if (thread_running == false)
			break;
		// 에러 처리
		if (0 == is_success) {
			cout << "Error in GQCS key[" << key << "]\n";
		}
		// 접속종료 처리
		if (0 == data_size)
		{
			DisconnectPlayer(key);
			continue;
		}
		// Send/Recv 처리
		//EXOver *o = reinterpret_cast<EXOver *>(p_over);
		if (E_RECV == p_over->command)
		{
			AfterRecv(key, data_size);
		}
		else if (E_SEND == p_over->command)
		{
			//cout << "Send Complete to Client : " << key << std::endl;
			delete p_over;
		}
		else if (NPC_MOVE == p_over->command)
		{
			cMonster* monster = dynamic_cast<cMonster*>(objects[key]);

			if (monster->GetIsUse() == false)
				continue;
			monster->HeartBeat();
			delete p_over;
		}
		else if (NPC_HIT_ON == p_over->command)
		{
			cMonster* monster = dynamic_cast<cMonster*>(objects[key]);

			if (monster->GetIsUse() == false)
				continue;
			monster->SetHitFalse();
			monster->HeartBeat();
			delete p_over;
		}
		else if (NPC_RECREATE == p_over->command)
		{
			cMonster* monster = dynamic_cast<cMonster*>(objects[key]);
			
			monster->SetID(key);
			monster->SetIsUse(true);
			monster->SetScene(FIELD);

			if (monster->GetMonsterType() == TYPE1)
			{
				monster->SetHp(200);
			}
			else if (monster->GetMonsterType() == TYPE2)
			{
				monster->SetHp(500);
			}
			else if (monster->GetMonsterType() == TYPE3)
			{
				monster->SetHp(2000);
			}
			monster->SetRecreateXY();

			monster->SetAnimationNum(IDLE);

			g_scene.UpdateZone(key);

			monster->NearlistClear();

			monster->WakeUpMonster();

			delete p_over;
		}
		else
		{
			std::cout << "Unknown GQCS Event Type!\n";
			exit(-1);
		}
	}
	cout << " exit worker thread " << endl;
}

void TimerThread()
{
	while (1)
	{
		Sleep(10);

		if (thread_running == false)
			break;

		timerMutex.lock();
		if (event_queue.empty())
		{
			timerMutex.unlock();
			continue;
		}

		NPC_EVENT top_event = event_queue.top();

		while (top_event.time <= GetCurrTime())
		{
			event_queue.pop();
			timerMutex.unlock();

			//이벤트 처리
			ProcessEvent(top_event);
			//cout << top_event.time << endl;
			//cout << top_event.id << endl;

			timerMutex.lock();
			if (event_queue.empty())
			{
				timerMutex.unlock();
				break;
			}
			top_event = event_queue.top();
		}
		timerMutex.unlock();
	}
	cout << " exit timer thread " << endl;

}

void AfterRecv(const UINT id, const int transferred)
{
	cPlayer* player = dynamic_cast<cPlayer*>(objects[id]);

	player->ReadPacket(id, transferred);
	player->RecvPacket();
}

void ProcessPacket(const UINT cl, BYTE * packet)
{
	int packet_type = 0;

	switch (packet[1])
	{
	case CS_UP:
	case CS_DOWN:
	case CS_LEFT:
	case CS_RIGHT:
	case CS_TELEPORT:
	case CS_SKILL_MOVE:
		packet_type = 0;
		break;
	case CS_SIGHT:
		packet_type = 1;
		break;
	case CS_ATTACK:
		packet_type = 2;
		break;
	case CS_ANIMATE:
		packet_type = 3;
		break;
	case CS_QUEST_OK:
		packet_type = 4;
		break;
	case CS_SKILL:
		packet_type = 5;
		break;
	default:
		cout << "정의 되지 않은 패킷이 전송 되었습니다." << endl;
		break;
	}
	//스킬 패킷
	if (packet_type == 5)
	{
		//cout << cl << "의 스킬패킷을 받았습니다" << endl;

		cPlayer* player = dynamic_cast<cPlayer*>(objects[cl]);

		if (objects[cl]->GetPlayerType() == MALE)
		{
			player->SkillSworder();
		}
		else if (objects[cl]->GetPlayerType() == FEMALE)
		{
			player->SkillAssasin();
		}

		return;
	}
	// 퀘스트 완료 패킷
	if (packet_type == 4)
	{
		cs_packet_quest_ok *p = reinterpret_cast<cs_packet_quest_ok *>(packet);

		//cout << "퀘스트 패킷 도착" << endl;
		//cout << "몇 번째 퀘스트 도착 : " << p->quest_num << endl;

		cPlayer* player = dynamic_cast<cPlayer*>(objects[cl]);

		BYTE temp = p->quest_num;

		if (temp == 1)
		{
			player->SetQuest1(true);
		}
		else if (temp == 2)
		{
			player->SetQuest2(true);
		}
		else if (temp == 3)
		{
			player->SetQuest3(true);
			player->SetQuestEnd(true);
		}
		player->ExpPlus(200);
		player->SetQuestNow(temp++);

		return;
	}
	// 애니메이션 패킷 
	if (packet_type == 3)
	{
		cs_packet_animate * p = reinterpret_cast<cs_packet_animate *>(packet);
		
		//cout <<cl <<"의 애니메이션 패킷 : " << (int)p->animate_num << endl;

		objects[cl]->SetAnimationNum(p->animate_num);

		cPlayer* player = dynamic_cast<cPlayer*>(objects[cl]);

		unordered_set<UINT> temp;

		temp = player->GetViewlist();

		for (auto other : temp)
		{
			if (other < MAX_USER)
			{
				SendAnimateObject(other, cl);
			}
		}

		if (p->animate_num == SKILL)
		{
			unordered_set<UINT> temp;

			temp = player->GetViewlist();

			for (auto p : temp)
			{
				if (p < MAX_USER)
				{
					if (player->GetPlayerType() == MALE)
						SendEffectObject(p, cl, SWORD_SKILL);
					else if (player->GetPlayerType() == FEMALE)
						SendEffectObject(p, cl, ASSASIN_SKILL);
				}
			}

			if (player->GetPlayerType() == MALE)
				SendEffectObject(cl, cl, SWORD_SKILL);
			else if (player->GetPlayerType() == FEMALE)
				SendEffectObject(cl, cl, ASSASIN_SKILL);
		}

		return;
	}
	// 공격 패킷
	if (packet_type == 2)
	{
		//cout << cl << "의 공격패킷을 받았습니다" << endl;

		cPlayer* player = dynamic_cast<cPlayer*>(objects[cl]);
		//mp바 테스트
	
		//cout << mp << endl;

		//
		player->AttackMonster();

		return;
	}
	// 시야 패킷
	if (packet_type == 1) 
	{
		//cout << "시야 패킷 받앗습니다 "<< endl;
		cs_packet_sight * p = reinterpret_cast<cs_packet_sight *>(packet);

		//cout << p->dir_x << ", " << p->dir_z << endl;

		float dirX = p->dir_x * 0.01f; //플레이어의 방향벡터 x축
		float dirZ = p->dir_z * 0.01f; //플레이어의 방향벡터 z축

		objects[cl]->SetSightX(dirX);
		objects[cl]->SetSightZ(dirZ);

		cPlayer* player = dynamic_cast<cPlayer*>(objects[cl]);

		unordered_set<UINT> temp;

		temp = player->GetViewlist();

		for (auto other : temp)
		{
			if (other < MAX_USER)
			{
				SendSightObject(other, cl);
			}
		}
		return;
	}

	cs_packet_move *p = reinterpret_cast<cs_packet_move *>(packet);

	float x = objects[cl]->GetX();
	float y = objects[cl]->GetZ();

	float dirX = p->dir_x * 0.01f; //플레이어의 방향벡터 x축
	float dirZ = p->dir_z * 0.01f; //플레이어의 방향벡터 z축

	switch (p->type)
	{
	case CS_UP:
		x = x + dirX * OBJECT_SPEED;
		y = y + dirZ * OBJECT_SPEED;
		break;
	case CS_DOWN:
		x = x - dirX * OBJECT_SPEED;
		y = y - dirZ * OBJECT_SPEED;
		break;
	case CS_LEFT:
		x = x - dirZ * OBJECT_SPEED;
		y = y + dirX * OBJECT_SPEED;
		break;
	case CS_RIGHT:
		x = x + dirZ * OBJECT_SPEED;
		y = y - dirX * OBJECT_SPEED;
		break;
	case CS_TELEPORT:
		x = x + dirX * OBJECT_SPEED * 12;
		y = y + dirZ * OBJECT_SPEED * 12;
		break;
	case CS_SKILL_MOVE:
		x = x + dirX * OBJECT_SPEED * 18;
		y = y + dirZ * OBJECT_SPEED * 18;
		break;

	default:
		cout << "UnKnown Protocol from Client [" << cl << "]" << endl;
	}
	// cout << x << " , " << y << endl;
	// 이동 후에 맵 내에 존재하면.


	//포탈
	if (CollisionCheck(x, y, BOARD_WIDTH / 2, 60, 5, 5) && objects[cl]->GetScene() == TOWN)
	{
		cPlayer* player = dynamic_cast<cPlayer*>(objects[cl]);

		player->SetLevel(player->GetLevel());
		/*
		cout << "충돌했어" << endl;
		*/
		objects[cl]->SetScene(FIELD);
		objects[cl]->SetX(200);
		objects[cl]->SetZ(890);
	
	
		objects[cl]->SetSightX(dirX);
		objects[cl]->SetSightZ(dirZ);
		//포탈에 들어가면

		UpdataPlayerView(cl);

		SendSceneObject(cl, objects[cl]->GetScene());

		SendMoveObject(cl);

		return;
	}
	if (CollisionCheck(x, y, 150, 900, 5, 5) && objects[cl]->GetScene() == FIELD)
	{
		/*
		cout << "충돌했어" << endl;
		*/
		cPlayer* player = dynamic_cast<cPlayer*>(objects[cl]);

		player->SetLevel(player->GetLevel());

		objects[cl]->SetScene(TOWN);
		objects[cl]->SetX(BOARD_WIDTH / 2);
		objects[cl]->SetZ(100);
		SendSceneObject(cl, objects[cl]->GetScene());
	

		objects[cl]->SetSightX(dirX);
		objects[cl]->SetSightZ(dirZ);
		//포탈에 들어가면

		UpdataPlayerView(cl);

		SendMoveObject(cl);

		return;
	}

	if (x >= 0.0f && x < BOARD_WIDTH && y >= 0.0f && y < BOARD_WIDTH && objects[cl]->GetScene() == FIELD)
	{
		cPlayer* player = dynamic_cast<cPlayer*>(objects[cl]);

		unordered_set<UINT> temp;

		temp = player->GetViewlist();

		for (auto other : temp)
		{
			if (objects[other]->GetIsUse())
			{
				if (CollisionCheck(player->GetX(), player->GetZ(), objects[other]->GetX(), objects[other]->GetZ(), 5.0, 5.0))
				{
					//cout << player->GetID() << "번 플레이어가 " << objects[other]->GetID() << "번 오브젝트에게 충돌" << endl;
					x = x - 2 * dirX * OBJECT_SPEED;
					y = y - 2 * dirZ * OBJECT_SPEED;
					break;
				}
			}
		}
		for (auto check : Field_Line)
		{
			if (check->IsInLine(objects[cl]->GetX(), objects[cl]->GetZ()))
			{
				float slide_vec_x = objects[cl]->GetSightX() - check->GetNormal_X()*((objects[cl]->GetSightX()*check->GetNormal_X()) + (objects[cl]->GetSightZ()*check->GetNormal_Y()));
				float slide_vec_z = objects[cl]->GetSightZ() - check->GetNormal_Y()*((objects[cl]->GetSightX()*check->GetNormal_X()) + (objects[cl]->GetSightZ()*check->GetNormal_Y()));

				x = x - dirX * OBJECT_SPEED;
				y = y - dirZ * OBJECT_SPEED;

				dirX = slide_vec_x;
				dirZ = slide_vec_z;

				x = x + dirX * OBJECT_SPEED;
				y = y + dirZ * OBJECT_SPEED;


				break;
			}
		}
		objects[cl]->SetX(x);
		objects[cl]->SetZ(y);
	}
	else if (x >= 0.0f && x < BOARD_WIDTH && y >= 0.0f && y < BOARD_WIDTH && objects[cl]->GetScene() == TOWN)
	{
		cPlayer* player = dynamic_cast<cPlayer*>(objects[cl]);

		unordered_set<UINT> temp;

		temp = player->GetViewlist();

		for (auto other : temp)
		{
			if (objects[other]->GetIsUse())
			{
				if (CollisionCheck(player->GetX(), player->GetZ(), objects[other]->GetX(), objects[other]->GetZ(), 5.0, 5.0))
				{
					x = x - 2 * dirX * OBJECT_SPEED;
					y = y - 2 * dirZ * OBJECT_SPEED;
					break;
				}
			}
		}

		for (auto check : Town_Line)
		{
			if (check->IsInLine(objects[cl]->GetX(), objects[cl]->GetZ()))
			{
				float slide_vec_x = objects[cl]->GetSightX() - check->GetNormal_X()*((objects[cl]->GetSightX()*check->GetNormal_X()) + (objects[cl]->GetSightZ()*check->GetNormal_Y()));
				float slide_vec_z = objects[cl]->GetSightZ() - check->GetNormal_Y()*((objects[cl]->GetSightX()*check->GetNormal_X()) + (objects[cl]->GetSightZ()*check->GetNormal_Y()));

				x = x - dirX * OBJECT_SPEED;
				y = y - dirZ * OBJECT_SPEED;

				dirX = slide_vec_x;
				dirZ = slide_vec_z;

				x = x + dirX * OBJECT_SPEED;
				y = y + dirZ * OBJECT_SPEED;

				break;
			}
		}

		objects[cl]->SetX(x);
		objects[cl]->SetZ(y);
	}
	else
	{
			cout << "맵을 벗어났습니다" << endl;
	}

	objects[cl]->SetSightX(dirX);
	objects[cl]->SetSightZ(dirZ);

	UpdataPlayerView(cl);

	SendMoveObject(cl);
}

void ProcessEvent(NPC_EVENT& event_object)
{
	EXOver *over_ex = new EXOver;

	if (event_object.type == NPC_MOVE)
		over_ex->command = NPC_MOVE;
	if (event_object.type == NPC_RECREATE)
		over_ex->command = NPC_RECREATE;
	if (event_object.type == NPC_HIT_ON)
		over_ex->command = NPC_HIT_ON;


	PostQueuedCompletionStatus(g_iocp, 1, event_object.id, (LPOVERLAPPED)&over_ex->wsaover);
}

void DisconnectPlayer(UINT cl)
{
	cPlayer* player = dynamic_cast<cPlayer*>(objects[cl]);

	//패킷 보내주기전에 내가 viewlist에 있는 오브젝트들의 viewlist에서 나를 제거시켜줘야한다.

	player->SetX(BOARD_WIDTH / 2);
	player->SetZ(BOARD_WIDTH / 2);
	player->SetScene(TOWN);

	for (auto p : Player_List)
	{
		if (strcmp(p->id, player->GetGameID()) == 0)
		{
			p->exp = player->GetExp();
			p->level = player->GetLevel();
			p->quest_1 = player->GetQuest1();
			p->quest_2 = player->GetQuest2();
			p->quest_3 = player->GetQuest3();
		}
	}
	if (player->GetIsUse())
	{
		closesocket(player->GetSock());
		player->Clear();
		player->SetIsUse(false);
		player->SetGameID((char*)"");
		cout << "Client [" << cl << "] Disconnected \n";
	}
}

void SendPacket(UINT cl, void * packet)
{
	EXOver *o = new EXOver;
	char *p = reinterpret_cast<char *>(packet);
	memcpy(o->io_buf, p, p[0]);
	o->command = E_SEND;
	o->wsabuf.buf = o->io_buf;
	o->wsabuf.len = p[0];
	ZeroMemory(&o->wsaover, sizeof(WSAOVERLAPPED));

	int ret = WSASend(dynamic_cast<cPlayer*>(objects[cl])->GetSock(), &o->wsabuf, 1, NULL, 0, &o->wsaover, NULL);
	if (0 != ret)
	{
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			Errordisplay("Error : ", err_no);
	}
}

void SendSceneObject(int client, SCENE scene)
{
	sc_packet_scene_player p;
	p.id = client;
	p.size = sizeof(sc_packet_scene_player);
	p.type = SC_SCENE;
	p.scene = scene;

	SendPacket(client, &p);
}

void SendMoveObject(int client, int object_id)
{
	float tempx, tempz;
	float sightx, sightz;
	float hp, mp;
	sc_packet_pos p;
	p.id = object_id;
	p.size = sizeof(sc_packet_pos);
	p.type = SC_POS;

	tempx = (roundf(objects[object_id]->GetX() * 10) / 10) * 10;
	tempz = (roundf(objects[object_id]->GetZ() * 10) / 10) * 10;
	
	sightx = (roundf(objects[object_id]->GetSightX() * 100) / 100) * 100;
	sightz = (roundf(objects[object_id]->GetSightZ() * 100) / 100) * 100;

	hp = (roundf(objects[object_id]->GetHp() * 10) / 10) * 10;
	mp = (roundf(objects[object_id]->GetMp() * 10) / 10) * 10;
	p.x = tempx;
	p.y = tempz;
	p.sight_x = sightx;
	p.sight_z = sightz;
	p.hp = hp;
	p.mp = mp;

	SendPacket(client, &p);
}

void SendMoveObject(int client)
{
	float tempx, tempz;
	float sightx, sightz;
	float hp, mp;

	sc_packet_pos p;
	p.id = client;
	p.size = sizeof(sc_packet_pos);
	p.type = SC_POS;

	tempx = (roundf(objects[client]->GetX() * 10) / 10) * 10;
	tempz = (roundf(objects[client]->GetZ() * 10) / 10) * 10;

	sightx = (roundf(objects[client]->GetSightX() * 100) / 100) * 100;
	sightz = (roundf(objects[client]->GetSightZ() * 100) / 100) * 100;

	hp = (roundf(objects[client]->GetHp() * 10) / 10) * 10;
	mp = (roundf(objects[client]->GetMp() * 10) / 10) * 10;

	p.x = tempx;
	p.y = tempz;
	p.sight_x = sightx;
	p.sight_z = sightz;
	p.hp = hp;
	p.mp = mp;

	SendPacket(client, &p);
}

void SendSightObject(int client, int object_id)
{
	float sightx, sightz;

	sc_packet_sight p;
	p.id = object_id;
	p.size = sizeof(sc_packet_sight);
	p.type = SC_SIGHT;

	sightx = (roundf(objects[object_id]->GetSightX() * 100) / 100) * 100;
	sightz = (roundf(objects[object_id]->GetSightZ() * 100) / 100) * 100;

	p.sight_x = sightx;
	p.sight_z = sightz;

	SendPacket(client, &p);
}

void SendPutObject(int client, int object_id) {

	float tempx, tempz;
	float sightx, sightz;
	float hp, mp;

	sc_packet_put_player p;
	p.id = object_id;
	p.size = sizeof(p);
	p.type = SC_PUT_PLAYER;

	tempx = (roundf(objects[object_id]->GetX() * 10) / 10) * 10;
	tempz = (roundf(objects[object_id]->GetZ() * 10) / 10) * 10;

	sightx = (roundf(objects[object_id]->GetSightX() * 100) / 100) * 100;
	sightz = (roundf(objects[object_id]->GetSightZ() * 100) / 100) * 100;

	hp = (roundf(objects[object_id]->GetHp() * 10) / 10) * 10;
	mp = (roundf(objects[object_id]->GetMp() * 10) / 10) * 10;
	p.x = tempx;
	p.y = tempz;
	p.sight_x = sightx;
	p.sight_z = sightz;
	p.hp = hp;
	p.mp = mp;
	p.animation_num = objects[object_id]->GetAnimationNum();
	p.player_type = objects[object_id]->GetPlayerType();

	SendPacket(client, &p);
}

void SendRemoveObject(int client, int object_id)
{
	float hp;

	hp = (roundf(objects[object_id]->GetHp() * 10) / 10) * 10;

	sc_packet_remove_player p;
	p.id = object_id;
	p.size = sizeof(p);
	p.type = SC_REMOVE_PLAYER;
	p.hp = hp;

	SendPacket(client, &p);
}

void SendAnimateObject(int client, int object_id)
{
	sc_packet_animate p;
	p.id = object_id;
	p.size = sizeof(p);
	p.type = SC_ANIMATE;
	p.animate_num = objects[object_id]->GetAnimationNum();

	SendPacket(client, &p);
}

void SendEffectObject(int client, int object_id, EFFECT_TYPE type)
{
	sc_packet_effect p;
	p.id = object_id;
	p.size = sizeof(p);
	p.type = SC_EFFECT;
	p.effect = type;

	SendPacket(client, &p);
}

void SendExpObejct(int client)
{
	sc_packet_exp p;
	p.id = client;
	p.size = sizeof(p);
	p.type = SC_EXP;

	cPlayer* player = dynamic_cast<cPlayer*>(objects[client]);

	p.exp = player->GetExp();

	SendPacket(client, &p);
}

void SendLevelUpObject(int client)
{
	sc_packet_levelup p;
	p.id = client;
	p.size = sizeof(p);
	p.type = SC_LEVELUP;

	cPlayer* player = dynamic_cast<cPlayer*>(objects[client]);

	p.level = player->GetLevel();
	
	SendPacket(client, &p);

}

void Errordisplay(const char * msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wprintf(L"에러%s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void UpdataPlayerView(const UINT id)
{
	cPlayer* player = dynamic_cast<cPlayer*>(objects[id]);
	
	g_scene.UpdateZone(id);

	player->UpdatesSectorSearch();
	player->UpdateViewList();
}

void AddTimer(const UINT id, const int type, const UINT time)
{
	timerQueueMutex.lock();
	NPC_EVENT new_event;

	new_event.id = id;
	new_event.type = type;
	new_event.time = GetCurrTime() + time;

	event_queue.push(new_event);
	timerQueueMutex.unlock();
}

static UINT GetCurrTime()
{
	static auto start = high_resolution_clock::now();

	return duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
}

bool CollisionCheck(float x1, float y1, float x2, float y2, float size1, float size2)
{
	if (x1 - size1 / 2 > x2 + size2 / 2)
		return false;
	if (x1 + size1 / 2 < x2 - size2 / 2)
	 	return false;
	if (y1 + size1 / 2 < y2 - size2 / 2)
	 	return false;
	if (y1 - size1 / 2 > y2 + size2 / 2)
	 	return false;
	return true;
}

