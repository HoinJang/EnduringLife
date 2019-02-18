//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dSwapChainBackBuffers[i] = NULL;
	m_nSwapChainBufferIndex = 0;

	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dCommandList = NULL;

	m_pd3dRtvDescriptorHeap = NULL;
	m_pd3dDsvDescriptorHeap = NULL;

	m_nRtvDescriptorIncrementSize = 0;
	m_nDsvDescriptorIncrementSize = 0;

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pCurrScene = NULL;
	m_pPlayer = NULL;

	in_packet_size = 0;
	saved_packet_size = 0;

	_tcscpy_s(m_pszFrameRate, _T("Enduring Life ( "));
	
	animation = 0;
	attack_on = false;
	hit_on = false;
	skill_on = false;
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();

	BuildObjects();
	CreateSound(&m_hWnd);
	return(true);
}

// #define _WITH_SWAPCHAIN

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

#ifdef _WITH_CREATE_SWAPCHAIN_FOR_HWND
	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	dxgiSwapChainDesc.Flags = 0;
#else
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1 **)&m_pdxgiSwapChain);
#else
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	dxgiSwapChainDesc.Flags = 0;
#else
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain **)&m_pdxgiSwapChain);
#endif

	if (!m_pdxgiSwapChain)
	{
		MessageBox(NULL, L"Swap Chain Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

#if defined(_DEBUG)
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif

	hResult = ::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);

	IDXGIAdapter1 *pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		//--------------------------------------------------
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice))) break;
	}

	if (!m_pd3dDevice)
	{
		hResult = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void **)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice);
	}

	if (!m_pd3dDevice)
	{
		MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_pd3dFence);
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 1;
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (pd3dAdapter) pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void **)&m_pd3dCommandList);
	hResult = m_pd3dCommandList->Close();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dRtvDescriptorHeap);
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void **)&m_ppd3dSwapChainBackBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dSwapChainBackBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);

	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
//	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, d3dDsvCPUDescriptorHandle);
}

void CGameFramework::OnResizeBackBuffers()
{
	WaitForGpuComplete();

	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	m_nSwapChainBufferIndex = 0;
#else
	//m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
	m_nSwapChainBufferIndex = 0;
#endif
	CreateRenderTargetViews();
	CreateDepthStencilView();

	m_pd3dCommandList->Close();

	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pCurrScene) m_pCurrScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos); 
			m_bMouseClick = true;
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			::ReleaseCapture();
			m_bMouseClick = false;
			break;
		case WM_MOUSEMOVE:
			::GetCursorPos(&m_ptCurrMousePos);
			::ScreenToClient(m_hWnd, &m_ptCurrMousePos);
			break;
		default:
			break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pCurrScene) m_pCurrScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
		case WM_KEYUP:
			switch (wParam)
			{
				case VK_ESCAPE:
					::PostQuitMessage(0);
					break;
				case VK_RETURN:
					break;
				case VK_F1:
				case VK_F2:
				case VK_F3:
					if(m_pCurrScene!=m_pTitleScene) //타이틀 씬은 카메라 전환 ㄴㄴ
						m_pCamera = m_pPlayer->ChangeCamera((DWORD)(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
					break;
				case VK_F9:
				{
					BOOL bFullScreenState = FALSE;
					m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
					m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

					DXGI_MODE_DESC dxgiTargetParameters;
					dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					dxgiTargetParameters.Width = m_nWndClientWidth;
					dxgiTargetParameters.Height = m_nWndClientHeight;
					dxgiTargetParameters.RefreshRate.Numerator = 60;
					dxgiTargetParameters.RefreshRate.Denominator = 1;
					dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
					dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
					m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

					OnResizeBackBuffers();

					break;
				}
				case VK_F10:
					break;
				default:
					break;
			}
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
				case 'C':
				{
					PlusQuestProgress();  //치트키
					break;
				}
				case 'L':
				{
					m_pCurrScene->FlashToggle();
					break;
				}
				case 'F':
				{
					m_bIsAttacking = true;
			
					if (m_bAttackPossible && attack_on == false && skill_on == false)
					{
						m_bAttackCheckigStart = true;
						m_fKeyWaitingTime = 0.0f;
	
						cs_packet_attack *my_packet = reinterpret_cast<cs_packet_attack*>(send_buffer);
						my_packet->size = sizeof(my_packet);
						send_wsabuf.len = sizeof(my_packet);
						DWORD iobyte;

						my_packet->type = CS_ATTACK;

						int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
						if (ret) {
							int error_code = WSAGetLastError();
							printf("Error while sending packet [%d]\n", error_code);
						}
						attack_on = true;
						hit_on = false;
					}			
					break;
				}
				case 'V':
				{
					m_bIsAttacking = true;

					if (m_bAttackPossible && attack_on == false && skill_on == false)
					{
						m_bAttackCheckigStart = true;
						m_fKeyWaitingTime = 0.0f;

						skill_on = true;
						hit_on = false;
					}
					break;
				}
			}
		default:
			m_pCurrScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
				m_GameTimer.Stop();
			else
				m_GameTimer.Start();
			break;
		}
		case WM_SIZE:
		{
			m_nWndClientWidth = LOWORD(lParam);
			m_nWndClientHeight = HIWORD(lParam);

			OnResizeBackBuffers();
			break;
		}
		case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
		case WM_SOCKET:
			Server(wParam, lParam);
	}
	return(0);
}

void CGameFramework::OnDestroy()
{
    ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

#if defined(_DEBUG)
	if (m_pd3dDebugController) m_pd3dDebugController->Release();
#endif

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
    if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();
}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

