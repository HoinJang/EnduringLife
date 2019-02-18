//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
DWORD g_Timer = 0;
static float ANIFRAMETIME = 0.0333333f;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers)
{
	m_nTextureType = nTextureType;
	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppd3dTextureUploadBuffers = new ID3D12Resource*[m_nTextures];
		m_ppd3dTextures = new ID3D12Resource*[m_nTextures];
	}

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; ++i) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
	}

	if (m_pRootArgumentInfos)
	{
		delete[] m_pRootArgumentInfos;
	}

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_nTextureType == RESOURCE_TEXTURE2D_ARRAY)
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	}
	else
	{
		for (int i = 0; i < m_nTextures; ++i)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_d3dSrvGpuDescriptorHandle);
		}
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[nIndex].m_nRootParameterIndex, m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle);
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++)
			if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex)
{
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
	if (m_pTexture) m_pTexture->Release();
	if (m_pShader) m_pShader->Release();
}

void CMaterial::SetTexture(CTexture *pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMaterial::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject(int nMeshes)
{
	m_xmf4x4ToParentTransform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity(); //단위행렬

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	ReleaseShaderVariables();

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pMaterial) m_pMaterial->Release();

	if (m_pSibling) delete m_pSibling;
	if (m_pChild) delete m_pChild;
}

void CGameObject::ResizeMeshes(int nMeshes)
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
		m_ppMeshes = NULL;
	}

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh*[m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	}
}

void CGameObject::SetChild(CGameObject *pChild)
{
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
	if (pChild) pChild->m_pParent = this;
}

void CGameObject::SetMesh(int nIndex, CMesh *pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CGameObject::SetShader(CShader *pShader)
{
	if (!m_pMaterial)
	{
		CMaterial *pMaterial = new CMaterial();
		SetMaterial(pMaterial);
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void CGameObject::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

ID3D12Resource *CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObject = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObject->Map(0, NULL, (void **)&m_pcbMappedGameObject);

	return(m_pd3dcbGameObject);
}

void CGameObject::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObject)
	{
		m_pd3dcbGameObject->Unmap(0, NULL);
		m_pd3dcbGameObject->Release();
	}
	if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	if (m_pMaterial) m_pcbMappedGameObject->m_nMaterial = m_pMaterial->m_nReflection;
}

void CGameObject::Animate(float fTimeElapsed)
{
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);	

	//for (int i = 0; i < m_nMeshes; i++)	
	//{
	//	m_ppMeshes[i]->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World)); //월드좌표계로의 변환
	//	XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	//}
}

void CGameObject::SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (!m_bActive) return;

	OnPrepareRender();

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	if (m_ppMeshes && (m_nMeshes > 0))
	{
		SetRootParameter(pd3dCommandList);

		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}


void CGameObject::ReleaseUploadBuffers()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}

	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParentTransform, *pxmf4x4Parent) : m_xmf4x4ToParentTransform;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4ToParentTransform._41 = x;
	m_xmf4x4ToParentTransform._42 = y;
	m_xmf4x4ToParentTransform._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetXPosition(float x)
{
	m_xmf4x4ToParentTransform._41 = x;

}

void CGameObject::SetYPosition(float y)
{
	m_xmf4x4ToParentTransform._42 = y;
}

void CGameObject::SetZPosition(float z)
{
	m_xmf4x4ToParentTransform._43 = z;

}
void CGameObject::SetWorldPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetLocalPosition(XMFLOAT3 xmf3Position)
{
	XMMATRIX mtxTranslation = XMMatrixTranslation(xmf3Position.x, xmf3Position.y, xmf3Position.z);
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(m_xmf4x4ToParentTransform, mtxTranslation);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParentTransform);
}

