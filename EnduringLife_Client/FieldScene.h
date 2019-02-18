//-----------------------------------------------------------------------------
// File: FieldScene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"
#include "Scene.h"


class CFieldScene :public CScene
{
public:
	CFieldScene();
	~CFieldScene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();

	virtual void BuildLightsAndMaterials();
	virtual void FlashToggle() { m_pLights->m_pLights[1].m_bEnable = !m_pLights->m_pLights[1].m_bEnable; }
	virtual void SetMaterial(int nIndex, MATERIAL *pMaterial);
	virtual void UpdateMaterial(CGameObject *pObject);

	virtual ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	virtual ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	virtual void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual bool ProcessInput(UCHAR *pKeysBuffer);
	virtual  void AnimateObjects(float fTimeElapsed, CCamera *pCamera, ID3D12Device *m_pd3dDevice);
	virtual void StartSkillLight();
	virtual void EndSkillLight() {m_pLights->m_pLights[5].m_bEnable = false;}
	virtual  void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

	virtual void ReleaseUploadBuffers();

	virtual CHeightMapTerrain *GetTerrain() { return(m_pTerrain); }


	virtual void UpdateMonHPIndex(int idx, bool visible, XMFLOAT3 pos, float hp);
protected:
	ID3D12RootSignature			*m_pd3dGraphicsRootSignature = NULL;


	CGameObject			**m_ppObjects = NULL;
	int							m_nObjects = 0;

	CShader						**m_ppShaders = NULL;
	int							m_nShaders = 0;

	LIGHTS						*m_pLights = NULL;

	ID3D12Resource				*m_pd3dcbLights = NULL;
	LIGHTS						*m_pcbMappedLights = NULL;

	MATERIALS					*m_pMaterials = NULL;
	int							m_nMaterials = 0;

	ID3D12Resource				*m_pd3dcbMaterials = NULL;
	MATERIAL					*m_pcbMappedMaterials = NULL;

	CHeightMapTerrain			*m_pTerrain = NULL;
	CSkyBox					 	*m_pSkyBox = NULL;


};