//-------------Scene과 Player 생성-------------
	m_pTitleScene = new CTitleScene();
	m_pTitleScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
	m_pTitlePlayer = new CTitlePlayer(m_pd3dDevice, m_pd3dCommandList, m_pTitleScene->GetGraphicsRootSignature(), m_pTitleScene->GetTerrain(), 1);

	m_pTownScene = new CTownScene();
	m_pTownScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
	m_pTownPlayer = new CHumanPlayer(ASSASSIN,m_pd3dDevice, m_pd3dCommandList, m_pTownScene->GetGraphicsRootSignature(), m_pTownScene->GetTerrain(), 1);
	
	m_pFieldScene = new CFieldScene();
	m_pFieldScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);
	
	//
	m_pFieldSworderPlayer = new CHumanPlayer(SWORDER, m_pd3dDevice, m_pd3dCommandList, m_pFieldScene->GetGraphicsRootSignature(), m_pFieldScene->GetTerrain(), 1);
	m_pFieldAssasinPlayer = new CHumanPlayer(ASSASSIN, m_pd3dDevice, m_pd3dCommandList, m_pFieldScene->GetGraphicsRootSignature(), m_pFieldScene->GetTerrain(), 1);
	m_pTownSworderPlayer= new CHumanPlayer(SWORDER, m_pd3dDevice, m_pd3dCommandList, m_pTownScene->GetGraphicsRootSignature(), m_pTownScene->GetTerrain(), 1);
	m_pTownAssassinPlayer = new CHumanPlayer(ASSASSIN, m_pd3dDevice, m_pd3dCommandList, m_pTownScene->GetGraphicsRootSignature(), m_pTownScene->GetTerrain(), 1);
	//
	for (int i = 0; i < 10; ++i)  m_pOtherSworderPlayer[i] = new COtherPlayer(SWORDER, m_pd3dDevice, m_pd3dCommandList, m_pTownScene->GetGraphicsRootSignature(), m_pTownScene->GetTerrain(), 1);
	for (int i = 0; i < 10; ++i)  m_pOtherAssassinPlayer[i] = new COtherPlayer(ASSASSIN, m_pd3dDevice, m_pd3dCommandList, m_pTownScene->GetGraphicsRootSignature(), m_pTownScene->GetTerrain(), 1);
	//
	m_pCurrScene = m_pTitleScene;
	m_pPlayer = new CHumanPlayer(SWORDER, m_pd3dDevice, m_pd3dCommandList, m_pCurrScene->GetGraphicsRootSignature(), m_pCurrScene->GetTerrain(), 1);
	m_pCurrScene->m_pPlayer = m_pPlayer;
	m_pCamera = m_pPlayer->GetCamera();

	//---------초기 TitleScene Player로 설정--------------

	m_pCurrScene = m_pTitleScene;
	m_pPlayer = m_pTitlePlayer;
	m_pCurrScene->m_pPlayer = m_pPlayer;

	m_pPlayer->SetPosition(XMFLOAT3(0.f, 0.f, 0.f));
	m_pCamera = m_pPlayer->GetCamera();


	//--------------------------------------------------

	m_ppOtherPlayers = new CPlayer*[MAX_USER];
	m_ppMonsters = new CGameObject*[MAX_OBJECT_INDEX - MAX_USER];
	

	for (int i = 0; i < MAX_USER; ++i)
	{
		m_ppOtherPlayers[i] = NULL;
	}

	for (int i = 0; i < MAX_OBJECT_INDEX - MAX_USER; ++i)
	{
		if (i < 30)
		{
		// 초급 : 거미
			m_ppMonsters[i] = new CAnimatedModelObject(OTHER, m_pd3dDevice, m_pd3dCommandList, m_pFieldScene->GetGraphicsRootSignature(),
				"Assets/Model/Creatures/0.Bug.data", L"Assets/Model/Creatures/0.CrawlingBug-Green.dds", 1.f); 
			m_ppMonsters[i]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
			m_ppMonsters[i]->SetScale(0.2f, 0.2f, 0.2f);
		}
		else if (i < 40)
		{
			//중급 : 게
			m_ppMonsters[i] = new CAnimatedModelObject(OTHER, m_pd3dDevice, m_pd3dCommandList, m_pFieldScene->GetGraphicsRootSignature(),
				"Assets/Model/Creatures/1.bdl_byx.data", L"Assets/Model/Creatures/1.bdl_byx01.dds", 1.f);
			m_ppMonsters[i]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
			m_ppMonsters[i]->SetScale(10.f, 10.f, 10.f);
		}
		else
		{
			//상급 : 골렘
			m_ppMonsters[i] = new CAnimatedModelObject(OTHER, m_pd3dDevice, m_pd3dCommandList, m_pFieldScene->GetGraphicsRootSignature(),
				"Assets/Model/Creatures/3.RockWarrior.data", L"Assets/Model/Creatures/3.RockWarrior.dds", 1.f);
			m_ppMonsters[i]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
			m_ppMonsters[i]->SetScale(15.f, 15.f, 15.f);
		}
			m_ppMonsters[i]->SetVisible(false);
			m_pCurrScene->UpdateMonHPIndex(i, false, m_ppMonsters[i]->GetPosition(), m_ppMonsters[i]->GetHP());

	}

	//--------------열쇠 아이템------------------
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];
	m_ppObjects[0] = new CFBXModel(m_pd3dDevice, m_pd3dCommandList, m_pFieldScene->GetGraphicsRootSignature(), "Assets/Model/Item/Key.data", L"Assets/Model/Item/Key.dds");
	float height = m_pFieldScene->GetTerrain()->GetHeight(150, 870);
	m_ppObjects[0]->SetPosition(XMFLOAT3(150, height - 10, 870));
	m_ppObjects[0]->SetScale(0.1f, 0.1f, 0.1f);
	//-------------------------------------------

	BuildShaders();
	SetLevelDesign(1);

	m_pd3dCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);


	WaitForGpuComplete();

	if (m_pPlayer) 
		m_pPlayer->ReleaseUploadBuffers();

	for (int i = 0; i < MAX_USER; ++i) 
	{
		if (m_ppOtherPlayers[i]!=NULL)
			m_ppOtherPlayers[i]->ReleaseUploadBuffers();
	}
	for (int i = 0; i < MAX_OBJECT_INDEX - MAX_USER; ++i)
	{
		if (m_ppMonsters)
			m_ppMonsters[i]->ReleaseUploadBuffers();
	}

	for (int i = 0; i < m_nShaders; i++) 
		m_ppShaders[i]->ReleaseUploadBuffers();


	if (m_pCurrScene) 
		m_pCurrScene->ReleaseUploadBuffers();
	m_GameTimer.Reset();
}

void CGameFramework::BuildShaders()
{
	m_nShaders = 19;
	m_ppShaders = new CShader*[m_nShaders];

	//MaxBarShader
	CUIShader*pMaxBarShader = new CUIShader();
	pMaxBarShader->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Image/UI/MaxBar.dds", 0);
	pMaxBarShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 0, 0, 0.5f, 0.03f, pTexture);
	m_ppShaders[MAXBAR] = pMaxBarShader;
	dynamic_cast<CUIShader*> (m_ppShaders[MAXBAR])->SetUIpos(0, -0.8f, 0.1f);  

	//HPBar
	CHPBarShader *pHPBarShader = new CHPBarShader();
	pHPBarShader->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Image/UI/HPBar.dds", 0);
	pHPBarShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 0.f, 0.f, 0.5f, 0.03f, pTexture);
	m_ppShaders[HP] = pHPBarShader;

	//MPBar
	CMPBarShader *pMPBarShader = new CMPBarShader();
	pMPBarShader->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Image/UI/MPBar.dds", 0);
	pMPBarShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 0.f, 0.f, 0.5f, 0.03f, pTexture);
	m_ppShaders[MP] = pMPBarShader;

	//EXPBar
	CEXPBarShader *pEXPBarShader = new CEXPBarShader();
	pEXPBarShader->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Image/UI/EXPBar.dds", 0);
	pEXPBarShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 0.f, 0.f, 2.f, 0.02f, pTexture);
	m_ppShaders[EXP] = pEXPBarShader;

	//Lv Shader
	CUIShader*pLvShader = new CUIShader();
	pLvShader->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Image/UI/lvnum.dds", 0);
	pLvShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 0, 0, 0.05f, 0.05f, pTexture);
	m_ppShaders[LEVEL] = pLvShader;
	dynamic_cast<CUIShader*> (m_ppShaders[LEVEL])->SetUIpos(-0.75f, -0.95f, 0.f); //레벨
	dynamic_cast<CUIShader*> (m_ppShaders[LEVEL])->SetMaxIdx(3);

	//Quest
	CUIShader*pQuestShader = new CUIShader();
	pQuestShader->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Image/UI/quest_list.dds", 0);
	pQuestShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 0, 0, 0.6f, 0.3f, pTexture);
	m_ppShaders[QUEST] = pQuestShader;
	dynamic_cast<CUIShader*> (m_ppShaders[QUEST])->SetUIpos(0, -0.2f, 0.f);
	dynamic_cast<CUIShader*> (m_ppShaders[QUEST])->SetMaxIdx(6);
	dynamic_cast<CUIShader*> (m_ppShaders[QUEST])->PlusUIIdx();

	//Quest Progress Shader
	CUIShader*pQuestProgressShader = new CUIShader();
	pQuestProgressShader->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Image/UI/quest_progress_list.dds", 0);
	pQuestProgressShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 0, 0, 0.4f, 0.2f, pTexture);
	m_ppShaders[QUSET_PROGRESS] = pQuestProgressShader;
	dynamic_cast<CUIShader*> (m_ppShaders[QUSET_PROGRESS])->SetUIpos(-0.8f, 0.9f, 0.1f); // 퀘스트 표시
	dynamic_cast<CUIShader*> (m_ppShaders[QUSET_PROGRESS])->PlusUIIdx();		 // 0번 인덱스					
	dynamic_cast<CUIShader*> (m_ppShaders[QUSET_PROGRESS])->SetMaxIdx(3);

	//Quest Process Numbering Shader
	CUIShader*pQuestNumShader = new CUIShader();
	pQuestNumShader->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, L"Assets/Image/UI/num.dds", 0);
	pQuestNumShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 0, 0, 0.065f, 0.1f, pTexture);
	m_ppShaders[NUM] = pQuestNumShader;

	dynamic_cast<CUIShader*> (m_ppShaders[NUM])->SetUIpos(-0.8f, 0.85f, 0.f); // 퀘스트 진척도 표시
	dynamic_cast<CUIShader*> (m_ppShaders[NUM])->PlusUIIdx();		 // 0번 인덱스					
	dynamic_cast<CUIShader*> (m_ppShaders[NUM])->SetMaxIdx(11);	

	CEffectShader *pEffectShader = new CEffectShader();
	pEffectShader->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pEffectShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 20, 20);
	pEffectShader->SetType(SworderSkill);
	m_ppShaders[SKILL] = pEffectShader;

	CEffectShader *pEffectShader1 = new CEffectShader();
	pEffectShader1->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pEffectShader1->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 20, 20);
	m_ppShaders[SKILL+1] = pEffectShader1;

	CEffectShader *pEffectShader2 = new CEffectShader();
	pEffectShader2->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pEffectShader2->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 20, 20);
	m_ppShaders[SKILL+2] = pEffectShader2;

	CEffectShader *pEffectShader3 = new CEffectShader();
	pEffectShader3->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pEffectShader3->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 20, 20);
	m_ppShaders[SKILL+3] = pEffectShader3;

	CEffectShader *pEffectShader4 = new CEffectShader();
	pEffectShader4->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pEffectShader4->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 20, 20);
	m_ppShaders[SKILL+4] = pEffectShader4;

	CEffectShader *pEffectShader5 = new CEffectShader();
	pEffectShader5->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pEffectShader5->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 20, 20);
	m_ppShaders[SKILL+5] = pEffectShader5;

	CEffectShader *pEffectShader6 = new CEffectShader();
	pEffectShader6->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pEffectShader6->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 20, 20);
	m_ppShaders[SKILL+6] = pEffectShader6;

	CEffectShader *pEffectShader7 = new CEffectShader();
	pEffectShader7->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pEffectShader7->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 20, 20);
	m_ppShaders[SKILL+7] = pEffectShader7;

	CEffectShader *pEffectShader8 = new CEffectShader();
	pEffectShader8->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pEffectShader8->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 20, 20);
	m_ppShaders[SKILL+8] = pEffectShader8;

	CEffectShader *pEffectShader9 = new CEffectShader();
	pEffectShader9->CreateShader(m_pd3dDevice, m_pTownScene->GetGraphicsRootSignature());
	pEffectShader9->BuildObjects(m_pd3dDevice, m_pd3dCommandList, 20, 20);
	m_ppShaders[SKILL+9] = pEffectShader9;

	CWaterShader *pWaterObjectShader = new CWaterShader();
	pWaterObjectShader->CreateShader(m_pd3dDevice, m_pCurrScene->GetGraphicsRootSignature());
	pWaterObjectShader->BuildObjects(m_pd3dDevice, m_pd3dCommandList, NULL);
	m_ppShaders[WATER] = pWaterObjectShader;
}