void CGameObject::SetLocalScale(float x, float y, float z)
{
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

void CGameObject::Rotate(XMFLOAT4 *pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

#define _WITH_DEBUG_FRAME_HIERARCHY

void CGameObject::LoadGeometryFromFBX(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, char *	pszFileName, wchar_t* pszDDSFileName)
{
	int nVertices = 0, nIndices = 0, m_nBoneCount, m_nAnimationClip;
	XMFLOAT3 *pxmf3Positions = NULL, *pxmf3Normals = NULL;
	XMFLOAT2 *pxmf3TextureCoords0 = NULL;
	UINT *pnIndices = NULL;

	ifstream fin;

	char* ch = new char[200];
	char* sToken = new char[50];
	char* temp;

	fin.open(pszFileName);
	if (!fin.fail())
	{
		// 데이터를 읽어와 필요한 정점, 인덱스, 본, 애니메이션 수 파악
		fin.getline(ch, 200);
		fin.getline(ch, 200);
		nVertices = stoi(ch);
		fin.getline(ch, 200);
		nIndices = stoi(ch);
		fin.getline(ch, 200);
		m_nBoneCount = stoi(ch);
		fin.getline(ch, 200);

		// 정점 데이터를 저장
		fin.getline(ch, 200);
		fin.getline(ch, 200);

		pxmf3Positions = new XMFLOAT3[nVertices];
		pxmf3Normals = new XMFLOAT3[nVertices];
		pxmf3TextureCoords0 = new XMFLOAT2[nVertices];

		for (int i = 0; i < nVertices; i++)
		{
			// Vertice
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);	 pxmf3Positions[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Positions[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Positions[i].z = stof(sToken);
			// Normal
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pxmf3Normals[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Normals[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Normals[i].z = stof(sToken);
			// UV
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pxmf3TextureCoords0[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3TextureCoords0[i].y = stof(sToken);
			if(m_nBoneCount)
			{		
				fin.getline(ch, 200);
				fin.getline(ch, 200);   //지금 무시이...
			}
		}

		// 인덱스 데이터 저장
		fin.getline(ch, 200);
		pnIndices = new UINT[nIndices];

		for (int i = 0; i < nIndices; i += 3)
		{
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pnIndices[i] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); pnIndices[i + 1] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); pnIndices[i + 2] = stoi(sToken);
		}
		
		CMesh *pMesh = NULL;
		CMaterial *pMaterial = NULL;
		if (pnIndices > 0)
		{
			if (nVertices > 0) pMesh = new CMeshIlluminatedTextured(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, pxmf3Normals, pxmf3TextureCoords0, nIndices, pnIndices);

			CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
			pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pszDDSFileName, 0);
			pMaterial = new CMaterial();

			pMaterial->SetTexture(pTexture);

			UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

			ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

			CIlluminatedTexturedShader *pShader = new CIlluminatedTexturedShader();
			pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
			pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
			pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
			pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
			pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

			SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

			pMaterial->SetShader(pShader);
		}
		else
		{
			if (nVertices > 0) pMesh = new CMesh(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, nIndices, pnIndices);
		}

		if (pMesh)
			SetMesh(0, pMesh);
		else
			ResizeMeshes(0);

		if (pMaterial) SetMaterial(pMaterial);
		if (pxmf3Positions) delete[] pxmf3Positions;
		if (pxmf3Normals) delete[] pxmf3Normals;
		if (pxmf3TextureCoords0) delete[] pxmf3TextureCoords0;
		if (pnIndices) delete[] pnIndices;


	}
}
//-----------------------------------------------------------------------------------------------------------------------



void CGameObject::LoadGeometryFromSpiderFBX(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, char *	pszFileName, wchar_t* pszDDSFileName)
{
	int nVertices = 0, nIndices = 0, m_nBoneCount;// , m_nAnimationClip;
	XMFLOAT3 *pxmf3Positions = NULL, *pxmf3Normals = NULL;
	XMFLOAT2 *pxmf3TextureCoords0 = NULL;
	UINT *pnIndices = NULL;

	ifstream fin;

	char* ch = new char[200];
	char* sToken = new char[50];
	char* temp;

	fin.open(pszFileName);
	if (!fin.fail())
	{
		// 데이터를 읽어와 필요한 정점, 인덱스, 본, 애니메이션 수 파악
		fin.getline(ch, 200);
		fin.getline(ch, 200);
		nVertices = stoi(ch);
		fin.getline(ch, 200);
		nIndices = stoi(ch);
		fin.getline(ch, 200);
		m_nBoneCount = stoi(ch);
		fin.getline(ch, 200);

		// 정점 데이터를 저장
		fin.getline(ch, 200);
		fin.getline(ch, 200);

		pxmf3Positions = new XMFLOAT3[nVertices];
		pxmf3Normals = new XMFLOAT3[nVertices];
		pxmf3TextureCoords0 = new XMFLOAT2[nVertices];

		for (int i = 0; i < nVertices; i++)
		{
			// Vertice
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);	 pxmf3Positions[i].x = 3.f*stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Positions[i].y =3.f*stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Positions[i].z = 3.f*stof(sToken);
			// Normal
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pxmf3Normals[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Normals[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Normals[i].z = stof(sToken);
			// UV
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pxmf3TextureCoords0[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3TextureCoords0[i].y = stof(sToken);
			if (m_nBoneCount)
			{
				fin.getline(ch, 200);
				fin.getline(ch, 200);   //지금 무시이...
			}
		}

		// 인덱스 데이터 저장
		fin.getline(ch, 200);
		pnIndices = new UINT[nIndices];

		for (int i = 0; i < nIndices; i += 3)
		{
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pnIndices[i] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); pnIndices[i + 1] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); pnIndices[i + 2] = stoi(sToken);
		}

		CMesh *pMesh = NULL;
		CMaterial *pMaterial = NULL;
		if (pnIndices > 0)
		{
			if (nVertices > 0) pMesh = new CMeshIlluminatedTextured(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, pxmf3Normals, pxmf3TextureCoords0, nIndices, pnIndices);

			CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
			pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pszDDSFileName, 0);
			pMaterial = new CMaterial();

			pMaterial->SetTexture(pTexture);

			//
			UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

			ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);

			CIlluminatedTexturedShader *pShader = new CIlluminatedTexturedShader();
			pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
			pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
			
			pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
			pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
			pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, ROOT_PARAMETER_TEXTURE, true);

			SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

			pMaterial->SetShader(pShader);
			//
		}
		else
		{
			if (nVertices > 0) pMesh = new CMesh(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, nIndices, pnIndices);
		}

		if (pMesh)
			SetMesh(0, pMesh);
		else
			ResizeMeshes(0);

		if (pMaterial) SetMaterial(pMaterial);
		if (pxmf3Positions) delete[] pxmf3Positions;
		if (pxmf3Normals) delete[] pxmf3Normals;
		if (pxmf3TextureCoords0) delete[] pxmf3TextureCoords0;
		if (pnIndices) delete[] pnIndices;


	}
}


