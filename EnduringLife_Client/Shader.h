//-----------------------------------------------------------------------------
// File: Shader.h
//-----------------------------------------------------------------------------

#pragma once

#include "Object.h"
#include "Camera.h"

struct CB_TEXTURE_MATRIX
{
	XMFLOAT4X4					    m_TexTransform;
};

struct CB_HP_INFO
{
	float						m_fHp;
};
struct CB_MP_INFO
{
	float						m_fMp;
};
struct CB_EXP_INFO
{
	float						m_fExp;
};
struct CB_INDEX
{
	float							m_Idx;
};
struct CB_EFFECT_INFO
{
	float							m_MaxIdx;
	float							m_SkillNum;
};
struct CB_UI_INFO
{
	float							m_fXPos;
	float							m_fYPos;
	float							m_fZPos;
	float							m_iMaxIdx;
};
struct CB_MON_HP_INFO
{
	float	m_HPratio;
};
///////////////////////////////////////////////////////////////////////////////////
class CShader
{
public:
	CShader();
	virtual ~CShader();

private:
	int													m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	D3D12_BLEND_DESC CreateAlphaBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateAlphaBlendShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature);

	void CreateCbvAndSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews);
	void CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	void CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);
	virtual void UpdateMonHPIndex(int idx, bool visible, XMFLOAT3 pos, float hp) {};


	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext = NULL) { }
	virtual void AnimateObjects(float fTimeElapsed, CCamera *pCamera) { }
	virtual void ReleaseObjects() { }

	virtual void ReleaseUploadBuffers();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void SetVisible(bool visible) {  }
	virtual void SetType(int type) {  }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

protected:
	ID3D12PipelineState * *m_ppd3dPipelineStates = NULL;
	int															m_nPipelineStates = 0;

	ID3D12DescriptorHeap							*m_pd3dCbvSrvDescriptorHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CIlluminatedShader : public CShader
{
public:
	CIlluminatedShader();
	virtual ~CIlluminatedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTexturedShader : public CShader
{
public:
	CTexturedShader();
	virtual ~CTexturedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void UpdateMonHPIndex(int idx, bool visible, XMFLOAT3 pos, float hp) {};

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define _WITH_BATCH_MATERIAL
class CTitleShader : public CTexturedShader
{
public:
	CTitleShader();
	virtual ~CTitleShader();

	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int Xsize, int Ysize, float xPosition, float zPosition, void *pContext, CTexture * pTexture);
	virtual void ReleaseObjects();


	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

protected:
	CGameObject * *m_ppObjects = 0;
	int											m_nObjects = 0;

	ID3D12Resource						*m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO			*m_pcbMappedGameObjects = NULL;

#ifdef _WITH_BATCH_MATERIAL
	CMaterial						*m_pMaterial = NULL;
#endif
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CIlluminatedTexturedShader : public CTexturedShader
{
public:
	CIlluminatedTexturedShader();
	virtual ~CIlluminatedTexturedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//



#define _WITH_BATCH_MATERIAL

class CBillboardShader : public CTexturedShader
{
public:
	CBillboardShader();
	virtual ~CBillboardShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, PlantType type, float xPosition, float zPosition, float Xsize, float Ysize, void *pContext, CTexture *pTexture);
	virtual void AnimateObjects(float fTimeElapsed, CCamera* pCamera);
	virtual void ReleaseObjects();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

protected:
	CGameObject * *m_ppObjects = 0;
	int											m_nObjects = 0;
	PlantType								m_Type;

	ID3D12Resource						*m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO			*m_pcbMappedGameObjects = NULL;

#ifdef _WITH_BATCH_MATERIAL
	CMaterial						*m_pMaterial = NULL;
#endif
};
/////////////////////////////////////////////////////////////////////////////////
class CMonHPShader : public CBillboardShader
{
public:
	CMonHPShader();
	virtual ~CMonHPShader();
	
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float Xsize, float Ysize);

	virtual void AnimateObjects(float fTimeElapsed, CCamera* pCamera);
	virtual void ReleaseObjects();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void UpdateMonHPIndex(int idx, bool visible, XMFLOAT3 pos, float hp);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

public:
	CGameObject						**m_ppObjects = 0;
	int											m_nObjects = 0;
	QUESTNUM								m_Type;  //몬스터 타입


	ID3D12Resource					*m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO		*m_pcbMappedGameObjects = NULL;

	float									m_MaxHP;    //레벨에 따른 Max 체력
	float									currHP = 0;
	CB_MON_HP_INFO				*m_pHP = NULL;
	ID3D12Resource					*m_pd3dcbHPInfo = NULL;
	CB_MON_HP_INFO				*m_pcbMappedHPInfo = NULL;

#ifdef _WITH_BATCH_MATERIAL
	CMaterial						*m_pMaterial = NULL;
#endif
};
//////////////////////////////////////////////////////////////////////////////////
class CWaterShader : public CTexturedShader
{
public:
	CWaterShader();
	virtual ~CWaterShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);


	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

protected:
	CGameObject * *m_ppObjects = 0;
	int										m_nObjects = 0;

	ID3D12Resource					*m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO		*m_pcbMappedGameObjects = NULL;

	ID3D12Resource					*m_pd3dcbTexObjects = NULL;
	CB_TEXTURE_MATRIX		*m_pcbMappedTexObjects = NULL;

	XMFLOAT4X4						m_xmf4x4Texture;


#ifdef _WITH_BATCH_MATERIAL
	CMaterial						*m_pMaterial = NULL;
#endif
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTerrainShader : public CTexturedShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	//CPlayer *GetPlayer(int nIndex = 0) { return((CPlayer *)m_ppObjects[nIndex]); }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBoxShader : public CTexturedShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
};