void CGameFramework::CreateSound(HWND * hwnd)
{
	CSound* background = new CSound();
	//cout<<
	background->Initialize(hwnd, "Assets/Sound/처음배경음.wav", 44100, 16, 2);
	//<< endl;
	m_vSounds.push_back(background);

	CSound* HitSound2 = new CSound();
	//cout<<
	HitSound2->Initialize(hwnd, "Assets/Sound/총쏘는거.wav", 44100, 16, 2);
	//<< endl;
	m_vSounds.push_back(HitSound2);

	CSound* AttackSound1 = new CSound();
	//cout << 
	AttackSound1->Initialize(hwnd, "Assets/Sound/활표창.wav", 44100, 16, 2);
	//<< endl;
	m_vSounds.push_back(AttackSound1);

	CSound* AttackSound2 = new CSound();
	//cout << 
	AttackSound2->Initialize(hwnd, "Assets/Sound/하급몬스터맞는소리.wav", 44100, 16, 2);
	//<< endl;
	m_vSounds.push_back(AttackSound2);

	CSound* AttackSound4 = new CSound();
	//cout << 
	AttackSound4->Initialize(hwnd, "Assets/Sound/중급몬스터맞는소리.wav", 44100, 16, 2);
	//<< endl;
	m_vSounds.push_back(AttackSound4);

}

void CGameFramework::SoundManager(int id, BYTE effect)
{
	bool isPlay = false;

	//플레이어일 때
	if (id == g_myid)
		isPlay = true;
	
	//아더플레이어일 때
	else if (id < MAX_USER)
		//같은 씬에 있을 때만 소리가 들리도록
		if (m_ppOtherPlayers[id]->GetScene() == m_pPlayer->GetScene())
			isPlay = true;
	
	//TRUE면 사운드ON
	if(isPlay)
	{
		switch (effect)
		{
		case ASSASIN_DEFAULT:
			m_vSounds[2]->PlayWaveFile(0);
			break;
		case ASSASIN_SKILL:
			m_vSounds[2]->PlayWaveFile(0);
			break;
		case SWORD_DEFAULT:
			m_vSounds[2]->PlayWaveFile(0);
			break;
		case SWORD_SKILL:
			m_vSounds[2]->PlayWaveFile(0);
			break;
		}
	}
}

void CGameFramework::SetLevelDesign(int lv)
{
	dynamic_cast<CUIShader*> (m_ppShaders[LEVEL])->SetIdx(lv-1);

	switch (lv)
	{
	case 1:
		m_Level = 1;
		m_MaxHP = 200;
		break;
	case 2:
		m_Level = 2;
		m_MaxHP = 1000;
		break;
	case 3:
		m_Level = 3;
		m_MaxHP = 3000;
		break;
	default:
		break;
	}
}

void CGameFramework::SetEffectPosition(int idx, XMFLOAT3 pos)
{
	dynamic_cast<CEffectShader*>(m_ppShaders[idx + SKILL])->SetEffectPosition(pos); 
}

bool CGameFramework::CheckItemCollision(XMFLOAT3 ItemPos)
{
	auto pos = m_pPlayer->GetPosition();

	if (ItemPos.x - 15.f <= pos.x && pos.x <= ItemPos.x + 15.f &&
		ItemPos.z - 15.f <= pos.z && pos.z <= ItemPos.x + 15.f)	 return true;
	else	return false;
	
	return false;
}

bool CGameFramework::CheckNPCCollision()
{
	auto pos = m_pPlayer->GetPosition();
	auto m_QuestNum = (m_CurrQuestIdx + 2) / 2;

	// 퀘스트1 : 300,400
	if (m_QuestNum== SPIDER)
	{
		if (285 <= pos.x && pos.x <= 325 &&
			385 <= pos.z && pos.z <= 425)	return true;
		else	return false;
	}

	// 퀘스트2 : 750,400
	if (m_QuestNum == CRAB)
	{
		if (725 <= pos.x && pos.x <= 775 &&
			375 <= pos.z && pos.z <= 425)	return true;
		else	return false;
	}
	// 퀘스트3 : 450, 720
	if (m_QuestNum == GOLEM)
	{
		if (425 <= pos.x && pos.x <= 475 &&
			700 <= pos.z && pos.z <= 745)	return true;
		else	return false;
	}
	return false;
}

bool CGameFramework::CheckMouseOver()
{
	if (650 <= m_ptCurrMousePos.x&&m_ptCurrMousePos.x <= 790 &&
		555 <= m_ptCurrMousePos.y&&m_ptCurrMousePos.y <= 590) 	return true;
	else	return false;
}

void CGameFramework::ShowQuestWindow()
{
	m_ppShaders[QUEST]->SetVisible(true);
}

void CGameFramework::HideQuestWindow()
{
	m_ppShaders[QUEST]->SetVisible(false);
}

void CGameFramework::ShowQuestProgressWindow()
{
	m_ppShaders[QUSET_PROGRESS]->SetVisible(true); //창
	m_ppShaders[NUM]->SetVisible(true); //숫자
}

void CGameFramework::HideQuestProgressWindow()
{
	m_ppShaders[QUSET_PROGRESS]->SetVisible(false); //창
	m_ppShaders[NUM]->SetVisible(false); //숫자
}

void CGameFramework::PlusQuestProgress()
{
	auto Q = (m_CurrQuestIdx + 1) / 2;      //퀘스트 번호
	//cout << "Q"<<Q << endl;
	if (Q==GOLEM)
	{
		if (m_QuestProgressDegree < 1)		m_QuestProgressDegree++; //넘지 못하도록 조치
	}
	else
	{
		if (m_QuestProgressDegree < 10)	m_QuestProgressDegree++; /*넘지 못하도록 조치*/
	}

	dynamic_cast<CUIShader*>(m_ppShaders[NUM])->SetIdx(m_QuestProgressDegree);
}

void CGameFramework::PlusQuestWidow()
{
	dynamic_cast<CUIShader*>(m_ppShaders[QUSET_PROGRESS])->SetIdx(m_CurrQuestIdx / 2);
}

bool CGameFramework::CheckQuestClear()
{
	auto Q = (m_CurrQuestIdx + 1) / 2;      //퀘스트 번호

	switch (Q)
	{
	case SPIDER:
		if (m_QuestProgressDegree == 10) {
			m_QuestProgressDegree = 0; 
			m_nQuestClear++;
			QuestClearPacket();
			return true; 
		}
		else return false;
		break;
	case CRAB:
		if (m_QuestProgressDegree == 10) {
			 m_QuestProgressDegree = 0; 
			 m_nQuestClear++; 
			 QuestClearPacket();
			 return true;
		}
		else return false;
		break;
	case GOLEM:
		if (m_QuestProgressDegree == 1) {
			 m_QuestProgressDegree = 0; 
			// m_nQuestClear++;
			 //QuestClearPacket();
			 return true;
		}
		else return false;
		break;
	default:
		return false;
		break;
	}
}

