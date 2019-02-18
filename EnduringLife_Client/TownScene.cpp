//-----------------------------------------------------------------------------
// File: TownScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "TownScene.h"

CTownScene::CTownScene()
{
}

CTownScene::~CTownScene()
{
}

void CTownScene::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = false;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 100.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(130.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

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

	//포탈
	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 45.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.36f, 0.17f, 0.90f, 0.3f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(512.0f, 50.0f, 60.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 4.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(30.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(10.0f));
	
	m_pLights->m_pLights[4].m_bEnable = true;
	m_pLights->m_pLights[4].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.45f, 0.45f, 0.45f, 0.7f);
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

void CTownScene::UpdateMaterial(CGameObject *pObject)
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

void CTownScene::SetMaterial(int nIndex, MATERIAL *pMaterial)
{
	m_pMaterials->m_pReflections[nIndex] = *pMaterial;
}


void CTownScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	XMFLOAT3 xmf3Scale(4.0f, 1.0f, 4.0f);

	XMFLOAT4 xmf4Color(0.5f, 0.5f, 0.5f, 0.0f);
#ifdef _WITH_TERRAIN_PARTITION
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature,'t', _T("Assets/Image/Terrain/default_heightmap.raw"), 257, 257, 17, 17, xmf3Scale, xmf4Color);
#else
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 't',_T("Assets/Image/Terrain/dddterrain.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);
#endif
	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_nObjects = 21;
	m_ppObjects = new CGameObject*[m_nObjects];
	//나비
	m_ppObjects[0] = new CAnimatedModelObject(OTHER, pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Animal/butterfly.data", L"Assets/Model/Animal/Butterfly.dds", 1.0f);
	float height = m_pTerrain->GetHeight(500, 300);
	m_ppObjects[0]->SetPosition(XMFLOAT3(500, height + 30, 300));
	m_ppObjects[0]->SetScale(0.1f, 0.1f, 0.1f);
	
	m_ppObjects[1] = new CFBXBumpedModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Fountain/fountain_LOD1.data", L"Assets/Model/Fountain/Fountain-Diffuse.dds", L"Assets/Model/Fountain/Fountain-Normal.dds");
	height = m_pTerrain->GetHeight(512, 600);
	m_ppObjects[1]->SetPosition(XMFLOAT3(512, height, 600));
	m_ppObjects[1]->SetScale(3.f, 3.f, 3.f);

	m_ppObjects[2] = new CFBXBumpedModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Arcane/h_arcane.data", L"Assets/Model/Arcane/h_arcane_Albedo_Red.dds", L"Assets/Model/Arcane/h_arcane_N.dds");
	height = m_pTerrain->GetHeight(870, 300);
	m_ppObjects[2]->SetPosition(XMFLOAT3(870, height, 300));
	m_ppObjects[2]->Rotate(0.f, 180.f, 0.f);
	m_ppObjects[2]->SetScale(0.4f, 0.4f, 0.4f);

	m_ppObjects[3] = new CFBXBumpedModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Townhall/h_town_hall.data", L"Assets/Model/Townhall/h_town_hall_Albedo_Gray.dds", L"Assets/Model/Townhall/h_town_hall_N.dds");
	height = m_pTerrain->GetHeight(930, 500);
	m_ppObjects[3]->SetPosition(XMFLOAT3(930, height, 500));
	m_ppObjects[3]->Rotate(0.f, 180.f, 0.f);
	m_ppObjects[3]->SetScale(0.6f, 0.6f, 0.6f);

	m_ppObjects[4] = new CFBXBumpedModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/House/h_farm.data", L"Assets/Model/House/h_farm_Albedo_Red.dds", L"Assets/Model/House/h_farm_N.dds");
	height = m_pTerrain->GetHeight(190, 400);
	m_ppObjects[4]->SetPosition(XMFLOAT3(190, height, 400));
	m_ppObjects[4]->SetScale(0.6f, 0.6f, 0.6f);

	m_ppObjects[5] = new CFBXBumpedModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Barracks/h_barracks.data", L"Assets/Model/Barracks/h_barracks_Albedo_Green.dds", L"Assets/Model/Barracks/h_barracks_N.dds");
	height = m_pTerrain->GetHeight(512, 950);
	m_ppObjects[5]->SetPosition(XMFLOAT3(512, height, 950));
	m_ppObjects[5]->Rotate(0.f, 90.f, 0.f);
	m_ppObjects[5]->SetScale(1.3f, 1.3f, 1.3f);

	m_ppObjects[6] = new CFBXBumpedModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/castle1.data", L"Assets/Model/Castle Towers UV 2.dds", L"Assets/Model/Castle Towers UV 3.dds");
	height = m_pTerrain->GetHeight(512, 900);
	m_ppObjects[6]->SetPosition(XMFLOAT3(512, height + 30.f, 900));
	m_ppObjects[6]->SetScale(30.f, 30.f, 20.f);

	m_ppObjects[7] = new CFBXBumpedModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Portal/Platform.data", L"Assets/Model/Portal/Stones_Color.dds", L"Assets/Model/Portal/Stones_Normal.dds");
	height = m_pTerrain->GetHeight(512, 60);
	m_ppObjects[7]->SetPosition(XMFLOAT3(512, height + 0.5f, 60));
	m_ppObjects[7]->SetScale(2.0f, 2.0f, 2.0f);

	m_ppObjects[8] = new CFBXBumpedModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Blacksmith/h_blacksmith.data", L"Assets/Model/Blacksmith/h_blacksmith_Albedo_Teal.dds", L"Assets/Model/Blacksmith/h_blacksmith_N.dds");
	height = m_pTerrain->GetHeight(200, 500);
	m_ppObjects[8]->SetPosition(XMFLOAT3(200, height, 500));	
	m_ppObjects[8]->SetScale(0.4f, 0.4f, 0.4f);

	m_ppObjects[9] = new CFBXBumpedModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Construction/h_construction.data", L"Assets/Model/Construction/h_construction_Albedo_Teal.dds", L"Assets/Model/Construction/h_construction_N.dds");
	height = m_pTerrain->GetHeight(200, 300);
	m_ppObjects[9]->SetPosition(XMFLOAT3(200, height, 300));
	m_ppObjects[9]->SetScale(0.2f, 0.2f, 0.2f);
	//NPC-Castle Guard
	m_ppObjects[10] = new CAnimatedModelObject(OTHER, pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/NPC/1.Gaurd.data", L"Assets/Model/NPC/1.Guard_diffuse.dds", 1.f);
	height = m_pTerrain->GetHeight(450, 720);
	m_ppObjects[10]->SetPosition(XMFLOAT3(450, height, 720));
	m_ppObjects[10]->SetScale(0.07f, 0.07f, 0.07f);
	m_ppObjects[10]->Rotate(-90.f, 0, 0);
	//NPC-Peasant man
	m_ppObjects[11] = new CAnimatedModelObject(OTHER,pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/NPC/0.Peasant_OffensiveIdle.data", L"Assets/Model/NPC/0.Peasant_Man_diffuse.dds", 1.0f);
	height = m_pTerrain->GetHeight(300, 400);
	m_ppObjects[11]->SetPosition(XMFLOAT3(300, height, 400));
	m_ppObjects[11]->SetScale(0.07f, 0.07f, 0.07f);
	m_ppObjects[11]->Rotate(-90.f, -90.f, 0);
	//fence1
	m_ppObjects[12] = new CFBXModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Construction/fence_double.data", L"Assets/Model/Construction/fence.dds");
	height = m_pTerrain->GetHeight(800, 683);
	m_ppObjects[12]->SetPosition(XMFLOAT3(800, height, 683));
	m_ppObjects[12]->SetScale(0.1f, 0.12f, 0.1f);
	m_ppObjects[12]->Rotate(-90.f, -90.f, 0);
	//fence2
	m_ppObjects[13] = new CFBXModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Construction/fence_double.data", L"Assets/Model/Construction/fence.dds");
	height = m_pTerrain->GetHeight(800, 510);
	m_ppObjects[13]->SetPosition(XMFLOAT3(800, height, 523));
	m_ppObjects[13]->SetScale(0.1f, 0.12f, 0.1f);
	m_ppObjects[13]->Rotate(-90.f, -90.f, 0);
	//fence3
	m_ppObjects[14] = new CFBXModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Construction/fence_double.data", L"Assets/Model/Construction/fence.dds");
	height = m_pTerrain->GetHeight(800, 790);
	m_ppObjects[14]->SetPosition(XMFLOAT3(800, height, 790));
	m_ppObjects[14]->SetScale(0.1f, 0.12f, 0.1f);
	m_ppObjects[14]->Rotate(-90.f, -90.f, 0);

	//fence4
	m_ppObjects[15] = new CFBXModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Construction/fence_double.data", L"Assets/Model/Construction/fence.dds");
	height = m_pTerrain->GetHeight(250, 410);
	m_ppObjects[15]->SetPosition(XMFLOAT3(250, height, 410));
	m_ppObjects[15]->SetScale(0.1f, 0.12f, 0.1f);
	m_ppObjects[15]->Rotate(-90.f, 90.f, 0);
	//fence5
	m_ppObjects[16] = new CFBXModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Construction/fence_double.data", L"Assets/Model/Construction/fence.dds");
	height = m_pTerrain->GetHeight(250, 520);
	m_ppObjects[16]->SetPosition(XMFLOAT3(250, height, 520));
	m_ppObjects[16]->SetScale(0.1f, 0.12f, 0.1f);
	m_ppObjects[16]->Rotate(-90.f, 90.f, 0);
	//fence6
	m_ppObjects[17] = new CFBXModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Construction/fence_double.data", L"Assets/Model/Construction/fence.dds");
	height = m_pTerrain->GetHeight(250, 630);
	m_ppObjects[17]->SetPosition(XMFLOAT3(250, height, 630));
	m_ppObjects[17]->SetScale(0.1f, 0.12f, 0.1f);
	m_ppObjects[17]->Rotate(-90.f, 90.f, 0);

	//NPC-Peasant man
	m_ppObjects[18] = new CAnimatedModelObject(OTHER, pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/NPC/0.Peasant_OffensiveIdle.data", L"Assets/Model/NPC/0.Peasant_Man_diffuse.dds", 1.0f);
	height = m_pTerrain->GetHeight(750, 400);
	m_ppObjects[18]->SetPosition(XMFLOAT3(750, height, 400));
	m_ppObjects[18]->SetScale(0.07f, 0.07f, 0.07f);
	m_ppObjects[18]->Rotate(-90.f, 90.f, 0);

	//Bridge1
	m_ppObjects[19] = new CFBXModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Bridge/Bridge.data", L"Assets/Model/Bridge/Bridge_DEF.dds");
	height = m_pTerrain->GetHeight(512, 173);
	m_ppObjects[19]->SetPosition(XMFLOAT3(512, height + 52, 130));
	m_ppObjects[19]->SetScale(0.7f, 0.4f, 0.17f);

	//Bridge2
	m_ppObjects[20] = new CFBXModel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Assets/Model/Bridge/Bridge.data", L"Assets/Model/Bridge/Bridge_DEF.dds");
	m_ppObjects[20]->SetPosition(XMFLOAT3(512, height + 52, 255));
	m_ppObjects[20]->SetScale(0.7f, 0.4f, 0.17f); 

	BuildLightsAndMaterials();

	m_nShaders = 3;
	m_ppShaders= new CShader*[m_nShaders]; 

	CBillboardShader *pFlowerObjectShader_1 = new CBillboardShader();
	CTexture *pFlowerTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pFlowerTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Grasses/Grass27.dds", 0);   //꽃밭
	pFlowerObjectShader_1->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pFlowerObjectShader_1->BuildObjects(pd3dDevice, pd3dCommandList, TownFlower, 100, 400, 20, 15, m_pTerrain, pFlowerTexture);  
	m_ppShaders[1] = pFlowerObjectShader_1;

	CBillboardShader *pGrassObjectShader = new CBillboardShader();
	CTexture *pF2Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pF2Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Trees/Tree10.dds", 0);				//풀
	pGrassObjectShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pGrassObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, TownGrass, 1000, 400, 30, 30, m_pTerrain, pF2Texture);
	m_ppShaders[2] = pGrassObjectShader;

	CTreeShader *pTreeShader = new CTreeShader();
	pTreeShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Model/Trees/TreeBrunches.dds", 0);		//나무
	pTreeShader->BuildObjects(pd3dDevice, pd3dCommandList, TownGrass, 280, 730, 1, 1, m_pTerrain, pTexture);
	m_ppShaders[0] = pTreeShader;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList);
}

void CTownScene::ReleaseObjects()
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

	dynamic_cast<CAnimatedModelObject*>(m_ppObjects[0])->ReleaseBuffer();

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

void CTownScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();

	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->ReleaseUploadBuffers();

	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
}

ID3D12RootSignature *CTownScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
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
	pd3dDescriptorRanges[8].BaseShaderRegister =9;  //t9: FountainTexture
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
	pd3dDescriptorRanges[11].BaseShaderRegister = 11; //t11: Texture
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

void CTownScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbMaterialBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbMaterials->Map(0, NULL, (void **)&m_pcbMappedMaterials);
}

void CTownScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));
}

void CTownScene::ReleaseShaderVariables()
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

bool CTownScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CTownScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CTownScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CTownScene::AnimateObjects(float fTimeElapsed, CCamera *pCamera, ID3D12Device *m_pd3dDevice)
{
	for (int i = 0; i < m_nShaders; i++) 
		m_ppShaders[i]->AnimateObjects(fTimeElapsed, pCamera);
	
	   (m_ppObjects[0])->FrameAdvance(fTimeElapsed);
	   (m_ppObjects[0])->UpdatePosition(fTimeElapsed);  //나비 움직임
	   (m_ppObjects[0])->UpdateBoneTransform(m_pd3dDevice);

	   (m_ppObjects[10])->FrameAdvance(fTimeElapsed);
	   (m_ppObjects[10])->UpdateBoneTransform(m_pd3dDevice);

	   (m_ppObjects[11])->FrameAdvance(fTimeElapsed);
	   (m_ppObjects[11])->UpdateBoneTransform(m_pd3dDevice);

	   (m_ppObjects[18])->FrameAdvance(fTimeElapsed);
	   (m_ppObjects[18])->UpdateBoneTransform(m_pd3dDevice);
	 ///손전등
	if (m_pLights)
	{
		m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void CTownScene::StartSkillLight()
{
	m_pLights->m_pLights[5].m_bEnable = true;
	m_pLights->m_pLights[5].m_xmf3Position = XMFLOAT3(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().y + 20.f, m_pPlayer->GetPosition().z);
}

void CTownScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList); //빛

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialsGpuVirtualAddress); //Materials

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	//-----------------------------------------------------------------------------
	(m_ppObjects[0])->UpdateBuffer(pd3dCommandList);
	(m_ppObjects[10])->UpdateBuffer(pd3dCommandList);
	(m_ppObjects[11])->UpdateBuffer(pd3dCommandList);
	(m_ppObjects[18])->UpdateBuffer(pd3dCommandList);
	//-----------------------------------------------------------------------------

	for (int i = 0; i < m_nObjects; ++i) m_ppObjects[i]->UpdateTransform(NULL);
	for (int i = 0; i < m_nObjects; ++i) m_ppObjects[i]->Render(pd3dCommandList);	
	for (int i = 0; i < m_nShaders; ++i) m_ppShaders[i]->Render(pd3dCommandList);

}

