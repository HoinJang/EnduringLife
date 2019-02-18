#include "stdafx.h"
#include "cServer.h"

#include "cPlayer.h"
#include "cMonster.h"

#include "cThreadFunc.h"
#include "cMyNamespace.h"


cServer::cServer()
{
	Initialize();
}

cServer::~cServer()
{
	Release();
}

void cServer::Initialize()
{
	ReadPlayerList();
	srand(unsigned(time(NULL)));

	wcout.imbue(locale("korean"));
	wcout << L" 서버 시작!\n ";
	thread_running = true;
	SYSTEM_INFO systeminfo;
	GetSystemInfo(&systeminfo);
	cpuNumber = systeminfo.dwNumberOfProcessors;

	cout << "WorkerThread 갯수 : " << cpuNumber << endl;
	//CPU 갯수는 코어갯수의 2배

	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);


	for (int i = 0; i < MAX_OBJECT_INDEX ; ++i)
	{
		if (i < MAX_USER)
		{
			objects[i] = new cPlayer();
			objects[i]->SetAnimationNum(0);
			cPlayer* player = dynamic_cast<cPlayer*>(objects[i]);
			player->SetLevel(1);
		}
		else if (i >= MAX_USER && i < 40) //소형 몬스터
		{
			objects[i] = new cMonster();
			objects[i]->SetID(i);
			objects[i]->SetIsUse(true);
			objects[i]->SetScene(FIELD);
			objects[i]->SetHp(200.0f);
			objects[i]->SetSightX(-1.0f);
			objects[i]->SetSightZ(0.0f);

			cMonster* monster = dynamic_cast<cMonster*>(objects[i]);

			if (i >= 30)
			{
				objects[i]->SetX((rand() % 820) + 110);
				objects[i]->SetZ((rand() % 120) + 100);
			}
			else
			{
				objects[i]->SetX((rand() % 620) + 310);
				objects[i]->SetZ((rand() % 100) + 840);
			}

			monster->SetInitX(monster->GetX());
			monster->SetInitZ(monster->GetZ());
			monster->SetType(TYPE1);
			monster->SetDamage(10);
			monster->SetAttackSpeed(4);

			g_scene.UpdateZone(i);
		}
		else if(i>=40 && i < 50) // 중형 몬스터
		{ 
			objects[i] = new cMonster();
			objects[i]->SetID(i);				
			objects[i]->SetX((rand() % 820) + 110);
			objects[i]->SetZ((rand() % 120) + 420);
			objects[i]->SetIsUse(true);
			objects[i]->SetScene(FIELD);
			objects[i]->SetHp(500.0f);
			objects[i]->SetSightX(1.0f);
			objects[i]->SetSightZ(0.0f);

			cMonster* monster = dynamic_cast<cMonster*>(objects[i]);

			monster->SetInitX(monster->GetX());
			monster->SetInitZ(monster->GetZ());
			monster->SetType(TYPE2);
			monster->SetDamage(70);
			monster->SetAttackSpeed(7);

			g_scene.UpdateZone(i);
		}
		else // 대형 몬스터
		{
			objects[i] = new cMonster();
			objects[i]->SetID(i);
			objects[i]->SetX(860);
			objects[i]->SetZ(175);
			objects[i]->SetSightX(-1.0f);
			objects[i]->SetSightZ(0.0f);
			objects[i]->SetIsUse(true);
			objects[i]->SetScene(FIELD);
			objects[i]->SetHp(2000.0f);

			cMonster* monster = dynamic_cast<cMonster*>(objects[i]);

			monster->SetInitX(monster->GetX());
			monster->SetInitZ(monster->GetZ());
			monster->SetType(TYPE3);
			monster->SetDamage(200);
			monster->SetAttackSpeed(5);

			g_scene.UpdateZone(i);

		}
	}

	Town_Line.push_back(new cLine(5.0f, 5.0f, 1015.0f, 5.0f)); //하단
	Town_Line.push_back(new cLine(790.0f, 335.0f, 790.0f, 730.0f)); //오른쪽
	Town_Line.push_back(new cLine(790.0f, 730.0f, 255.0f, 730.0f)); //상단
	Town_Line.push_back(new cLine(255.0f, 730.0f, 255.0f, 335.0f)); //왼쪽
	Town_Line.push_back(new cLine(255.0f, 335.0f, 460.0f, 335.0f)); //하단1
	Town_Line.push_back(new cLine(560.0f, 335.0f, 790.0f, 335.0f)); //하단2
	Town_Line.push_back(new cLine(460.0f, 90.0f, 5.0f, 90.0f)); //상단1
	Town_Line.push_back(new cLine(1015.0f, 90.0f, 560.0f, 90.0f)); //상단2
	Town_Line.push_back(new cLine(560.0f, 90.0f, 560.0f, 335.0f)); //오른쪽 1
	Town_Line.push_back(new cLine(460.0f, 335.0f, 460.0f, 90.0f)); //왼쪽 1
	Town_Line.push_back(new cLine(1015.0f, 5.0f, 1015.0f, 90.0f)); //하단 오른쪽
	Town_Line.push_back(new cLine(5.0f, 90.0f, 5.0f, 5.0f)); //하단 왼쪽

	Town_Line.push_back(new cLine(530.0f, 585.0f, 495.0f, 585.0f));
	Town_Line.push_back(new cLine(495.0f, 620.0f, 530.0f, 620.0f));
	Town_Line.push_back(new cLine(495.0f, 585.0f, 495.0f, 620.0f));
	Town_Line.push_back(new cLine(530.0f, 620.0f, 530.0f, 585.0f));

	Field_Line.push_back(new cLine(110.0f, 100.0f, 930.0f, 100.0f));
	Field_Line.push_back(new cLine(930.0f, 100.0f, 930.f, 950.0f));
	Field_Line.push_back(new cLine(930.0f, 950.0f, 110.0f, 950.0f));
	Field_Line.push_back(new cLine(110.0f, 950.0f, 110.0f, 100.0f));

	Field_Line.push_back(new cLine(110.0f, 830.0f, 805.0f, 830.0f));
	Field_Line.push_back(new cLine(805.0f, 830.0f, 805.0f, 670.0f));
	Field_Line.push_back(new cLine(805.0f, 670.0f, 110.0f, 670.0f));
	
	Field_Line.push_back(new cLine(230.0f, 420.0f, 930.0f, 420.0f));
	Field_Line.push_back(new cLine(230.0f, 250.0f, 230.0f, 420.0f));
	Field_Line.push_back(new cLine(930.0f, 250.0f, 230.0f, 250.0f));


	a_thread = thread{ AcceptThread };

	for (unsigned int i = 0; i < cpuNumber; ++i)
		w_threads.push_back(new thread{ WorkerThread });

	t_thread = thread{ TimerThread };
}

