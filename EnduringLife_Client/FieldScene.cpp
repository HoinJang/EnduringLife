//-----------------------------------------------------------------------------
// File: FieldScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "FieldScene.h"

CFieldScene::CFieldScene()
{
}

CFieldScene::~CFieldScene()
{
}

void CFieldScene::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 100.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(130.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	//손전등
	m_pLights->m_pLights[1].m_bEnable = false;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 100.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.35f, 0.35f, 0.35f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.58f, 0.58f, 0.58f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	
	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.36f, 0.17f, 0.90, 0.3f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(150.0f, 160.0f, 900.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, -10.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 8.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	m_pLights->m_pLights[4].m_bEnable = true;
	m_pLights->m_pLights[4].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[4].m_xmf3Direction = XMFLOAT3(1.0f, 1.0f, 1.0f);

	//광역 스킬 조명 !!
	m_pLights->m_pLights[5].m_bEnable = false;
	m_pLights->m_pLights[5].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[5].m_fRange = 30.0f;
	m_pLights->m_pLights[5].m_xmf4Ambient = XMFLOAT4(3.f, 3.f, 3.f, 3.0f);
	m_pLights->m_pLights[5].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.3f);
	m_pLights->m_pLights[5].m_xmf4Specular = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	m_pLights->m_pLights[5].m_xmf3Position = XMFLOAT3(0.f, 0.f, 0.f);
	m_pLights->m_pLights[5].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	m_pLights->m_pLights[5].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[5].m_fFalloff = 8.0f;
	m_pLights->m_pLights[5].m_fPhi = (float)cos(XMConvertToRadians(30.0f));
	m_pLights->m_pLights[5].m_fTheta = (float)cos(XMConvertToRadians(10.0f));

	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));

	m_pMaterials->m_pReflections[0] = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_nMaterials = 1;

	for (int i = 0; i < m_nObjects; i++) UpdateMaterial(m_ppObjects[i]);
}

void CFieldScene::UpdateMaterial(CGameObject *pObject)
{
	CMaterial *pMaterial = pObject->m_pMaterial;
	if (pMaterial)
	{
		if (m_nMaterials < MAX_MATERIALS)
		{
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Ambient = pMaterial->m_xmf4Albedo;
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Diffuse = pMaterial->m_xmf4Albedo;
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f);
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			pMaterial->m_nReflection = m_nMaterials++;
		}
		else
			pMaterial->m_nReflection = 0;
	}

	if (pObject->m_pSibling) UpdateMaterial(pObject->m_pSibling);
	if (pObject->m_pChild) UpdateMaterial(pObject->m_pChild);
}

void CFieldScene::SetMaterial(int nIndex, MATERIAL *pMaterial)
{
	m_pMaterials->m_pReflections[nIndex] = *pMaterial;
}


void CFieldScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	XMFLOAT3 xmf3Scale(4.0f, 1.0f, 4.0f);

	XMFLOAT4 xmf4Color(0.5f, 0.5f, 0.5f, 0.0f);