///////////////////////////////////////////////////////////////////

class CFountainShader : public CIlluminatedTexturedShader
{

public:
	CFountainShader();
	virtual ~CFountainShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CHPBarShader : public CTexturedShader
{
public:
	CHPBarShader();
	virtual ~CHPBarShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float xPosition, float zPosition, float Xsize, float Ysize, CTexture *pTexture);
	virtual void SetHP(float currhp) { currHP = currhp; }

	virtual void ReleaseObjects();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

protected:
	CGameObject * *m_ppObjects = 0;
	int								m_nObjects = 0;

	CPlaneObject				*pPlaneObject = NULL;

	float							currHP = 0;
	CB_HP_INFO				*m_pHP = NULL;
	ID3D12Resource			*m_pd3dcbHPInfo = NULL;
	CB_HP_INFO				*m_pcbMappedHPInfo = NULL;


#ifdef _WITH_BATCH_MATERIAL
	CMaterial				*m_pMaterial = NULL;
#endif
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CMPBarShader : public CTexturedShader
{
public:
	CMPBarShader();
	virtual ~CMPBarShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float xPosition, float zPosition, float Xsize, float Ysize, CTexture *pTexture);
	virtual void SetMP(float currmp) { currMP = currmp; }

	virtual void ReleaseObjects();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

protected:
	CGameObject * *m_ppObjects = 0;
	int						m_nObjects = 0;

	CPlaneObject			*pPlaneObject = NULL;

	float						currMP = 0;
	CB_MP_INFO				*m_pMP = NULL;
	ID3D12Resource			*m_pd3dcbMPInfo = NULL;
	CB_MP_INFO				*m_pcbMappedMPInfo = NULL;


#ifdef _WITH_BATCH_MATERIAL
	CMaterial				*m_pMaterial = NULL;
#endif
};
//////////////////////////////////////////////////////////////////////////////////////
class CEXPBarShader : public CTexturedShader
{
public:
	CEXPBarShader();
	virtual ~CEXPBarShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float xPosition, float zPosition, float Xsize, float Ysize, CTexture *pTexture);
	virtual void SetEXP(float exp) {	currEXP = exp; }

	virtual void ReleaseObjects();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

protected:
	CGameObject * *m_ppObjects = 0;
	int						m_nObjects = 0;

	CPlaneObject			*pPlaneObject = NULL;

	float							currEXP = 0;
	CB_EXP_INFO				*m_pEXP = NULL;
	ID3D12Resource			*m_pd3dcbEXPInfo = NULL;
	CB_EXP_INFO				*m_pcbMappedEXPInfo = NULL;


#ifdef _WITH_BATCH_MATERIAL
	CMaterial				*m_pMaterial = NULL;