void CGameObject::LoadMeshFromFBX(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, char *	pszFileName )
{
	int nVertices = 0, nIndices = 0, m_nBoneCount;// , m_nAnimationClip;
	XMFLOAT3 *pxmf3Positions = NULL, *pxmf3Normals = NULL;
	XMFLOAT2 *pxmf3TextureCoords0 = NULL;
	UINT *pnIndices = NULL;

	ifstream fin;

	char* ch = new char[200];
	char* sToken = new char[50];
	char* temp;

	fin.open(pszFileName);
	if (!fin.fail())
	{
		// 데이터를 읽어와 필요한 정점, 인덱스, 본, 애니메이션 수 파악
		fin.getline(ch, 200);
		fin.getline(ch, 200);
		nVertices = stoi(ch);
		fin.getline(ch, 200);
		nIndices = stoi(ch);
		fin.getline(ch, 200);
		m_nBoneCount = stoi(ch);
		fin.getline(ch, 200);

		// 정점 데이터를 저장
		fin.getline(ch, 200);
		fin.getline(ch, 200);

		pxmf3Positions = new XMFLOAT3[nVertices];
		pxmf3Normals = new XMFLOAT3[nVertices];
		pxmf3TextureCoords0 = new XMFLOAT2[nVertices];

		for (int i = 0; i < nVertices; i++)
		{
			// Vertice
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);	 pxmf3Positions[i].x = 3.f*stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Positions[i].y = 3.f*stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Positions[i].z = 3.f*stof(sToken);
			// Normal
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pxmf3Normals[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Normals[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Normals[i].z = stof(sToken);
			// UV
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pxmf3TextureCoords0[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3TextureCoords0[i].y = stof(sToken);
			if (m_nBoneCount)
			{
				fin.getline(ch, 200);
				fin.getline(ch, 200);   //지금 무시이...
			}
		}

		// 인덱스 데이터 저장
		fin.getline(ch, 200);
		pnIndices = new UINT[nIndices];

		for (int i = 0; i < nIndices; i += 3)
		{
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pnIndices[i] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); pnIndices[i + 1] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); pnIndices[i + 2] = stoi(sToken);
		}

		CMesh *pMesh = NULL;
		CMaterial *pMaterial = NULL;

	
		if (nVertices > 0) pMesh = new CMeshIlluminatedTextured(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, pxmf3Normals, pxmf3TextureCoords0, nIndices, pnIndices);

		if (pMesh)
			SetMesh(0, pMesh);
		else
			ResizeMeshes(0);

		if (pMaterial) SetMaterial(pMaterial);
		if (pxmf3Positions) delete[] pxmf3Positions;
		if (pxmf3Normals) delete[] pxmf3Normals;
		if (pxmf3TextureCoords0) delete[] pxmf3TextureCoords0;
		if (pnIndices) delete[] pnIndices;
	}
}

//--------------------------------------------------------------------------