bool CGameFramework::CheckQuestAllClear()
{
	if (m_CurrQuestIdx == 6) return true;
	return false;
}

void CGameFramework::QuestClearPacket()
{
	cs_packet_quest_ok *my_packet = reinterpret_cast<cs_packet_quest_ok*>(send_buffer);
	my_packet->size = sizeof(my_packet);
	my_packet->type = CS_QUEST_OK;
	send_wsabuf.len = sizeof(my_packet);
	DWORD iobyte;

	my_packet->quest_num = m_nQuestClear;

	int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
	if (ret) {
		int error_code = WSAGetLastError();
		printf("Error while sending packet [%d]", error_code);
	}
}

void CGameFramework::QuestProcess()
{				
	if(m_pCurrScene==m_pTownScene) //타운씬일 때만
	if (CheckNPCCollision()) 
	{
		if (!m_bQuestING) //퀘스트가 진행중이지 않을 때
		{
			ShowQuestWindow(); //퀘스트 창을 띄움

			if (CheckMouseOver()) //마우스가 닿으면
			{
				dynamic_cast<CUIShader*> (m_ppShaders[QUEST])->SetIdx(m_CurrQuestIdx + 1);//버튼 색을 바꿈
				if (m_bMouseClick)  //수락버튼을 클릭하면
				{
					m_bQuestING = true; //퀘스트 진행 중으로 바꾸고
					m_CurrQuestIdx++; //퀘스트 번호++
					m_QuestProgressDegree = 0;
					ShowQuestProgressWindow(); //퀘스트 진척창을 띄움
					HideQuestWindow(); //퀘스트 창을 감춤
				}
			}
			else
				dynamic_cast<CUIShader*> (m_ppShaders[QUEST])->SetIdx(m_CurrQuestIdx);//마우스가 닿지 않으면 버튼 색은 회색
		}
		else
		{
			HideQuestWindow(); //퀘스트가 진행 중이면 반응 없음
			
			if (CheckQuestClear()	)  //퀘스트 목표를 달성했다면
			{
				m_bQuestING = false;
				m_CurrQuestIdx++; 
				dynamic_cast<CUIShader*>(m_ppShaders[NUM])->SetIdx(0);
				
				HideQuestProgressWindow();

				if (CheckQuestAllClear())
				{
					HideQuestWindow();
					m_bQuestING = true; //퀘스트 더 받을 수 없도록 조치
				}
				else
					PlusQuestWidow();
			}
		}
	}
	else
		HideQuestWindow(); //충돌하지 않아도 반응 없음
}

