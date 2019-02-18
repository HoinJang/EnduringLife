#pragma once

//���������� �����ϴ� ����
#include "cObject.h"
#include "cZone.h"
#include "cLine.h"


extern map<UINT, cObject*> objects;
extern vector<cLine*> Field_Line;
extern vector<cLine*> Town_Line;
extern vector<PlayerList*> Player_List;

extern volatile bool   thread_running;
extern volatile HANDLE g_iocp;
extern cScene g_scene;

extern mutex timerMutex;
extern mutex timerQueueMutex;
extern priority_queue<NPC_EVENT, vector<NPC_EVENT>, Comparison> event_queue;