void CGameObject::LoadNormalMappedGeometryFromFBX(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, char *	pszFileName, wchar_t* pszTextureFileName, wchar_t* pszNormalmapFileName)
{
	int nVertices = 0, nIndices = 0, m_nBoneCount, m_nAnimationClip;
	XMFLOAT3 *pxmf3Positions = NULL, *pxmf3Normals = NULL;
	XMFLOAT2 *pxmf3TextureCoords0 = NULL;
	XMFLOAT3  *pxmf3Tangents = NULL;
	UINT *pnIndices = NULL;

	ifstream fin;

	char* ch = new char[200];
	char* sToken = new char[50];
	char* temp;

	fin.open(pszFileName);
	if (!fin.fail())
	{
		// 데이터를 읽어와 필요한 정점, 인덱스, 본, 애니메이션 수 파악
		fin.getline(ch, 200);
		fin.getline(ch, 200);
		nVertices = stoi(ch);
		fin.getline(ch, 200);
		nIndices = stoi(ch);
		fin.getline(ch, 200);

		m_nBoneCount = stoi(ch);
		fin.getline(ch, 200);

		// 정점 데이터를 저장
		fin.getline(ch, 200);
		fin.getline(ch, 200);

		pxmf3Positions = new XMFLOAT3[nVertices];
		pxmf3Normals = new XMFLOAT3[nVertices];
		pxmf3TextureCoords0 = new XMFLOAT2[nVertices];
		pxmf3Tangents = new XMFLOAT3[nVertices];;

		for (int i = 0; i < nVertices; i++)
		{
			// Vertice
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);	 pxmf3Positions[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Positions[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Positions[i].z = stof(sToken);
			// Normal
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pxmf3Normals[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Normals[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Normals[i].z = stof(sToken);
			// UV
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pxmf3TextureCoords0[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3TextureCoords0[i].y = stof(sToken);
			if (m_nBoneCount)
			{
				fin.getline(ch, 200);
				fin.getline(ch, 200);   //지금 무시...
			}
		}

		// 인덱스 데이터 저장
		fin.getline(ch, 200);
		pnIndices = new UINT[nIndices];

		for (int i = 0; i < nIndices; i += 3)
		{
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pnIndices[i] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); pnIndices[i + 1] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); pnIndices[i + 2] = stoi(sToken);
		}

		fin.getline(ch, 200);
		fin.getline(ch, 200);

		for (int i = 0; i < nVertices; i++)
		{
			//Tangent
			fin >> pxmf3Tangents[i].x >> pxmf3Tangents[i].y >> pxmf3Tangents[i].z;
		}

		CMesh *pMesh = NULL;
		CMaterial *pMaterial = NULL;
		if (pnIndices > 0)
		{
			if (nVertices > 0) pMesh = new CMeshIlluminatedTexturedNormalMapped(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, pxmf3Normals, pxmf3TextureCoords0,pxmf3Tangents, nIndices, pnIndices);
			//바꾸자!
			CTexture *pTexture = new CTexture(2, RESOURCE_TEXTURE2D, 0);
			pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pszTextureFileName, 0);
			pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pszNormalmapFileName, 1);

			pMaterial = new CMaterial();

			pMaterial->SetTexture(pTexture);

			UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

			ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
			//-------------------------------------------------------------------
			CFountainShader *pShader = new CFountainShader();
			pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
			pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
			pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 9);
			pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
			pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 13, true);
			//-------------------------------------------------------------------

			SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

			pMaterial->SetShader(pShader);
		}
		else
		{
			if (nVertices > 0) pMesh = new CMesh(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, nIndices, pnIndices);
		}

		if (pMesh)
			SetMesh(0, pMesh);
		else
			ResizeMeshes(0);



		if (pMaterial) SetMaterial(pMaterial);
		if (pxmf3Positions) delete[] pxmf3Positions;
		if (pxmf3Normals) delete[] pxmf3Normals;
		if (pxmf3TextureCoords0) delete[] pxmf3TextureCoords0;
		if (pxmf3Tangents)delete[] pxmf3Tangents;
		if (pnIndices) delete[] pnIndices;
	}
}

//-----------------------------------------------------------------------------------------------------------------------



////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimatedModelObject::CAnimatedModelObject(int nMeshes) :CGameObject(nMeshes) {};

CAnimatedModelObject::CAnimatedModelObject(PLAYERTYPE type, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *	pszFileName, wchar_t* pszDDSFileName, float scale)
{
	LoadGeometryFromAnimatedFBX(type, pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pszFileName, pszDDSFileName, scale);
	m_fbxType = type;
}

