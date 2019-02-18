#pragma once

void AcceptThread();

void WorkerThread();

void TimerThread();

void AfterRecv(const UINT id, const int transferred);

void ProcessPacket(UINT cl, BYTE * packet);

void ProcessEvent(NPC_EVENT& event_object);

void DisconnectPlayer(UINT cl);

void SendPacket(UINT cl, void * packet);

void Errordisplay(const char * msg, int err_no);

void SendSceneObject(int client, SCENE scene);

void SendMoveObject(int client, int object_id);

void SendMoveObject(int client);

void SendSightObject(int client, int object_id);

void SendPutObject(int client, int object_id);

void SendRemoveObject(int client, int object_id);

void SendAnimateObject(int client, int object_id);

void SendEffectObject(int client, int object_id, EFFECT_TYPE type);

void SendExpObejct(int client);

void SendLevelUpObject(int client);

void UpdataPlayerView(const UINT id);

void AddTimer(const UINT id, const int type, const UINT time);

static UINT GetCurrTime();

bool CollisionCheck(float x1, float y1, float x2, float y2, float size1, float size2);
