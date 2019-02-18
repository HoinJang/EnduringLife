//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 						m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 						m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool						m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
};

struct MATERIAL
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4				m_xmf4Emissive;
};

struct MATERIALS
{
	MATERIAL				m_pReflections[MAX_MATERIALS];
};

class CScene
{
public:
	CScene() ;
	~CScene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) ;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) ;

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)=0 ;
	virtual void ReleaseObjects();

	virtual void BuildLightsAndMaterials();
	virtual void FlashToggle() {}
	virtual void SetMaterial(int nIndex, MATERIAL *pMaterial);
	virtual void UpdateMaterial(CGameObject *pObject);

	virtual ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice) = 0;
	virtual ID3D12RootSignature *GetGraphicsRootSignature() = 0;// { return(m_pd3dGraphicsRootSignature); }
	virtual void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) = 0;// { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) ;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual bool ProcessInput(UCHAR *pKeysBuffer);
	virtual  void AnimateObjects(float fTimeElapsed, CCamera *pCamera, ID3D12Device *m_pd3dDevice);
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera) ;
	virtual void StartSkillLight() {}
	virtual void EndSkillLight() {}
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL) ;

	virtual void ReleaseUploadBuffers() ;

	virtual CHeightMapTerrain *GetTerrain() = 0;// { return(m_pTerrain); }

	virtual void SetHP(float hp) {}
	virtual void SetMP(float mp) {}
	
	virtual void UpdateMonHPIndex(int idx, bool visible, XMFLOAT3 pos, float hp) {}
	virtual void UpdateHPBar(int) {}

	CPlayer								*m_pPlayer = NULL;
};