//애니메이션 정보 로드할 것
void CAnimatedModelObject::LoadGeometryFromAnimatedFBX(PLAYERTYPE type, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, char *	pszFileName, wchar_t* pszDDSFileName, float scale)
{
	m_fbxType = type;

	int nVertices = 0, nIndices = 0;

	UINT *pnIndices = NULL;

	ifstream fin;

	char* ch = new char[200];
	char* sToken = new char[50];
	char* temp;

	fin.open(pszFileName);
	if (!fin.fail())
	{
		// 데이터를 읽어와 필요한 정점, 인덱스, 본, 애니메이션 수 파악
		fin.getline(ch, 200);
		fin.getline(ch, 200);
		nVertices = stoi(ch);
		fin.getline(ch, 200);
		nIndices = stoi(ch);
		fin.getline(ch, 200);
		m_nBoneCount = stoi(ch);
		fin.getline(ch, 200);
		m_nAnimationClip = stoi(ch);

		// [ VERTEX_DATA ]
		fin.getline(ch, 200);

		pxmf3Positions = new XMFLOAT3[nVertices];
		pxmf3Normals = new XMFLOAT3[nVertices];
		pxmf3TextureCoords0 = new XMFLOAT2[nVertices];
		if (m_nBoneCount)
		{
			pxmf4BoneIndices = new XMFLOAT4[nVertices];
			pxmf4BoneWeights = new XMFLOAT4[nVertices];
		}
		for (int i = 0; i < nVertices; i++)
		{
			// Vertice
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);	 pxmf3Positions[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Positions[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Positions[i].z = stof(sToken);
			// Normal
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pxmf3Normals[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Normals[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3Normals[i].z = stof(sToken);
			// UV
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pxmf3TextureCoords0[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); pxmf3TextureCoords0[i].y = stof(sToken);
			if (m_nBoneCount)
			{	// BoneIndice
				fin.getline(ch, 200);
				sToken = strtok_s(ch, " ", &temp); 	 pxmf4BoneIndices[i].x = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); pxmf4BoneIndices[i].y = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); pxmf4BoneIndices[i].z = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); pxmf4BoneIndices[i].w = stof(sToken);
				// BoneWeight
				fin.getline(ch, 200);
				sToken = strtok_s(ch, " ", &temp);   pxmf4BoneWeights[i].x = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); pxmf4BoneWeights[i].y = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); pxmf4BoneWeights[i].z = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); pxmf4BoneWeights[i].w = stof(sToken);

			}
		}

		// 인덱스 데이터 저장
		fin.getline(ch, 200);
		pnIndices = new UINT[nIndices];

		for (int i = 0; i < nIndices; i += 3)
		{
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   pnIndices[i] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); pnIndices[i + 1] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); pnIndices[i + 2] = stoi(sToken);
		}
		///---------------------------수정 부분-----------------------------------------
		//	// (애니메이션을 포함한 메쉬일 경우) 본 정보와 애니메이션 정보 저장
		if (m_nBoneCount)
		{
			fin.getline(ch, 200);
			m_pBoneHierarchy = new UINT[m_nBoneCount];
			m_pxm4x4BoneOffsets = new XMFLOAT4X4[m_nBoneCount];
			m_pxm4x4SQTTransform = new XMMATRIX[m_nBoneCount];
			m_pxm4x4FinalBone = new XMFLOAT4X4[m_nBoneCount];

			// 부모 뼈대를 가리키는 BoneHierarchy를 저장
			for (int i = 0; i < m_nBoneCount; i++)
			{
				fin.getline(ch, 200);
				m_pBoneHierarchy[i] = stoi(ch);
			}
			// 뼈대 자체의 오프셋 행렬을 저장
			fin.getline(ch, 200);
			for (int i = 0; i < m_nBoneCount; i++)
			{
				fin.getline(ch, 200);
				sToken = strtok_s(ch, " ", &temp);		m_pxm4x4BoneOffsets[i]._11 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._12 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._13 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._14 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._21 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._22 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._23 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._24 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._31 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._32 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._33 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._34 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._41 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._42 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._43 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pxm4x4BoneOffsets[i]._44 = stof(sToken);
			}


			// 여기에서부터 애니메이션을 담는다.
			m_ppBoneAnimationData = new BoneAnimationData*[m_nAnimationClip];

			BoneAnimationData *pBoneAnimationData;

			for (int k = 0; k < m_nAnimationClip; k++)
			{
				pBoneAnimationData = new BoneAnimationData[m_nBoneCount];

				//[ANIMATION_CLIPS]
				fin.getline(ch, 200);

				for (int i = 0; i < m_nBoneCount; i++)
				{
					fin.getline(ch, 200);
					sToken = strtok_s(ch, " ", &temp);
					sToken = strtok_s(NULL, " ", &temp);
					pBoneAnimationData[i].m_nFrameCount = stoi(sToken);

					pBoneAnimationData[i].m_pd3dxvTranslate = new XMFLOAT3[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pd3dxvScale = new XMFLOAT3[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pd3dxvQuaternion = new XMFLOAT4[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pfAniTime = new float[pBoneAnimationData[i].m_nFrameCount];

					for (int j = 0; j < pBoneAnimationData[i].m_nFrameCount; j++)
					{
						fin.getline(ch, 200);
						sToken = strtok_s(ch, " ", &temp);
						pBoneAnimationData[i].m_pfAniTime[j] = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvTranslate[j].x = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvTranslate[j].y = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvTranslate[j].z = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvScale[j].x = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvScale[j].y = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvScale[j].z = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].x = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].y = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].z = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].w = stof(sToken);
					}
				}
				m_ppBoneAnimationData[k] = pBoneAnimationData;
			}
		}

		////-------------------------------------------------------------------------------------
		CreateBuffer(pd3dDevice, pd3dCommandList);


		CMesh *pMesh = NULL;
		CMaterial *pMaterial = NULL;
		if (pnIndices > 0)
		{
			if (nVertices > 0) pMesh = new CFBXMesh(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, pxmf3Normals, pxmf3TextureCoords0, nIndices, pnIndices, pxmf4BoneWeights, pxmf4BoneIndices);
			CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
			pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pszDDSFileName, 0);
			//-------Material 만들기
			pMaterial = new CMaterial();
			
			if (m_fbxType != OTHER)
			{
				pMaterial->SetAlbedo(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
			}

			pMaterial->SetTexture(pTexture);
	

			UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

			ID3D12Resource *pd3dcbResource = /*CGameObject::*/CreateShaderVariables(pd3dDevice, pd3dCommandList);

			CAnimatedFBXShader *pShader = new CAnimatedFBXShader();
			pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
			pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
			pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
			pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
			pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);   //쉐이더리소스뷰도 바꿔줘야한다네!

			SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

			pMaterial->SetShader(pShader);
			//--------------------------------------------------------------------------------------------------------
		}
		else
		{
			if (nVertices > 0) pMesh = new CMesh(pd3dDevice, pd3dCommandList, nVertices, pxmf3Positions, nIndices, pnIndices);
		}

		if (pMesh)
			SetMesh(0, pMesh);
		else
			ResizeMeshes(0);

		m_fFBXModelScale = scale;

		if (pMaterial) SetMaterial(pMaterial);
		if (pxmf3Positions) delete[] pxmf3Positions;
		if (pxmf3Normals) delete[] pxmf3Normals;
		if (pxmf3TextureCoords0) delete[] pxmf3TextureCoords0;
		if (pnIndices) delete[] pnIndices;
		if (pxmf4BoneIndices) delete[] pxmf4BoneIndices;
		if (pxmf4BoneWeights) delete[] pxmf4BoneWeights;
	}
	if (m_nBoneCount)
		m_nFBXMaxFrameNum = m_ppBoneAnimationData[0][0].m_nFrameCount - 1;
}
//-----------------------------------------------------------------------------------------------------------------------