#endif
};
///////////////////////////////////////////////////////////////////////////////////////
class CEffectShader : public CBillboardShader
{
public:
	CEffectShader();
	virtual ~CEffectShader();
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float Xsize, float Ysize);
	virtual void AnimateObjects(float fTimeElapsed, CCamera* pCamera);
	virtual void ReleaseObjects();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

	void SetMaxIdx(int idx) { m_MaxIdx = idx; }
	void UpdateIdx(float fTimeElapsed);
	void SetAnimationFinish(bool bFinish);
	bool GetAnimationFinish() { return m_IsAnimationFinish; }
	void SetEffectPosition(XMFLOAT3 pos);

	//bool isRotate = false;
	int m_SkillType = SworderDefault;
	virtual void SetType(int type);
	int GetType() { return m_SkillType; }

protected:
	float										m_fAnimationTime{ 0.f };

	CGameObject							**m_ppObjects = 0;
	int											m_nObjects = 0;
	CBillboardObject					 *pEffectObject = NULL;
	CPlaneObject							 *pMagicSquareObject = NULL;
	bool											m_IsAnimationFinish{ true };

	float										m_Idx = 0;

	CB_INDEX								*m_pIdx = NULL;
	ID3D12Resource						*m_pd3dcbIdxInfo = NULL;
	CB_INDEX								*m_pcbMappedIdxInfo = NULL;
	//
	int											m_MaxIdx = 0;
	//int											m_SkillRepeat = 0;
	CB_EFFECT_INFO					*m_pMaxIdxInfo = NULL;
	ID3D12Resource						*m_pd3dcbMaxIdxInfo = NULL;
	CB_EFFECT_INFO					*m_pcbMappedMaxIdxInfo = NULL;
	//

	ID3D12Resource						*m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO			*m_pcbMappedGameObjects = NULL;

#ifdef _WITH_BATCH_MATERIAL
	CMaterial									*m_pMaterial = NULL;
#endif
};
///////////////////////////////////////////////////////////////////////////////////////
class CUIShader : public CTexturedShader
{
public:
	CUIShader();
	virtual ~CUIShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float xPosition, float zPosition, float Xsize, float Ysize, CTexture *pTexture);
	void SetUIpos(float x, float y, float z) { m_fXPos = x; m_fYPos = y; m_fZPos = z; }
	void SetMaxIdx(int idx) { m_MaxIdx = idx; }
	void PlusUIIdx() { m_iIdx++; }
	void MinusUIIdx() { m_iIdx--; }
	void SetIdx(int i) { m_iIdx = i; }

	virtual void ReleaseObjects();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void SetVisible(bool visible) { m_visible = visible; }
protected:
	CGameObject * *m_ppObjects = 0;
	int								m_nObjects = 0;

	CPlaneObject				*pPlaneObject = NULL;

	bool								m_visible = false;
	//-----------------------------------------------
	float							m_fXPos, m_fYPos, m_fZPos;
	int								m_MaxIdx = -1;
	CB_UI_INFO				*m_UIInfo = NULL;
	ID3D12Resource			*m_pd3dcbUIInfo = NULL;
	CB_UI_INFO				*m_pcbMappedUIInfo = NULL;

	float							m_iIdx = -1;
	CB_INDEX					*m_pIdx = NULL;
	ID3D12Resource			*m_pd3dcbIdxInfo = NULL;
	CB_INDEX					*m_pcbMappedIdxInfo = NULL;
	//-----------------------------------------------

#ifdef _WITH_BATCH_MATERIAL
	CMaterial					*m_pMaterial = NULL;
#endif
};
///////////////////////////////////////////////////////////////////////////////////////
class CAnimatedFBXShader : public CIlluminatedTexturedShader
{
public:
	CAnimatedFBXShader() {};
	virtual ~CAnimatedFBXShader() {};

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};
///////////////////////////////////////////////////////////////////////////////////////

class CTreeShader : public CTexturedShader
{
public:
	CTreeShader();
	virtual ~CTreeShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, PlantType type, float xPosition, float zPosition, float Xsize, float Ysize, void *pContext, CTexture *pTexture);
	virtual void AnimateObjects(float fTimeElapsed, CCamera* pCamera);
	virtual void ReleaseObjects();

	D3D12_INPUT_LAYOUT_DESC CTreeShader::CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

protected:
	CGameObject * *m_ppObjects = 0;
	int											m_nObjects = 0;
	PlantType								m_Type;

	ID3D12Resource						*m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO			*m_pcbMappedGameObjects = NULL;

#ifdef _WITH_BATCH_MATERIAL
	CMaterial						*m_pMaterial = NULL;
#endif
};