#ifdef _WITH_TERRAIN_PARTITION
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 'f', _T("Assets/Image/Terrain/default_heightmap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);
#else
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 'f', _T("Assets/Image/Terrain/Field/field_heightmap_low_low.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);
#endif
	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];


	m_ppObjects[0] = new CFBXBumpedModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Portal/Platform.data", L"Assets/Model/Portal/Stones_Color.dds", L"Assets/Model/Portal/Stones_Normal.dds");
	float height = m_pTerrain->GetHeight(150, 900);
	m_ppObjects[0]->SetPosition(XMFLOAT3(150, height + 0.5f, 900));
	m_ppObjects[0]->SetScale(2.0f, 2.0f, 2.0f);


	BuildLightsAndMaterials();

	m_nShaders = 41;  //몬스터  hp바
	m_ppShaders = new CShader*[m_nShaders];

	CMonHPShader *S0 = new CMonHPShader();
	S0->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S0->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[0] = S0;

	CMonHPShader *S1 = new CMonHPShader();
	S1->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S1->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[1] = S1;

	CMonHPShader *S2 = new CMonHPShader();
	S2->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S2->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[2] = S2;

	CMonHPShader *S3 = new CMonHPShader();
	S3->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S3->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[3] = S3;

	CMonHPShader *S4 = new CMonHPShader();
	S4->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S4->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[4] = S4;

	CMonHPShader *S5 = new CMonHPShader();
	S5->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S5->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[5] = S5;

	CMonHPShader *S6 = new CMonHPShader();
	S6->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S6->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[6] = S6;

	CMonHPShader *S7 = new CMonHPShader();
	S7->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S7->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[7] = S7;

	CMonHPShader *S8 = new CMonHPShader();
	S8->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S8->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[8] = S8;

	CMonHPShader *S9 = new CMonHPShader();
	S9->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S9->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[9] = S9;

	CMonHPShader *S10 = new CMonHPShader();
	S10->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S10->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[10] = S10;

	CMonHPShader *S11 = new CMonHPShader();
	S11->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S11->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[11] = S11;

	CMonHPShader *S12 = new CMonHPShader();
	S12->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S12->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[12] = S12;

	CMonHPShader *S13 = new CMonHPShader();
	S13->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S13->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[13] = S13;

	CMonHPShader *S14 = new CMonHPShader();
	S14->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S14->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[14] = S14;

	CMonHPShader *S15 = new CMonHPShader();
	S15->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S15->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[15] = S15;

	CMonHPShader *S16 = new CMonHPShader();
	S16->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S16->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[16] = S16;

	CMonHPShader *S17 = new CMonHPShader();
	S17->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S17->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[17] = S17;

	CMonHPShader *S18 = new CMonHPShader();
	S18->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S18->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[18] = S18;

	CMonHPShader *S19 = new CMonHPShader();
	S19->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S19->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[19] = S19;

	CMonHPShader *S20 = new CMonHPShader();
	S20->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S20->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[20] = S20;

	CMonHPShader *S21 = new CMonHPShader();
	S21->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S21->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[21] = S21;

	CMonHPShader *S22 = new CMonHPShader();
	S22->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S22->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[22] = S22;

	CMonHPShader *S23 = new CMonHPShader();
	S23->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S23->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[23] = S23;

	CMonHPShader *S24 = new CMonHPShader();
	S24->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S24->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[24] = S24;

	CMonHPShader *S25 = new CMonHPShader();
	S25->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S25->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[25] = S25;

	CMonHPShader *S26 = new CMonHPShader();
	S26->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S26->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[26] = S26;

	CMonHPShader *S27 = new CMonHPShader();
	S27->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S27->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[27] = S27;

	CMonHPShader *S28 = new CMonHPShader();
	S28->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S28->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[28] = S28;

	CMonHPShader *S29 = new CMonHPShader();
	S29->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S29->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[29] = S29;

	CMonHPShader *S30 = new CMonHPShader();
	S30->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S30->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[30] = S30;

	CMonHPShader *S31 = new CMonHPShader();
	S31->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S31->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[31] = S31;

	CMonHPShader *S32 = new CMonHPShader();
	S32->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S32->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[32] = S32;

	CMonHPShader *S33 = new CMonHPShader();
	S33->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S33->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[33] = S33;

	CMonHPShader *S34 = new CMonHPShader();
	S34->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S34->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[34] = S34;

	CMonHPShader *S35 = new CMonHPShader();
	S35->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S35->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[35] = S35;

	CMonHPShader *S36 = new CMonHPShader();
	S36->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S36->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[36] = S36;

	CMonHPShader *S37 = new CMonHPShader();
	S37->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S37->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[37] = S37;

	CMonHPShader *S38 = new CMonHPShader();
	S38->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S38->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[38] = S38;

	CMonHPShader *S39 = new CMonHPShader();
	S39->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S39->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[39] = S39;

	CMonHPShader *S40 = new CMonHPShader();
	S40->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	S40->BuildObjects(pd3dDevice, pd3dCommandList, 10, 0.5);
	m_ppShaders[40] = S40;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList);
}

void CFieldScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

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

	if (m_ppObjects)
	{
		for (int i = 0; i < m_nObjects; i++) delete m_ppObjects[i];
		delete[] m_ppObjects;
	}

	ReleaseShaderVariables();

	if (m_pLights) delete m_pLights;
	if (m_pMaterials) delete m_pMaterials;

	if (m_pTerrain) delete m_pTerrain;
	if (m_pSkyBox) delete m_pSkyBox;
}

void CFieldScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();

	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->ReleaseUploadBuffers();

	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
}

void CFieldScene::UpdateMonHPIndex(int idx, bool visible, XMFLOAT3 pos, float hp)
{
	m_ppShaders[idx]->UpdateMonHPIndex(idx, visible, pos, hp);
}