void CAnimatedModelObject::CreateBuffer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	///뼈_상수버퍼
	UINT ncbElementBytes = ((sizeof(CB_SKINNED_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbBones = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbBones->Map(0, NULL, (void **)&m_pcbMappedBoneInfo);
}

void CAnimatedModelObject::ReleaseBuffer()
{
	if (m_pd3dcbBones)
	{
		m_pd3dcbBones->Unmap(0, NULL);
		m_pd3dcbBones->Release();
	}
	if (m_pMaterial) m_pMaterial->ReleaseShaderVariables();
}

void CAnimatedModelObject::UpdateBuffer(ID3D12GraphicsCommandList *pd3dCommandList)
{
	CB_SKINNED_INFO *m_pcbMappedBones;
	m_pd3dcbBones->Map(0, NULL, (void **)&m_pcbMappedBones);

	for (int i = 0; i < m_nBoneCount; i++)
	{
		XMMATRIX temp = XMLoadFloat4x4(&m_pxm4x4FinalBone[i]);
		m_pcbMappedBones->m_xmtxBone[i] = XMMatrixTranspose(temp);
	}
	m_pd3dcbBones->Unmap(0, NULL);


	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbBones->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(15, d3dGpuVirtualAddress);
}

void CAnimatedModelObject::UpdateBoneTransform(ID3D12Device *pd3dDevice)
{
	for (int i = 0; i < m_nBoneCount; i++)
	{
		MakeBoneMatrix(m_nFBXNowFrameNum, m_nFBXAnimationNum, i, *(m_pxm4x4SQTTransform + i));
	}

	// 마지막으로 본의 기본 오프셋행렬을 곱해주어 최종 행렬을 만들어준다.
	for (int i = 0; i < m_nBoneCount; i++)
	{
		XMFLOAT4X4 offset = m_pxm4x4BoneOffsets[i];
		XMFLOAT4X4 toRoot;// = m_pxm4x4SQTTransform[i];

		XMStoreFloat4x4(&toRoot, m_pxm4x4SQTTransform[i]);

		m_pxm4x4FinalBone[i] = Matrix4x4::Multiply(offset, toRoot);
	}
	//이 함수 끝나면 UpdateBoneShaderVariables 실행되도록 호출한다.
}

void CAnimatedModelObject::MakeBoneMatrix(int nNowframe, int nAnimationNum, int nBoneNum, XMMATRIX& BoneMatrix)
{
	if (m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_nFrameCount != 0)
	{
		XMFLOAT3 fScale = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvScale[nNowframe];
		XMVECTOR vScale = XMLoadFloat3(&fScale);

		XMFLOAT3 d3dxvTranslate = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvTranslate[nNowframe];
		XMVECTOR d3dxvTrans = XMLoadFloat3(&d3dxvTranslate);

		XMFLOAT4 d3dxvQuaternion = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvQuaternion[nNowframe];
		XMVECTOR d3dxvQuater = XMLoadFloat4(&d3dxvQuaternion);

		XMVECTOR d3dxvZero = { 0.0f, 0.0f, 0.0f };
		//D3DXMatrixAffineTransformation(&BoneMatrix, fScale, &d3dxvZero, &d3dxvQuaternion, &d3dxvTranslate);
		BoneMatrix = XMMatrixAffineTransformation(vScale, d3dxvZero, d3dxvQuater, d3dxvTrans);
	}
	else // 해당 본에 애니메이션 프레임이 아예 없을 경우 단위행렬을 리턴하자.
	{
		BoneMatrix = XMMatrixIdentity();
	}
}

// 다음 애니메이션을 위한 프레임으로 넘긴다.
// 추가적인 애니메이션 관리를 위해 마지막 프레임일 경우 true를 리턴한다.
bool CAnimatedModelObject::FrameAdvance(float fTimeElapsed)
{
	m_fFBXAnimationTime += fTimeElapsed;

	float animtime = ANIFRAMETIME;
	//"마법사" "스킬" 애니메이션일 때만 3배해서 쓸 것
	if (m_nFBXAnimationNum == 5 && m_fbxType == ASSASSIN)
		animtime = ANIFRAMETIME * 2;

	if (m_fFBXAnimationTime > animtime)	// 0.0333333f초가 지나면 1프레임 올리자.
	{
		if (m_nFBXNowFrameNum < m_nFBXMaxFrameNum - 1)
		{
			m_nFBXNowFrameNum++;
			m_fFBXAnimationTime = 0.0f;
			return false;
		}
		else
		{
			m_nFBXNowFrameNum = 0;
			m_fFBXAnimationTime = 0.0f;
			return true;
		}
	}
	else
		return false;
}

void CAnimatedModelObject::UpdatePosition(float fTimeElapsed)
{
	m_fRotationTime += fTimeElapsed;

	if (m_fRotationTime > ANIFRAMETIME)
	{
		m_fRotationTime = 0.f;
		
		m_fTheta += 0.1f;

		SetXPosition(520 + 20.f * cos(m_fTheta));
		SetZPosition(300 + 20.f * sin(m_fTheta));
	}
}


void CAnimatedModelObject::SetAnimation(int nFBXAnimation)
{
	m_nFBXAnimationNum = nFBXAnimation;
	m_nFBXNowFrameNum = 0;
	m_nFBXMaxFrameNum = m_ppBoneAnimationData[nFBXAnimation][0].m_nFrameCount;
	m_fFBXAnimationTime = 0.0f;
}

bool CAnimatedModelObject::IsAnimationFinish()
{
	///현재 프레임이 마지막일 때
	if(m_nFBXNowFrameNum == m_nFBXMaxFrameNum -1) 
		return true;
	else 
		return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//

CPlaneObject::CPlaneObject(int nMeshes)
{
}

CPlaneObject::~CPlaneObject()
{
}

void CPlaneObject::Animate(float fTimeElapsed)
{
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char type ,LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(0)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;

	CHeightMapGridMesh *pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart, zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z*cxBlocks), pHeightMapGridMesh);
		}
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pTerrainTexture = new CTexture(2, RESOURCE_TEXTURE2D, 0);

	if (type == 'f') //filed
	{
		pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/Field/Base_Texture.dds", 0);
		pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/Field/Detail_Texture_7.dds", 1);
	}
	else if(type=='t') //town
	{
		pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/Base_tile.dds", 0);
		pTerrainTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Terrain/stone.dds", 1);
	}


	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CTerrainShader *pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
 	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pTerrainShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 2);
	pTerrainShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pTerrainShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTerrainTexture, 6, true);

	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->SetTexture(pTerrainTexture);

	SetMaterial(pTerrainMaterial);

	SetCbvGPUDescriptorHandle(pTerrainShader->GetGPUCbvDescriptorStartHandle());

	SetShader(pTerrainShader);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CSkyBox::CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature) : CGameObject(6)
{
	CTexturedRectMesh *pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	SetMesh(0, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	SetMesh(1, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, 0.0f);
	SetMesh(2, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 0.0f, 20.0f, 20.0f, +10.0f, 0.0f, 0.0f);
	SetMesh(3, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, +10.0f, 0.0f);
	SetMesh(4, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 20.0f, 0.0f, 20.0f, 0.0f, -10.0f, 0.0f);
	SetMesh(5, pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture *pSkyBoxTexture = new CTexture(6, RESOURCE_TEXTURE2D, 0);

	//pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Sunset/Front_Tex.dds", 0);
	//pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Sunset/Back_Tex.dds", 1);
	//pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Sunset/Left_Tex.dds", 2);
	//pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Sunset/Right_Tex.dds", 3);
	//pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Sunset/Up_Tex.dds", 4);
	//pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Sunset/Down_Tex.dds", 5);

	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Daylight Box_Pieces/Daylight Box_Front.dds", 0);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Daylight Box_Pieces/Daylight Box_Back.dds", 1);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Daylight Box_Pieces/Daylight Box_Left.dds", 2);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Daylight Box_Pieces/Daylight Box_Right.dds", 3);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Daylight Box_Pieces/Daylight Box_Top.dds", 4);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/SkyBox/Daylight Box_Pieces/Daylight Box_Bottom.dds", 5);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	CSkyBoxShader *pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pSkyBoxShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 8);
	pSkyBoxShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pSkyBoxShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pSkyBoxTexture, 8, false);

	CMaterial *pSkyBoxMaterial = new CMaterial();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);

	SetMaterial(pSkyBoxMaterial);

	SetCbvGPUDescriptorHandle(pSkyBoxShader->GetGPUCbvDescriptorStartHandle());

	SetShader(pSkyBoxShader);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetWorldPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	OnPrepareRender();

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			UpdateShaderVariables(pd3dCommandList);
		}
	}

	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_pMaterial)
			{
				if (m_pMaterial->m_pTexture) m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, i);
			}
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
CBillboardObject::CBillboardObject(int nMeshes)
{
}