void CGameFramework::ReleaseObjects()
{
	if (m_pPlayer) m_pPlayer->ReleaseBuffer();
	if (m_pPlayer) delete m_pPlayer;

	for (int i = 0; i < MAX_USER; ++i) {
		if (m_ppOtherPlayers[i]!=NULL) m_ppOtherPlayers[i]->ReleaseBuffer();
			delete m_ppOtherPlayers[i];
	}
	for (int i = 0; i < MAX_OBJECT_INDEX - MAX_USER; ++i)
	{
		if (m_ppMonsters)
			delete m_ppMonsters[i];
	}
	if (m_ppObjects)
	{
		for (int i = 0; i < m_nObjects; i++) delete m_ppObjects[i];
		delete[] m_ppObjects;
	}
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pCurrScene) m_pCurrScene->ReleaseObjects();
	if (m_pCurrScene) delete m_pCurrScene;
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pCurrScene) bProcessedByScene = m_pCurrScene->ProcessInput(pKeysBuffer);
	
	if (!bProcessedByScene)
	{
		DWORD dwDirection = 0;

		int x = 0, y = 0;
		bool teleport = false;




		if (attack_on == false && skill_on == false)
			animation = 0;

		if (hit_on)
			animation = 3;

		// 0 - 가만히, 1 -move , 2-attack , 3-hit (맞았을 때) , 4-die , 5-skill

		if (attack_on == false && skill_on == false)
		{
			XMFLOAT3 player_position = m_pPlayer->GetPosition();
			XMFLOAT3 player_lookvector = m_pPlayer->GetLookVector();
			if (pKeysBuffer['D'] & 0xF0)
			{
				hit_on = false;

				x += 1;
				animation = 1;
				player_position.x += player_lookvector.x;
				player_position.z -= player_lookvector.z;
			}
			if (pKeysBuffer['A'] & 0xF0)
			{
				hit_on = false;

				x -= 1;
				animation = 1;
				player_position.x -= player_lookvector.x;
				player_position.z += player_lookvector.z;

			}
			if (pKeysBuffer['W'] & 0xF0)
			{
				hit_on = false;

				y -= 1;
				animation = 1;


				player_position.x += player_lookvector.x;
				player_position.z += player_lookvector.z;
			}

			if (pKeysBuffer['S'] & 0xF0)
			{
				hit_on = false;

				y += 1;
				animation = 1;
				player_position.x -= player_lookvector.x;
				player_position.z -= player_lookvector.z;
			}

			m_pPlayer->SetPosition(player_position);

			if (m_pCurrScene == m_pTownScene)
			{
				auto pos = player_position;

				if ((455 <= pos.x) && (pos.x <= 565) && (100 <= pos.z) && (pos.z <= 335))
				{
					m_pPlayer->SetBridge(true);
				}
				else
					m_pPlayer->SetBridge(false);
			}

			if (pKeysBuffer[VK_SPACE] & 0xF0)
				teleport = true;

			cs_packet_move *my_packet = reinterpret_cast<cs_packet_move*>(send_buffer);
			my_packet->size = sizeof(my_packet);
			send_wsabuf.len = sizeof(my_packet);

			DWORD iobyte;
			float tempx, tempz;
			tempx = (roundf(m_pPlayer->GetLookVector().x * 100) / 100) * 100;
			tempz = (roundf(m_pPlayer->GetLookVector().z * 100) / 100) * 100;

			my_packet->dir_x = tempx;
			my_packet->dir_z = tempz;

			if (0 != x) {
				if (1 == x) my_packet->type = CS_RIGHT;
				else my_packet->type = CS_LEFT;


				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			if (0 != y) {
				if (1 == y) my_packet->type = CS_DOWN;
				else my_packet->type = CS_UP;

				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}

			if (teleport) {
				my_packet->type = CS_TELEPORT;

				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
		}

		if (pKeysBuffer['1'] & 0xF0) {
			InitServer();
			m_pCurrScene = m_pTownScene;
			m_pPlayer = m_pTownPlayer;
			m_pCurrScene->m_pPlayer = m_pPlayer;
			m_pPlayer->SetPosition(XMFLOAT3(512.0f, m_pCurrScene->GetTerrain()->GetHeight(512.f, 512.f), 512.0f));
			for (int i = 0; i < MAX_USER; ++i)
				if (m_ppOtherPlayers[i] != NULL) m_ppOtherPlayers[i]->SetPlayerUpdatedContext(m_pCurrScene->GetTerrain());
			for (int i = 0; i < QUEST; ++i)
				m_ppShaders[i]->SetVisible(true);
			m_pCamera = m_pPlayer->GetCamera();
		}


		if (animation == 0 && attack_on == false && skill_on == false)
		{
			if (m_pPlayer->GetFBXAnimationNum() != 0)
			{

				cs_packet_animate *packet = reinterpret_cast<cs_packet_animate*>(send_buffer);

				packet->size = sizeof(packet);
				send_wsabuf.len = sizeof(packet);
				DWORD iobyte;

				packet->type = CS_ANIMATE;
				packet->animate_num = IDLE;

				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
				m_pPlayer->SetAnimation(IDLE);
				//m_pPlayer->SetIndieCamera(false);

				m_pCurrScene->EndSkillLight();
			}
		}
		else if (animation == 1 && attack_on == false && skill_on == false)
		{
			if (m_pPlayer->GetFBXAnimationNum() != 1)
			{
				cs_packet_animate *packet = reinterpret_cast<cs_packet_animate*>(send_buffer);

				packet->size = sizeof(packet);
				send_wsabuf.len = sizeof(packet);
				DWORD iobyte;

				packet->type = CS_ANIMATE;
				packet->animate_num = WALK;

				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}

				m_pPlayer->SetAnimation(1);
				//m_pPlayer->SetIndieCamera(false);
			}
		}
		else if (animation == 3 && attack_on == false && skill_on == false)
		{
			if (m_pPlayer->GetFBXAnimationNum() != HIT)
			{
				m_pPlayer->SetAnimation(HIT);
				//m_pPlayer->SetIndieCamera(false);
			}
			else
			{
				if (m_pPlayer->IsAnimationFinish())
				{
					cs_packet_animate *packet = reinterpret_cast<cs_packet_animate*>(send_buffer);

					packet->size = sizeof(packet);
					send_wsabuf.len = sizeof(packet);
					DWORD iobyte;

					packet->type = CS_ANIMATE;
					packet->animate_num = IDLE;

					int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
					if (ret) {
						int error_code = WSAGetLastError();
						printf("Error while sending packet [%d]", error_code);
					}

					m_pPlayer->SetAnimation(IDLE);
					//m_pPlayer->SetIndieCamera(false);
					hit_on = false;

					m_pCurrScene->EndSkillLight();
				}

			}
		}

		if (attack_on)
		{
			bool end = false;

			if (m_pPlayer->IsAnimationFinish())
			{
				cs_packet_animate *packet = reinterpret_cast<cs_packet_animate*>(send_buffer);

				packet->size = sizeof(packet);
				send_wsabuf.len = sizeof(packet);
				DWORD iobyte;

				packet->type = CS_ANIMATE;
				packet->animate_num = IDLE;

				m_pCurrScene->EndSkillLight();

				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}

				else
					if (GetEffectStart(g_myid))    //이미 시작한 상황
						SetEffectStart(g_myid, false);

				m_pPlayer->SetAnimation(IDLE);

				attack_on = false;
				end = true;

				if (m_cType == SWORDER)
				{
					cs_packet_move *my_packet = reinterpret_cast<cs_packet_move*>(send_buffer);
					my_packet->size = sizeof(my_packet);
					send_wsabuf.len = sizeof(my_packet);

					DWORD iobyte;
					float tempx, tempz;
					tempx = (roundf(m_pPlayer->GetLookVector().x * 100) / 100) * 100;
					tempz = (roundf(m_pPlayer->GetLookVector().z * 100) / 100) * 100;

					my_packet->dir_x = tempx;
					my_packet->dir_z = tempz;
					my_packet->type = CS_TELEPORT;

					int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
					if (ret) {
						int error_code = WSAGetLastError();
						printf("Error while sending packet [%d]", error_code);
					}

					XMFLOAT3 temp_position;

					temp_position.x = m_pPlayer->GetPosition().x;
					temp_position.z = m_pPlayer->GetPosition().z;

					temp_position.x = temp_position.x + m_pPlayer->GetLookVector().x * 12;
					temp_position.z = temp_position.z + m_pPlayer->GetLookVector().z * 12;

					temp_position.y = m_pCurrScene->GetTerrain()->GetHeight(temp_position.x, temp_position.z);

					m_pPlayer->SetPosition(temp_position);

				}
			}

			if (m_pPlayer->GetFBXAnimationNum() != 2 && end == false)
			{
				cs_packet_animate *packet = reinterpret_cast<cs_packet_animate*>(send_buffer);

				packet->size = sizeof(packet);
				send_wsabuf.len = sizeof(packet);
				DWORD iobyte;
				packet->type = CS_ANIMATE;
				packet->animate_num = ATTACK;

				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
				m_pPlayer->SetAnimation(ATTACK);
				if (m_cType == SWORDER)			m_pPlayer->SetIndieCamera(true, 38.f);
			}
		}



		if (skill_on)
		{
			bool end = false;

			if (m_pPlayer->IsAnimationFinish())
			{
				m_pPlayer->SetIndieCamera(false,0.f);

				if (m_cType == SWORDER)
				{
					cs_packet_move *my_packet = reinterpret_cast<cs_packet_move*>(send_buffer);
					my_packet->size = sizeof(my_packet);
					send_wsabuf.len = sizeof(my_packet);

					DWORD iobyte;
					float tempx, tempz;
					tempx = (roundf(m_pPlayer->GetLookVector().x * 100) / 100) * 100;
					tempz = (roundf(m_pPlayer->GetLookVector().z * 100) / 100) * 100;

					my_packet->dir_x = tempx;
					my_packet->dir_z = tempz;
					my_packet->type = CS_SKILL_MOVE;

					int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
					if (ret) {
						int error_code = WSAGetLastError();
						printf("Error while sending packet [%d]", error_code);
					}

					XMFLOAT3 temp_position;

					temp_position.x = m_pPlayer->GetPosition().x;
					temp_position.z = m_pPlayer->GetPosition().z;

					temp_position.x = temp_position.x + m_pPlayer->GetLookVector().x * 18;
					temp_position.z = temp_position.z + m_pPlayer->GetLookVector().z * 18;

					temp_position.y = m_pCurrScene->GetTerrain()->GetHeight(temp_position.x, temp_position.z);

					m_pPlayer->SetPosition(temp_position);

				}

				cs_packet_attack *my_packet = reinterpret_cast<cs_packet_attack*>(send_buffer);
				my_packet->size = sizeof(my_packet);
				send_wsabuf.len = sizeof(my_packet);
				DWORD iobyte1;

				my_packet->type = CS_SKILL;

				int ret1 = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte1, 0, NULL, NULL);
				if (ret1) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]\n", error_code);
				}

				cs_packet_animate *packet = reinterpret_cast<cs_packet_animate*>(send_buffer);

				packet->size = sizeof(packet);
				send_wsabuf.len = sizeof(packet);
				DWORD iobyte2;

				packet->type = CS_ANIMATE;
				packet->animate_num = IDLE;

				m_pCurrScene->EndSkillLight();

				int ret2 = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte2, 0, NULL, NULL);
				if (ret2) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}

				m_pPlayer->SetAnimation(IDLE);
				m_pPlayer->SetIndieCamera(false, 0.f);
				skill_on = false;
				end = true;
			}

			if (m_pPlayer->GetFBXAnimationNum() != 5 && end == false)
			{
				cs_packet_animate *packet = reinterpret_cast<cs_packet_animate*>(send_buffer);

				packet->size = sizeof(packet);
				send_wsabuf.len = sizeof(packet);
				DWORD iobyte;
				packet->type = CS_ANIMATE;
				packet->animate_num = 5;

				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
				m_pPlayer->SetAnimation(5);
				if (m_cType == SWORDER)	m_pPlayer->SetIndieCamera(true, 48.f);
			}
		}


		if (attack_on == false && skill_on == false)
		{
			float cxDelta = 0.0f, cyDelta = 0.0f;
			POINT ptCursorPos;
			if (GetCapture() == m_hWnd)
			{
				SetCursor(NULL);
				GetCursorPos(&ptCursorPos);
				cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
				cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
				SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
			}

			if (m_pCurrScene != m_pTitleScene)
				if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
				{
					if (cxDelta || cyDelta)
					{
						if (pKeysBuffer[VK_RBUTTON] & 0xF0)
							m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
						else
							m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
					}
					if (dwDirection) m_pPlayer->Move(dwDirection, 50.0f * m_GameTimer.GetTimeElapsed(), true);

					cs_packet_sight  *my_packet = reinterpret_cast<cs_packet_sight*>(send_buffer);
					my_packet->size = sizeof(my_packet);
					send_wsabuf.len = sizeof(my_packet);
					DWORD iobyte;
					float tempx, tempz;
					tempx = (roundf(m_pPlayer->GetLookVector().x * 100) / 100) * 100;
					tempz = (roundf(m_pPlayer->GetLookVector().z * 100) / 100) * 100;

					my_packet->type = CS_SIGHT;
					my_packet->dir_x = tempx;
					my_packet->dir_z = tempz;

					int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
					if (ret) {
						int error_code = WSAGetLastError();
						printf("Error while sending packet [%d]", error_code);
					}
				}
		}
	}

	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_ppOtherPlayers[i] != NULL)
			if (m_ppOtherPlayers[i]->GetVisible() && (g_myid != i))
			{
				auto pos = m_ppOtherPlayers[i]->GetPosition();
				//다리 위에서
				if ((455 <= pos.x) && (pos.x <= 565) && (100 <= pos.z) && (pos.z <= 335))
					m_ppOtherPlayers[i]->SetBridge(true);
				
				else
					m_ppOtherPlayers[i]->SetBridge(false);


				m_ppOtherPlayers[i]->Update(m_GameTimer.GetTimeElapsed());
			}
	}
}