void cServer::Progress()
{
	cout << " 서버 종료 [y/Y] " << endl;

	int key;

	while (true)
	{
		if (_kbhit() != false)
		{
			key = _getch();

			if (key == 'y' || key == 'Y')
			{
				thread_running = false;
				break;
			}
		}
	}
	SYSTEM_INFO systeminfo;
	GetSystemInfo(&systeminfo);
	DWORD cpu_count = systeminfo.dwNumberOfProcessors;

	// worker_thread 종료
	for (int i = 0; i < cpuNumber; ++i)
		PostQueuedCompletionStatus(g_iocp, 0, 0, nullptr);

	// accept_thread 종료
	while (true)
	{
		SOCKET tmp_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

		SOCKADDR_IN server_addr;
		ZeroMemory(&server_addr, sizeof(SOCKADDR_IN));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(MY_SERVER_PORT);
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		int retval = WSAConnect(tmp_sock, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(server_addr), NULL, NULL, NULL, NULL);
		if (SOCKET_ERROR != retval)
			break;
	}
}

void cServer::Release()
{
	SavePlayerList();
	
	a_thread.join();

	for (auto worker : w_threads)
	{
		worker->join();
		delete worker;
	}
	w_threads.clear();

	t_thread.join();

	for (auto object : objects)
	{
		delete object.second;
		object.second = nullptr;
	}
	objects.clear();
}

void cServer::ReadPlayerList()
{
	ifstream file;

	file.open("PlayerList.txt");

	if (file.is_open() == false)
	{
		cout << "파일을 열지 못했습니다" << endl;
	}
	while(!file.eof())
	{
		char* id = new char[20];
		int level;
		int exp;
		int type;
		bool quest_1;
		bool quest_2;
		bool quest_3;
		
		file >> id >> type >> level >> exp >> quest_1 >> quest_2 >> quest_3;

		Player_List.push_back(new PlayerList{ id, (PLAYER_TYPE)type ,level, exp, quest_1, quest_2, quest_3 });
	}

	//for (auto list : player_list)
	//{
	//	cout << list->id << "," << list->level << "," << list->exp << ", " << list->quest_1 << list->quest_2 << list->quest_3 << endl;
	//}

	if (file.is_open() == true)
	{
		file.close();
	}
}

void cServer::SavePlayerList()
{
	int num = Player_List.size();
	int count = 0;

	ofstream file;

	file.open("PlayerList.txt");

	for (auto list : Player_List)
	{
		file << list->id << "\t" << list->type << "\t"<<list->level << "\t" << list->exp << "\t" << list->quest_1 << "\t"
			<< list->quest_2 << "\t" << list->quest_3;

		count++;
		if(count != num)
			file << endl;
	}

	if (file.is_open() == true)
	{
		file.close();
	}
}