CBillboardObject::~CBillboardObject()
{
}

void CBillboardObject::Animate(float fTimeElapsed, CCamera * pCamera)
{
	XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	SetLookAt(xmf3CameraPosition);
}

void CBillboardObject::SetLookAt(XMFLOAT3 & xmf3Target)
{
	XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f);
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::Subtract(xmf3Target, xmf3Position);
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);

	m_xmf4x4World._11 = xmf3Right.x; m_xmf4x4World._12 = xmf3Right.y; m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x; m_xmf4x4World._22 = xmf3Up.y; m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;	m_xmf4x4World._32 = xmf3Look.y;	m_xmf4x4World._33 = xmf3Look.z;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
CFBXModel::CFBXModel(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, char * pszFileName, wchar_t* pszDDSFileName)
{
	LoadGeometryFromSpiderFBX(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pszFileName, pszDDSFileName);
}


CFBXModel::~CFBXModel()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
CFBXBumpedModel::CFBXBumpedModel(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, char * pszFileName, wchar_t* pszTextureFileName, wchar_t* pszNormalmapFileName)
{
	LoadNormalMappedGeometryFromFBX(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pszFileName, pszTextureFileName, pszNormalmapFileName);
}

CFBXBumpedModel::~CFBXBumpedModel()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTreeModel::CTreeModel(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, char * pszFileName )
{
	LoadMeshFromFBX(pd3dDevice, pd3dCommandList, pszFileName);
}


CTreeModel::~CTreeModel()
{
}