//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Mesh.h"
#include "Camera.h"
#include "Mydefine.h"

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT						0x04
#define DIR_RIGHT					0x08
#define DIR_UP							0x10
#define DIR_DOWN					0x20

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

class CShader;

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	UINT							m_nMaterial;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct SRVROOTARGUMENTINFO
{
	UINT														m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};

class CTexture
{
public:
	CTexture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE2D, int nSamplers = 0);
	virtual ~CTexture();

private:
	int										m_nReferences = 0;

	UINT									m_nTextureType = RESOURCE_TEXTURE2D;
	int										m_nTextures = 0;
	ID3D12Resource					**m_ppd3dTextures = NULL;
	ID3D12Resource					**m_ppd3dTextureUploadBuffers;
	SRVROOTARGUMENTINFO	*m_pRootArgumentInfos = NULL;

	int										m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex);

	int GetTextureCount() { return(m_nTextures); }
	ID3D12Resource *GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	UINT GetTextureType() { return(m_nTextureType); }

	void ReleaseUploadBuffers();
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4					m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	UINT							m_nReflection = 0;
	CTexture						*m_pTexture = NULL;
	CShader						*m_pShader = NULL;

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetTexture(CTexture *pTexture);
	void SetShader(CShader *pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGameObject
{
public:
	CGameObject(int nMeshes = 1);
	virtual ~CGameObject();

private:
	int								m_nReferences = 0;
private:
	bool m_bVisible = false;
	SCENE scene;
	float	m_hp;
	float	m_mp;
	float	m_MaxHP;
public:
	SCENE GetScene() { return scene; }
	void SetScene(SCENE s) { scene = s; }
	void SetHp(float hp) { m_hp = hp; }
	void SetMp(float mp) { m_mp = mp; }
	void SetMaxHP(float maxhp) {m_MaxHP = maxhp	;}
	float GetHP() { return m_hp; }
	float GetMP() { return m_mp; }
	float GetMaxMP() { return m_MaxHP; }
	void SetVisible(bool b) { m_bVisible = b; }
	bool GetVisible() { return m_bVisible; }
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	TCHAR							m_pstrFrameName[256];
	bool								m_bActive = true;

	XMFLOAT4X4				m_xmf4x4ToParentTransform;
	XMFLOAT4X4				m_xmf4x4ToRootTransform;
	XMFLOAT4X4				m_xmf4x4World;

	CMesh							**m_ppMeshes;
	int								m_nMeshes;
	BoundingOrientedBox		m_xmOOBB;

	CMaterial						*m_pMaterial = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

protected:
	ID3D12Resource							*m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO				*m_pcbMappedGameObject = NULL;

public:
	void SetMesh(int nIndex, CMesh *pMesh);
	void SetShader(CShader *pShader);
	void SetMaterial(CMaterial *pMaterial);

	void ResizeMeshes(int nMeshes);

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	virtual ID3D12Resource *CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void OnPrepareRender();
	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	virtual void BuildMaterials(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetXPosition(float x);
	void SetYPosition(float y);
	void SetZPosition(float z);
	void SetLocalPosition(XMFLOAT3 xmf3Position);
	void SetWorldPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetLocalScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4 *pxmf4Quaternion);

	void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent=NULL);

	virtual void UpdateBuffer(ID3D12GraphicsCommandList *pd3dCommandList) {}
	virtual bool FrameAdvance(float fTimeElapsed) { return false; }
	virtual void UpdateBoneTransform(ID3D12Device *pd3dDevice) {}
	virtual void UpdatePosition(float fTimeElapsed) {}
	virtual void SetAnimation(int nFBXAnimation) {};

public:
	CGameObject 					*m_pParent = NULL;
	CGameObject 					*m_pChild = NULL;
	CGameObject 					*m_pSibling = NULL;

	int Timer = 0;
	bool moveFlag = false;


	void SetChild(CGameObject *pChild);
	CGameObject *GetParent() { return(m_pParent); }

	//-----0305 추가한 코드-----
	void LoadGeometryFromFBX(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *	pszFileName, wchar_t* pszDDSFileName);
	//-----0420 추가한 코드-----
	void LoadNormalMappedGeometryFromFBX(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, char *	pszFileName, wchar_t* pszTextureFileName, wchar_t* pszNormalmapFileName);
	
	//-----0511 추가한 코드------
	void LoadGeometryFromSpiderFBX(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *	pszFileName, wchar_t* pszDDSFileName);
	//----shader에서 읽는 용도. 메쉬만 불러오는 거----
	void LoadMeshFromFBX(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,  char *	pszFileName );
};

