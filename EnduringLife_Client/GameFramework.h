#pragma once

#include "Timer.h"
#include "Player.h"
#include "Scene.h"
#include "Object.h"
#include "TitleScene.h"
#include "TownScene.h"
#include "FieldScene.h"
#include "MyDefine.h"
#include "Sound.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	void CreateCommandQueueAndList();

	void OnResizeBackBuffers();

    void BuildObjects();
	void BuildShaders();

	CShader**					m_ppShaders = NULL;
	int								m_nShaders = 0;
	CGameObject**			m_ppObjects = NULL;
	int								m_nObjects = 0;

	//-------------------플레이어 정보------------------------
	float						m_currHp = 0;
	float						m_currMp = 100;
	float						m_currExp = 0;
	bool							m_bDamage = false;
	bool							m_bEffect[MAX_USER] = { false, };

	float						m_Level = 1;
	float						m_MaxHP = 0;
	float						m_MaxMP = 1000;
	float						m_MaxEXP = 1200;
	
	void SetLevelDesign(int lv);


	virtual void SetEffectStart(int idx, bool bEffect) { m_bEffect[idx] = bEffect; }
	virtual bool GetEffectStart(int idx) { return m_bEffect[idx]; }
	virtual void SetEffectPosition(int idx, XMFLOAT3 pos);
	virtual void SetHP(float hp) { m_currHp = hp;	m_bDamage = true; }
	virtual void SetMP(float mp) { m_currMp = mp; }
	//------------------사운드---------------------------
	vector<CSound*>	 m_vSounds;
	void CreateSound(HWND * hwnd);
	void SoundManager(int id, BYTE effect);
	//-------------------퀘스트 정보------------------------
	int							m_CurrQuestIdx = 0;	   //수락 버튼을 누르면 +1, 퀘스트 완료하면  +1
	bool							m_bNpcContact = false;
	bool							m_bQuestING = false;
	int							m_QuestProgressDegree = 0; //퀘스트 진척도 (몇마리 잡았는지, 아이템 몇 개 먹었는지)
	bool							m_bItem = false;
	int							m_nQuestClear = 0;

	bool CheckItemCollision(XMFLOAT3 ItemPos);

	bool CheckNPCCollision();
	bool CheckMouseOver();
	void ShowQuestWindow();
	void HideQuestWindow();
	void ShowQuestProgressWindow();
	void HideQuestProgressWindow();
	void PlusQuestProgress();
	void PlusQuestWidow();
	bool CheckQuestClear();
	bool CheckQuestAllClear();

	void QuestClearPacket();

	void QuestProcess();
	//------------------------------------------------------

    void ReleaseObjects();

    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

///-------------------------------------------------------------------------------------
//서버


public:
	void Server(WPARAM wParam, LPARAM lParam);
	void InitServer();
	void ShutDownServer();
	void ReadPacket(SOCKET sock);
	void ProcessPacket(char *ptr);
	float GetAngleBetweenTwoVectors2(float dVec1X, float dVec1Y, float dVec2X, float dVec2Y);
private:
	WSABUF		send_wsabuf;
	char 			send_buffer[BUF_SIZE];
	WSABUF		recv_wsabuf;
	char				recv_buffer[BUF_SIZE];
	DWORD		in_packet_size;
	int				saved_packet_size;
	char				packet_buffer[BUF_SIZE];
private:
	WSADATA			wsa;
	int						retval;
	SOCKET				g_mysocket;
	SOCKADDR_IN		serveraddr;
private:
	int		g_myid;
	int		animation;
	bool		attack_on;
	bool		hit_on;
	bool		skill_on;
	///-----------------------------------------------------------------------
private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd; 

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;
        
	IDXGIFactory4				*m_pdxgiFactory = NULL;
	IDXGISwapChain3				*m_pdxgiSwapChain = NULL;
	ID3D12Device				*m_pd3dDevice = NULL;
	//Comptr
	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap = NULL;
	UINT						m_nRtvDescriptorIncrementSize;

	ID3D12Resource				*m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap = NULL;
	UINT						m_nDsvDescriptorIncrementSize;

	ID3D12CommandAllocator		*m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue			*m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList	*m_pd3dCommandList = NULL;

	ID3D12Fence					*m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

#if defined(_DEBUG)
	ID3D12Debug					*m_pd3dDebugController;
#endif

	CGameTimer					m_GameTimer;

	float								m_fKeyWaitingTime = 0.f;// 1.2f;
	bool									m_bAttackPossible = true;
	bool									m_bAttackCheckigStart = false;
	bool CheckKeyTime(float fElapsedTime);

	bool									m_bIsAttacking = false;


	CScene						* m_pCurrScene = NULL;
	CScene						* m_pTitleScene = NULL;
	CScene						* m_pTownScene = NULL;
	CScene						* m_pFieldScene = NULL;

	PLAYERTYPE				m_cType = SWORDER;
	CPlayer						*m_pPlayer = NULL;
	CPlayer						*m_pTitlePlayer = NULL;
	CPlayer						*m_pTownPlayer = NULL;
	CPlayer						*m_pFieldPlayer = NULL;

	CPlayer						*m_pFieldSworderPlayer = NULL;
	CPlayer						*m_pFieldAssasinPlayer = NULL;

	CPlayer						*m_pTownSworderPlayer = NULL;
	CPlayer						*m_pTownAssassinPlayer = NULL;

	CPlayer						*m_pOtherSworderPlayer[10];// = NULL;
	CPlayer						*m_pOtherAssassinPlayer[10];// = NULL;

	//
	CPlayer						**m_ppOtherPlayers;
	CGameObject					**m_ppMonsters;
	//
	CCamera						*m_pCamera = NULL;
	CCamera						**m_ppOtherPlayerCameras = NULL;

	POINT						m_ptOldCursorPos;
	POINT						m_ptCurrMousePos;
	bool							m_bMouseClick = false;

	_TCHAR						m_pszFrameRate[50];
};