void CGameFramework::AnimateObjects()
{
	//----------BGM--------------
	if (m_pCurrScene == m_pTitleScene);
		//m_vSounds[0]->PlayWaveFile(0);
	//----------------------------

	float fTimeElapsed = m_GameTimer.GetTimeElapsed();

	if (m_bAttackCheckigStart)
	{
		m_bAttackPossible = CheckKeyTime(fTimeElapsed);
		if(m_bAttackPossible) m_bAttackCheckigStart = false;
	}

	if (m_pPlayer) 
		m_pPlayer->Animate(fTimeElapsed);

	//-------------------플레이어 애니메이션----------------------------
	m_pPlayer->FrameAdvance(fTimeElapsed);
	m_pPlayer->UpdateBoneTransform(m_pd3dDevice);
	//---------------------------------------------------------------


	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_ppOtherPlayers[i] != NULL)
		if (m_ppOtherPlayers[i]->GetVisible() && (g_myid != i))
		{
			m_ppOtherPlayers[i]->Animate(fTimeElapsed);
			m_ppOtherPlayers[i]->FrameAdvance(fTimeElapsed);
			m_ppOtherPlayers[i]->UpdateBoneTransform(m_pd3dDevice);
		}
	}
	for (int i = 0; i < MAX_OBJECT_INDEX - MAX_USER; ++i)
	{			

		if (m_ppMonsters[i]->GetVisible())
		{
			m_pCurrScene->UpdateMonHPIndex(i, true, m_ppMonsters[i]->GetPosition(), m_ppMonsters[i]->GetHP());  
			m_ppMonsters[i]->Animate(fTimeElapsed);
			m_ppMonsters[i]->FrameAdvance(fTimeElapsed);
			m_ppMonsters[i]->UpdateBoneTransform(m_pd3dDevice);
		}
	}

	//퀘스트 처리
	QuestProcess();

	if (m_bItem) //충돌처리
	{
		if (CheckItemCollision(m_ppObjects[0]->GetPosition()))
		{
			PlusQuestProgress();
			m_bItem = false;
		}
	}

	for (int i = 0; i < m_nShaders; i++)	m_ppShaders[i]->AnimateObjects(fTimeElapsed, m_pCamera);

	if (m_pPlayer && m_pPlayer != m_pTitlePlayer) m_pPlayer->Animate(fTimeElapsed);

	if (m_pCurrScene) m_pCurrScene->AnimateObjects(fTimeElapsed, m_pCamera, m_pd3dDevice);
}

void CGameFramework::WaitForGpuComplete()
{
	const UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	//m_nSwapChainBufferIndex = (m_nSwapChainBufferIndex + 1) % m_nSwapChainBuffers;

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}


void CGameFramework::FrameAdvance()
{    
	m_GameTimer.Tick(30.0f);

	ProcessInput();

	
    AnimateObjects();

	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);

	m_pCurrScene->Render(m_pd3dCommandList, m_pCamera);

#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	//--------------플레이어 Render--------------------------
	if (m_pPlayer != NULL)
	{
		m_pPlayer->UpdateTransform(NULL);
		if(m_pPlayer!=m_pTitlePlayer) m_pPlayer->UpdateBuffer(m_pd3dCommandList);
		m_pPlayer->Render(m_pd3dCommandList, m_pCamera);
	}

	//-------------other플레이어 Render----------------------
	if(m_pCurrScene!=m_pTitleScene)
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_ppOtherPlayers[i] != NULL)
		{
			if (m_ppOtherPlayers[i]->GetVisible() && (g_myid != i))
			{			
				m_ppOtherPlayers[i]->UpdateBuffer(m_pd3dCommandList);
				m_ppOtherPlayers[i]->UpdateTransform(NULL);
				m_ppOtherPlayers[i]->Render(m_pd3dCommandList, m_pCamera);
			}
		}
	}	
	
	//----------------몬스터 Render---------------------------
	if (m_pCurrScene == m_pFieldScene)
	for (int i = 0; i < MAX_OBJECT_INDEX - MAX_USER; ++i)
	{
		auto visible = m_ppMonsters[i]->GetVisible();

		if (visible)
		{	
			m_ppMonsters[i]->UpdateBuffer(m_pd3dCommandList);
			m_ppMonsters[i]->UpdateTransform(NULL);
			m_ppMonsters[i]->Render(m_pd3dCommandList, m_pCamera);
		}
	}
	//----------------UI Render---------------------------
		dynamic_cast<CHPBarShader*>(m_ppShaders[HP])->SetHP(m_currHp/m_MaxHP);
		dynamic_cast<CMPBarShader*>(m_ppShaders[MP])->SetMP(m_currMp/m_MaxMP); 
		dynamic_cast<CEXPBarShader*>(m_ppShaders[EXP])->SetEXP(m_currExp/m_MaxEXP);

	////////////////////////////////////////////////////////////////////////////////////
	

	
	//---------------쉐이더 Render------------------	
	///알파 블렌딩 렌더 순서 주의
	if (m_pCurrScene == m_pTownScene)
		m_ppShaders[WATER]->Render(m_pd3dCommandList); //water shader

	m_ppShaders[MAXBAR]->Render(m_pd3dCommandList);

	for (int i = HP; i < m_nShaders - 1; ++i)
		m_ppShaders[i]->Render(m_pd3dCommandList); 

	//---------------아이템 Render------------------	
	if (m_bItem)
	{
		m_ppObjects[0]->UpdateTransform(NULL);
		m_ppObjects[0]->Render(m_pd3dCommandList);
	}


	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	hResult = m_pd3dCommandList->Close();
	
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
#else
#ifdef _WITH_SYNCH_SWAPCHAIN
	m_pdxgiSwapChain->Present(1, 0);
#else
	m_pdxgiSwapChain->Present(0, 0);
#endif
#endif

//	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}


void CGameFramework::InitServer()
{
	WSAStartup(MAKEWORD(2, 2), &wsa);

	g_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	char ip[128] = { 0 };
	char id[20] = { 0 };

	cout << " 서버 IP 입력 : ";
	cin >> ip;

	cout << " ID 입력 : ";
	cin >> id;

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr(ip);

	int Result = WSAConnect(g_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;

	int retval = send(g_mysocket, id, strlen(id), 0);

	char buf[20];

	retval = recv(g_mysocket, buf, 10, 0);
	buf[retval] = '\0';

	if (strcmp(buf, "False") == 0) {

		cout << "이미 접속중인 ID이거나, 최대 접속인원을 초과하셨습니다. " << endl;
		cout << "접속을 종료합니다." << endl;
		exit(-1);
	}
	else if (strcmp(buf, "Okay") == 0) {
		cout << "입력하신 ID로 접속 합니다. " << endl;
	}
	else if (strcmp(buf, "Newid") == 0) {
		cout << "EnduringLife의 새식구가 되신것을 환영합니다." << endl;
		cout << "1. 검사,  2. 마법사" << endl;
		cout << "캐릭터를 선택해주세요 : ";

		int character = 0;

		cin >> character;
		
		char is_character[20];

		if (character == 1)
		{
			char gender[20] = "Male";
			strcpy(is_character, gender);

			//여기서 캐릭터 빌드?
		}
		else if (character == 2)
		{
			char gender[20] = "FeMale";
			strcpy(is_character, gender);
			//여기서 캐릭터 빌드?
		}
		int retval = send(g_mysocket, is_character, strlen(is_character), 0);
	}

	WSAAsyncSelect(g_mysocket, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ);
}

void CGameFramework::ShutDownServer()
{
	WSACleanup();
}

void CGameFramework::ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
			iobyte = 0;
		}
	}
}