//==========================================================================================================================================================================
// 상수 버퍼를 위한 구조체
struct CB_SKINNED_INFO
{
	XMMATRIX	m_xmtxBone[64];
};

struct BoneAnimationData
{
	int				 m_nFrameCount;
	float			*m_pfAniTime;
	XMFLOAT3		*m_pd3dxvScale;
	XMFLOAT3		*m_pd3dxvTranslate;
	XMFLOAT4		*m_pd3dxvQuaternion;
};

class CAnimatedModelObject : public CGameObject
{
public:
	CAnimatedModelObject(int nMeshes = 1);
	CAnimatedModelObject(PLAYERTYPE type, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *	pszFileName, wchar_t* pszDDSFileName, float scale);

	//-----0520 수정한 코드-----
	void LoadGeometryFromAnimatedFBX(PLAYERTYPE type, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *	pszFileName, wchar_t* pszDDSFileName, float scale);

	float					m_fFBXModelScale = 1.f;			// 모델의 사이즈 (보통 Animate에서 조절해주기 위함)
	XMFLOAT3			m_xmf3FBXModelRotate;			//모델의 회전 정보
	float					m_fFBXAnimationTime = 0.0f;
	int						m_nFBXAnimationNum = 0;
	int						m_nFBXMaxFrameNum = 0;			// 모델이 실행할 애니메이션의 최대 프레임 수.
	int						m_nFBXNowFrameNum = 0;		// 모델이 진행중인 애니메이션의 현재 프레임 값.

	BoneAnimationData	**m_ppBoneAnimationData;

	XMFLOAT3					*pxmf3Positions = NULL, *pxmf3Normals = NULL;
	XMFLOAT2					*pxmf3TextureCoords0 = NULL;
	XMFLOAT4					*pxmf4BoneIndices = NULL, *pxmf4BoneWeights = NULL;

	UINT							*m_pBoneHierarchy;
	XMFLOAT4X4				*m_pxm4x4BoneOffsets;
	XMMATRIX					*m_pxm4x4SQTTransform;
	XMFLOAT4X4				*m_pxm4x4FinalBone;

	XMFLOAT4X4				m_xmf4x4Frame[128];
	PLAYERTYPE				m_fbxType;
protected:
	// 뼈대 상수버퍼
	ID3D12Resource			* m_pd3dcbBones = NULL;
	CB_SKINNED_INFO		*m_pcbMappedBoneInfo = NULL;

public:
	//	뼈대 상수버퍼 전달 부분!+_+
	void CreateBuffer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseBuffer();
	virtual void UpdateBuffer(ID3D12GraphicsCommandList *pd3dCommandList);


	float GetFBXAnimationTime() { return m_fFBXAnimationTime; }
	int GetFBXAnimationNum() { return m_nFBXAnimationNum; }
	int GetFBXNowFrameNum() { return m_nFBXNowFrameNum; }
	int GetFBXMaxFrameNum() { return m_nFBXMaxFrameNum; }

	int m_nBoneCount;
	int m_nAnimationClip;

	virtual void UpdateBoneTransform(ID3D12Device *pd3dDevice);
	void MakeBoneMatrix(int nNowframe, int nAnimationNum, int nBoneNum, XMMATRIX& BoneMatrix);
	virtual bool FrameAdvance(float fTimeElapsed);
	virtual void SetAnimation(int nFBXAnimation);

	float						m_fRotationTime = 0.f;
	float						m_fTheta = 0.f;
	void UpdatePosition(float fTimeElapsed);

	bool IsAnimationFinish();
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CPlaneObject : public CGameObject
{
public:
	CPlaneObject(int nMeshes=1);
	virtual ~CPlaneObject();

public:
	virtual void Animate(float fTimeElapsed);
	//virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CFBXModel : public CGameObject
{
public:
	CFBXModel( ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char * pszFileNam, wchar_t* pszDDSFileNamee);

	virtual ~CFBXModel();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CFBXBumpedModel : public CGameObject
{
public:
	CFBXBumpedModel(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char * pszFileName, wchar_t* pszTextureFileName, wchar_t* pszNormalmapFileName);
	virtual ~CFBXBumpedModel();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,char type, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapImage * m_pHeightMapImage;

	int							m_nWidth;
	int							m_nLength;

	XMFLOAT3				m_xmf3Scale;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBillboardObject :public CGameObject
{
public:
	CBillboardObject(int nMeshes = 1);
	virtual ~CBillboardObject();

	virtual void Animate(float fTimeElapsed, CCamera * pCamera);
	void SetLookAt(XMFLOAT3& xmf3Target);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTreeModel : public CGameObject
{
public:
	CTreeModel(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, char * pszFileNam );

	virtual ~CTreeModel();
};