ID3D12RootSignature *CFieldScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[12];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 2; //b2: Game Objects
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 0; //t0: Texture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 6; //t6: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 7; //t7: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 8; //t8: gtxtSkyBoxTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 1; //t1: TreeTexture
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 2; //t2: WaterBaseTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 3; //t3: WaterDetailTexture
	pd3dDescriptorRanges[7].RegisterSpace = 0;
	pd3dDescriptorRanges[7].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[8].NumDescriptors = 1;
	pd3dDescriptorRanges[8].BaseShaderRegister = 9;  //t9: FountainTexture
	pd3dDescriptorRanges[8].RegisterSpace = 0;
	pd3dDescriptorRanges[8].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[9].NumDescriptors = 1;
	pd3dDescriptorRanges[9].BaseShaderRegister = 10; //t10: NormalMapTexture
	pd3dDescriptorRanges[9].RegisterSpace = 0;
	pd3dDescriptorRanges[9].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	////////////////////////////////////////////////////////////////////////////////
	pd3dDescriptorRanges[10].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[10].NumDescriptors = 1;
	pd3dDescriptorRanges[10].BaseShaderRegister = 5;  //b5: Txt  Objects
	pd3dDescriptorRanges[10].RegisterSpace = 0;
	pd3dDescriptorRanges[10].OffsetInDescriptorsFromTableStart = 0;
	////////////////////////////////////////////////////////////////////////////////
	pd3dDescriptorRanges[11].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[11].NumDescriptors = 4;
	pd3dDescriptorRanges[11].BaseShaderRegister = 11; //t11: Texture[4]
	pd3dDescriptorRanges[11].RegisterSpace = 0;
	pd3dDescriptorRanges[11].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	////////////////////////////////////////////////////////////////////////////////

	D3D12_ROOT_PARAMETER pd3dRootParameters[21];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //b0:  Player
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //b1: Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0]; //Game Objects
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //b3: Materials
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 4; //b4: Lights
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1]; //t0: Texture
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2]; //t6: gtxtTerrainBaseTexture
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3]; //t7: gtxtTerrainDetailTexture
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[4]; //t8: skybox
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[5]; //t1: TreeTexture
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[6]; //t2: WaterTexture
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[11].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[11].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[7]; //t3: WaterTexture
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	////////////////////////////////////////////////////////////////////////////////
	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[12].Descriptor.ShaderRegister = 5; //b5: Txt  Objects
	pd3dRootParameters[12].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	////////////////////////////////////////////////////////////////////////////////
	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[8];//t9: FountainTexture
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[14].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[14].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[9]; //t10: NormalMapTexture
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	////////////////////////////////////////////////////////////////////////////////
	pd3dRootParameters[15].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[15].Descriptor.ShaderRegister = 6; //b6: Bone Buffer
	pd3dRootParameters[15].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[15].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	////////////////////////////////////////////////////////////////////////////////
	pd3dRootParameters[16].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[16].Descriptor.ShaderRegister = 7; //b7:HP Info
	pd3dRootParameters[16].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[16].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	////////////////////////////////////////////////////////////////////////////////
	pd3dRootParameters[17].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[17].Descriptor.ShaderRegister = 8; //b8:UI Pos
	pd3dRootParameters[17].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[17].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	////////////////////////////////////////////////////////////////////////////////
	pd3dRootParameters[18].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[18].Descriptor.ShaderRegister = 9; //b9:Max Pos ---Effect
	pd3dRootParameters[18].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[18].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//
	//----스킬 이펙트 텍스쳐 
	pd3dRootParameters[19].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[19].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[19].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[11]; //t11: Texture
	pd3dRootParameters[19].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//
	//---안개 버퍼---
	pd3dRootParameters[20].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[20].Descriptor.ShaderRegister = 10; //b10:cbFog
	pd3dRootParameters[20].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[20].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[3];
	//::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//////CLAMP모드로 하나 더 생성!
	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//////MIRROR모드로 하나 더 생성!
	pd3dSamplerDescs[2].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].MipLODBias = 0;
	pd3dSamplerDescs[2].MaxAnisotropy = 1;
	pd3dSamplerDescs[2].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	pd3dSamplerDescs[2].MinLOD = 0;
	pd3dSamplerDescs[2].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[2].ShaderRegister = 2;
	pd3dSamplerDescs[2].RegisterSpace = 0;
	pd3dSamplerDescs[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);;
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CFieldScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbMaterialBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbMaterials->Map(0, NULL, (void **)&m_pcbMappedMaterials);
}

void CFieldScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));
}

void CFieldScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
	if (m_pd3dcbMaterials)
	{
		m_pd3dcbMaterials->Unmap(0, NULL);
		m_pd3dcbMaterials->Release();
	}
}

bool CFieldScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CFieldScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CFieldScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CFieldScene::AnimateObjects(float fTimeElapsed, CCamera *pCamera, ID3D12Device *m_pd3dDevice)
{
	for (int i = 0; i < m_nShaders; ++i)
		m_ppShaders[i]->AnimateObjects(fTimeElapsed, pCamera);

	//
	//조명-손전등
	if (m_pLights)
	{
		m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void CFieldScene::StartSkillLight()
{
	m_pLights->m_pLights[5].m_bEnable = true;
	m_pLights->m_pLights[5].m_xmf3Position = XMFLOAT3(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y + 20.f, m_pPlayer->GetPosition().z);	
}

void CFieldScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList); //조명

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialsGpuVirtualAddress); //Materials

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nObjects; ++i) m_ppObjects[i]->UpdateTransform(NULL);
	for (int i = 0; i < m_nObjects; ++i) m_ppObjects[i]->Render(pd3dCommandList);

	for (int i = 0; i < m_nShaders; ++i) m_ppShaders[i]->Render(pd3dCommandList);

}