void CGameFramework::ProcessPacket(char * ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_PUT_PLAYER:
	{
		sc_packet_put_player *my_packet = reinterpret_cast<sc_packet_put_player *>(ptr);
		int id = my_packet->id;

		if (first_time) {
			first_time = false;
			g_myid = id;

			if (my_packet->player_type == SWORDER)
			{
				m_cType = SWORDER;
				m_pPlayer = m_pTownPlayer = m_pTownSworderPlayer;
				m_pFieldPlayer = m_pFieldSworderPlayer;
			}
			if (my_packet->player_type == ASSASSIN)
			{
				m_cType = ASSASSIN;
				m_pTownPlayer = m_pPlayer = m_pTownAssassinPlayer;
				m_pFieldPlayer = m_pFieldAssasinPlayer;
			}
			m_pCamera = m_pPlayer->GetCamera();
		}
		if (id == g_myid) {
			XMFLOAT3 temp = { (float)(my_packet->x)*0.1f, 0.0, (float)my_packet->y*0.1f };
			temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);
			float temp_hp = my_packet->hp * 0.1f;
			float temp_mp = my_packet->mp * 0.1f;

			m_pPlayer->SetPosition(temp);
			m_pPlayer->SetVisible(true);
			m_pPlayer->SetScene(TOWN);

			SetHP(temp_hp);
			SetMP(temp_mp);
		}
		else if (id < MAX_USER)
		{

			if (m_ppOtherPlayers[id] == NULL)
			{
				PLAYERTYPE type = (PLAYERTYPE)(int)(my_packet->player_type);
				//otherplayer 생성
				if (type == SWORDER)			m_ppOtherPlayers[id] = m_pOtherSworderPlayer[id];
				else if (type == ASSASSIN)	m_ppOtherPlayers[id] = m_pOtherAssassinPlayer[id];

				//cout << id << "번째 캐릭터 생성" << endl;
			}


			XMFLOAT3 temp = { (float)my_packet->x*0.1f, 0.0, (float)my_packet->y*0.1f };
			XMFLOAT3 newlookvector = { (float)my_packet->sight_x*0.01f, 0.0, (float)my_packet->sight_z*0.01f };
			XMFLOAT3 currlookvector = m_ppOtherPlayers[id]->GetLookVector();
			temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);

			float degree = GetAngleBetweenTwoVectors2(newlookvector.x, newlookvector.z, currlookvector.x, currlookvector.z);
			float temp_hp = my_packet->hp * 0.1f;

			m_ppOtherPlayers[id]->Rotate(0.0, degree, 0.0);
			m_ppOtherPlayers[id]->SetVisible(true);
			m_ppOtherPlayers[id]->SetPosition(temp);
			m_ppOtherPlayers[id]->SetHp(temp_hp);
			m_ppOtherPlayers[id]->SetAnimation(my_packet->animation_num);
		}
		else
		{
			XMFLOAT3 temp = { (float)my_packet->x*0.1f, 0.0, (float)my_packet->y*0.1f };
			XMFLOAT3 newlookvector = { (float)my_packet->sight_x*0.01f, 0.0, (float)my_packet->sight_z*0.01f };
			XMFLOAT3 currlookvector = m_ppMonsters[id - MAX_USER]->GetLook();
			if (id - MAX_USER < 30)
				temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z) + 10.f;
			else
				temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);

			float degree = GetAngleBetweenTwoVectors2(newlookvector.x, newlookvector.z, currlookvector.x, currlookvector.z);
			float temp_hp = my_packet->hp * 0.1f;

			m_ppMonsters[id - MAX_USER]->Rotate(0.0, degree + 180.0, 0.0);
			m_ppMonsters[id - MAX_USER]->SetVisible(true);
			m_ppMonsters[id - MAX_USER]->SetPosition(temp);
			m_ppMonsters[id - MAX_USER]->SetHp(temp_hp);

			m_pCurrScene->UpdateMonHPIndex(id - MAX_USER, true, temp, temp_hp);
		}
		break;
	}
	case SC_POS:
	{
		sc_packet_pos *my_packet = reinterpret_cast<sc_packet_pos *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			XMFLOAT3 temp = { (float)my_packet->x*0.1f, 0.0,(float)my_packet->y*0.1f };
			temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);
			float temp_hp = my_packet->hp * 0.1f;
			float temp_mp = my_packet->mp * 0.1f;

			SetHP(temp_hp);
			SetMP(temp_mp);
			m_pPlayer->SetPosition(temp);
			m_pPlayer->SetIndieCamera(false, 0.f);

		}
		else if (other_id < MAX_USER)
		{
			XMFLOAT3 temp = { (float)my_packet->x*0.1f, 0.0, (float)my_packet->y*0.1f };
			XMFLOAT3 newlookvector = { (float)my_packet->sight_x*0.01f, 0.0, (float)my_packet->sight_z*0.01f };
			XMFLOAT3 currlookvector = m_ppOtherPlayers[other_id]->GetLookVector();
			temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);

			float degree = GetAngleBetweenTwoVectors2(newlookvector.x, newlookvector.z, currlookvector.x, currlookvector.z);
			float temp_hp = my_packet->hp * 0.1f;

			m_ppOtherPlayers[other_id]->Rotate(0.0, degree, 0.0);
			m_ppOtherPlayers[other_id]->SetPosition(temp);
			m_ppOtherPlayers[other_id]->SetVisible(true);
			m_ppOtherPlayers[other_id]->SetHp(temp_hp);
		}
		else
		{
			XMFLOAT3 temp = { (float)my_packet->x*0.1f, 0.0, (float)my_packet->y*0.1f };
			XMFLOAT3 newlookvector = { (float)my_packet->sight_x*0.01f, 0.0, (float)my_packet->sight_z*0.01f };
			XMFLOAT3 currlookvector = m_ppMonsters[other_id - MAX_USER]->GetLook();
			float degree = GetAngleBetweenTwoVectors2(newlookvector.x, newlookvector.z, currlookvector.x, currlookvector.z);

			if (other_id - MAX_USER < 30)
				temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z) + 10.f;
			else
				temp.y = m_pCurrScene->GetTerrain()->GetHeight(temp.x, temp.z);

			float temp_hp = my_packet->hp * 0.1f;

			m_ppMonsters[other_id - MAX_USER]->Rotate(0.0, degree + 180.0, 0.0);
			m_ppMonsters[other_id - MAX_USER]->SetPosition(temp);
			m_ppMonsters[other_id - MAX_USER]->SetVisible(true);
			m_ppMonsters[other_id - MAX_USER]->SetHp(temp_hp);
		}
		break;
	}
	case SC_REMOVE_PLAYER:
	{
		sc_packet_remove_player *my_packet = reinterpret_cast<sc_packet_remove_player *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid)
		{
			m_pPlayer->SetVisible(false);
		}
		else if (other_id < MAX_USER)
				m_ppOtherPlayers[other_id]->SetVisible(false);
		else
		{
			m_ppMonsters[other_id - MAX_USER]->SetVisible(false);
			m_ppMonsters[other_id - MAX_USER]->SetAnimation(IDLE);

			m_pCurrScene->UpdateMonHPIndex(other_id - MAX_USER, false, m_ppMonsters[other_id - MAX_USER]->GetPosition(), m_ppMonsters[other_id - MAX_USER]->GetHP());

			//몬스터가 죽었을 때
			if (my_packet->hp <= 0)
				///퀘스트 진행 중이라면
				if (m_bQuestING)
				{ //if(지금 때려잡는 몬스터 종류가 퀘스트랑 맞는지)
					auto QuestNum = (m_CurrQuestIdx + 1) / 2;
					if (other_id < 29) //초급 30마리
					{
						if (QuestNum == SPIDER)
							PlusQuestProgress();
					}
					else if (other_id < 49) //중급 20마리
					{
						if (QuestNum == CRAB)
							PlusQuestProgress();
					}
					else
					{
						if (QuestNum == GOLEM)
						{
							if (other_id == 50)
							{
								m_bItem = true;
								m_ppObjects[0]->SetPosition(m_ppMonsters[other_id - MAX_USER]->GetPosition());
							}
						}
					}
				}
		}
		break;
	}

	case SC_SCENE:
	{
		sc_packet_scene_player * my_packet = reinterpret_cast<sc_packet_scene_player *>(ptr);

		if (my_packet->scene == FIELD)
		{
			m_pPlayer->SetScene(FIELD);

			m_pCurrScene = m_pFieldScene;
			m_pPlayer = m_pFieldPlayer;
			m_pCurrScene->m_pPlayer = m_pPlayer;
			m_pPlayer->SetPosition(XMFLOAT3(512.0f, m_pCurrScene->GetTerrain()->GetHeight(512.f, 950.f), 900.0f));
			m_pCamera = m_pPlayer->GetCamera();

			for (int i = 0; i < MAX_USER; ++i)
				if (m_ppOtherPlayers[i] != NULL) m_ppOtherPlayers[i]->SetPlayerUpdatedContext(m_pCurrScene->GetTerrain());

		}

		if (my_packet->scene == TOWN)
		{
			m_pPlayer->SetScene(TOWN);
			m_pCurrScene = m_pTownScene;
			m_pPlayer = m_pTownPlayer;
			m_pCurrScene->m_pPlayer = m_pPlayer;
			m_pPlayer->SetPosition(XMFLOAT3(512.0f, m_pCurrScene->GetTerrain()->GetHeight(512.f, 100.f), 100.0f));

			for (int i = 0; i < MAX_USER; ++i)
				if (m_ppOtherPlayers[i] != NULL) m_ppOtherPlayers[i]->SetPlayerUpdatedContext(m_pCurrScene->GetTerrain());

			m_pCamera = m_pPlayer->GetCamera();
		}
		break;
	}

	case SC_ANIMATE:
	{
		sc_packet_animate *my_packet = reinterpret_cast<sc_packet_animate *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid)
		{
			//cout << other_id <<"번 애니메이션 패킷 : " << (int)my_packet->animate_num << endl;
			if (my_packet->animate_num == HIT)
			{
				hit_on = true;
				m_pPlayer->GetCamera()->Move(XMFLOAT3(-5.f, 0.f, 0.f));

			}
		}
		else if (other_id < MAX_USER)
		{
			m_ppOtherPlayers[other_id]->SetAnimation(my_packet->animate_num);

			if (my_packet->animate_num == ATTACK)
			{
				SetEffectPosition(other_id,
					XMFLOAT3(
						m_ppOtherPlayers[other_id]->GetPosition().x + m_ppOtherPlayers[other_id]->GetLookVector().x*20.f,
						m_ppOtherPlayers[other_id]->GetPosition().y,
						m_ppOtherPlayers[other_id]->GetPosition().z + m_ppOtherPlayers[other_id]->GetLookVector().z*20.f)
				);
				SetEffectStart(other_id, true);
				dynamic_cast<CEffectShader*>(m_ppShaders[other_id + SKILL])->SetAnimationFinish(!m_bEffect[other_id]);
			}
			else
			{
				SetEffectStart(other_id, false);
			}
		}
		else
		{
			m_ppMonsters[other_id - MAX_USER]->SetAnimation(my_packet->animate_num);
			//hit
			if (my_packet->animate_num == HIT)
			{
				if (other_id - MAX_USER < 30) //하급
					m_vSounds[3]->PlayWaveFile(0);
				else if (other_id - MAX_USER <= 40) //중급
					m_vSounds[4]->PlayWaveFile(0);
			}
		}
		break;
	}

	case SC_EXP:
	{
		sc_packet_exp *my_packet = reinterpret_cast<sc_packet_exp *>(ptr);
		int id = my_packet->id;

		if (id == g_myid)
		{
			m_currExp = (int)my_packet->exp;
		}

		break;
	}
	case SC_LEVELUP:
	{
		sc_packet_levelup *my_packet = reinterpret_cast<sc_packet_levelup *>(ptr);
		int id = my_packet->id;

		if (id == g_myid)
		{
			SetLevelDesign((int)my_packet->level);
			m_currExp = 0.f;
		}
		break;
	}

	case SC_INFO:
	{
		sc_packet_info *my_packet = reinterpret_cast<sc_packet_info *>(ptr);
		int id = my_packet->id;

		if (id == g_myid)
		{
			//레벨
			SetLevelDesign(my_packet->level);
			//경험치
			m_currExp = my_packet->exp;
			m_bQuestING = false;
			//퀘스트 1,2,3 여부
			if (my_packet->Q1)
			{
				m_nQuestClear = 1;
				m_CurrQuestIdx = 2;
				dynamic_cast<CUIShader*>(m_ppShaders[QUSET_PROGRESS])->SetIdx(m_CurrQuestIdx / 2);
			}
			if (my_packet->Q2)
			{
				m_nQuestClear = 2;
				m_CurrQuestIdx = 4;
				dynamic_cast<CUIShader*>(m_ppShaders[QUSET_PROGRESS])->SetIdx(m_CurrQuestIdx / 2);
			}
			if (my_packet->Q3)
			{
				m_nQuestClear = 3;
				m_CurrQuestIdx = 6;
				m_bQuestING = true;
			}
			m_QuestProgressDegree = 0;
		}
		break;
	}
	case SC_SIGHT:
	{
		sc_packet_sight *my_packet = reinterpret_cast<sc_packet_sight *>(ptr);

		int id = my_packet->id;

		if (id == g_myid)
		{
			XMFLOAT3 newlookvector = { (float)my_packet->sight_x*0.01f, 0.0, (float)my_packet->sight_z*0.01f };
			XMFLOAT3 currlookvector = m_pPlayer->GetLookVector();

			float degree = GetAngleBetweenTwoVectors2(newlookvector.x, newlookvector.z, currlookvector.x, currlookvector.z);

			m_pPlayer->Rotate(0.0, degree, 0.0);
		}
		else if (id < MAX_USER)
		{
			XMFLOAT3 newlookvector = { (float)my_packet->sight_x*0.01f, 0.0, (float)my_packet->sight_z*0.01f };
			XMFLOAT3 currlookvector = m_ppOtherPlayers[id]->GetLookVector();

			float degree = GetAngleBetweenTwoVectors2(newlookvector.x, newlookvector.z, currlookvector.x, currlookvector.z);

			m_ppOtherPlayers[id]->Rotate(0.0, degree, 0.0);
		}
		break;
	}
	case SC_EFFECT:
	{
		sc_packet_effect *my_packet = reinterpret_cast<sc_packet_effect *>(ptr);

		int id = my_packet->id;
		auto effect = my_packet->effect;
		
		m_ppShaders[SKILL+id]->SetType((int)effect);

		SoundManager(id, effect);

		if (id == g_myid)
		{
			if (effect == ASSASIN_SKILL)
				SetEffectPosition(g_myid, XMFLOAT3(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y, m_pPlayer->GetPosition().z));
			else
				SetEffectPosition(g_myid, XMFLOAT3(m_pPlayer->GetPosition().x + m_pPlayer->GetLookVector().x * 20.f, m_pPlayer->GetPosition().y, m_pPlayer->GetPosition().z + m_pPlayer->GetLookVector().z * 20.f));

			//스킬 조명 처리
			if (effect == ASSASIN_SKILL)
				m_pCurrScene->StartSkillLight();

			SetEffectStart(g_myid, true);
			dynamic_cast<CEffectShader*>(m_ppShaders[g_myid + SKILL])->SetAnimationFinish(!m_bEffect[g_myid]);
		}
		else if (id < MAX_USER)
		{
			if (effect == ASSASIN_SKILL)
				SetEffectPosition(id, XMFLOAT3(m_ppOtherPlayers[id]->GetPosition().x, m_ppOtherPlayers[id]->GetPosition().y, m_ppOtherPlayers[id]->GetPosition().z));
			else
				SetEffectPosition(g_myid, XMFLOAT3(m_ppOtherPlayers[id]->GetPosition().x + m_ppOtherPlayers[id]->GetLookVector().x * 20.f, m_ppOtherPlayers[id]->GetPosition().y, m_ppOtherPlayers[id]->GetPosition().z + m_ppOtherPlayers[id]->GetLookVector().z * 20.f));

			SetEffectStart(id, true);
			dynamic_cast<CEffectShader*>(m_ppShaders[id + SKILL])->SetAnimationFinish(!m_bEffect[id]);
		}

		break;
	}
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}
	

float CGameFramework::GetAngleBetweenTwoVectors2(float dVec1X, float dVec1Y, float dVec2X, float dVec2Y)
{
	float dAngle1 = RAD2DEG(atan2(dVec1X, dVec1Y));

	float dAngle2 = RAD2DEG(atan2(dVec2X, dVec2Y));

	float dDiffAngles = dAngle1 - dAngle2;


	if (dDiffAngles < 0)

		dDiffAngles = 360 + dDiffAngles;

	return dDiffAngles;
}

bool CGameFramework::CheckKeyTime(float fElapsedTime)
{
	m_fKeyWaitingTime += fElapsedTime;
	
	float time = 1.2f;

	if (m_cType == SWORDER) time = 1.26667f;
	
	if (m_fKeyWaitingTime > time)
	{
		m_fKeyWaitingTime = 0.0f;
		m_bIsAttacking = false;
		return true;
	}

	return false;
}

void CGameFramework::Server(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam)) {
		closesocket((SOCKET)wParam);
		exit(1);
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
		ReadPacket((SOCKET)wParam);
		break;
	case FD_CLOSE:
		closesocket((SOCKET)wParam);
		exit(1);
		break;
	}
}
