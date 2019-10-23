#include "stdafx.h"
#include "Level.h"
#include "GameObject.h"
#include "PhysxProxy.h"
#include "ContentManager.h"
#include "ColliderComponent.h"
#include "PhysxManager.h"
#include "Components.h"
#include "RigidBodyComponent.h"
#include "../../Prefabs/PickUps/WumpaFruit.h"
#include "../../Prefabs/Character/CrashBandicoot.h"
#include "../../Prefabs/Player/PlayerInventory.h"
#include "../../Prefabs/Player/PlayerHUD.h"
#include "../../Prefabs/PickUps/Crystal.h"
#include "../../Prefabs/Crates/CrateNormal.h"
#include "../../Prefabs/Crates/CrateLife.h"
#include "../../Prefabs/Crates/CrateJump.h"
#include "../../Prefabs/Crates/CrateCheckPoint.h"
#include "../../Prefabs/Crates/CrateTNT.h"
#include "../../Prefabs/Crates/CrateNitro.h"
#include "../../Prefabs/Character/FollowCam.h"
#include "../../Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "SkyBoxPrefab.h"


using namespace physx;
using namespace std;
using namespace DirectX;

Level::Level() :
	GameScene(L"Level")
{}


void Level::Initialize()
{
	//ESSENTIALS
//**********
	
	const auto gameContext = GetGameContext();
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	auto physx = PhysxManager::GetInstance()->GetPhysics();
	gameContext.pShadowMapper->SetLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	m_ShadowFor = { 0.740129888f, -0.597205281f, 0.309117377f };
	m_ShadowPos = { -120,150,20.0f };
	auto pDefaultMaterial = physx->createMaterial(0.5f, 0.5f, 0.5f);
	m_IsSongPlaying = false;
	m_CamIndex = 0;
	m_TotalBoxes = 0;
	m_CamRotator = 0.0f;
	m_StartedGame = false;
	////Character
	m_pCharacter = new CrashBandicoot();
	AddChild(m_pCharacter);
	m_pCharacter->SetRespawnTarget({ 10.0f,10.0f,10.0f });

	m_pFollowCam = new FollowCam{m_pCharacter};
	AddChild(m_pFollowCam);

	PlayerInventory::GetInstance()->Initialize();

	LevelLayout();
	LevelCollision();

	SetItemsPositions();
	LevelExtras();
	PlayerHUD* pCrashHUD = new PlayerHUD();
	AddChild(pCrashHUD);

	PlayerInventory::GetInstance()->SetGameOver(false);


	m_pCharacter->GetComponent<ControllerComponent>()->Move({ 0.0f, 10.0f, 10.0f });


	//Floor
	auto pGround = new GameObject();
	pGround->AddComponent(new RigidBodyComponent(true));
	shared_ptr<physx::PxGeometry> pFloorGeometry(new physx::PxPlaneGeometry());
	pGround->AddComponent(new ColliderComponent(pFloorGeometry, *pDefaultMaterial,
		PxTransform(PxQuat(XM_PIDIV2, physx::PxVec3(0.0f, 0.0f, 1.0f)))));
	pGround->GetTransform()->Translate(0.0f, -50.0f, 0.0f);
	AddChild(pGround);
	
	m_pSoundManager = SoundManager::GetInstance();

	m_pSoundManager->GetSystem()->createStream("./Resources/Game/Sound/Music/sound03.mp3", FMOD_LOOP_NORMAL | FMOD_2D, 0, &m_pSongLevel);
	m_pSoundManager->GetSystem()->createStream("./Resources/Game/Sound/Music/sound02.mp3", FMOD_LOOP_NORMAL | FMOD_2D, 0, &m_pSongGameOver);
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/falling.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundFalling);
}

void Level::Update()
{
	const auto gameContext = GetGameContext();
	ChangeCam();
	if (!m_StartedGame)
	{
		m_pSoundManager->GetSystem()->playSound(m_pSongLevel, 0, false, 0);
		m_StartedGame = true;
	}
	if (PlayerInventory::GetInstance()->IsGameOver())
	{
		if (!m_IsSongPlaying)
		{
			m_pSongLevel->release();
			m_pSoundManager->GetSystem()->playSound(m_pSongGameOver, 0, false, 0);
			m_IsSongPlaying = true;
		}
		
	}

	XMFLOAT3 offsetPos{};
	offsetPos.x = m_pCharacter->GetTransform()->GetPosition().x + m_ShadowPos.x;
	offsetPos.y = m_pCharacter->GetTransform()->GetPosition().y + m_ShadowPos.y;
	offsetPos.z = m_pCharacter->GetTransform()->GetPosition().z + m_ShadowPos.z;

	XMFLOAT3 offsetFor{};
	offsetFor.x = m_pCharacter->GetTransform()->GetPosition().x - offsetPos.x;
	offsetFor.y = m_pCharacter->GetTransform()->GetPosition().y - offsetPos.y;
	offsetFor.z = m_pCharacter->GetTransform()->GetPosition().z - offsetPos.z;

	PlayerInventory::GetInstance()->SetMaxBoxes(m_TotalBoxes);
	gameContext.pShadowMapper->SetLight(offsetPos, offsetFor);
}

void Level::Draw()
{}

void Level::LevelLayout()
{
	const auto gameContext = GetGameContext();
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial *pDefaultMaterial = physX->createMaterial(0.5f, 0.0f, 0.5f);
	AddChild(new SkyboxPrefab{});


	//BEST PARTICLE EVER
	auto crashParticle = new GameObject();
	m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Game/Texture/crash.png", 60);
	m_pParticleEmitter->SetVelocity(DirectX::XMFLOAT3(-6.0f, 6.0f, 0.f));
	m_pParticleEmitter->SetMinSize(3.0f);
	m_pParticleEmitter->SetMaxSize(5.0f);
	m_pParticleEmitter->SetMinEnergy(1.0f);
	m_pParticleEmitter->SetMaxEnergy(2.0f);
	m_pParticleEmitter->SetMinSizeGrow(5.0f);
	m_pParticleEmitter->SetMaxSizeGrow(10.5f);
	m_pParticleEmitter->SetMinEmitterRange(1.0f);
	m_pParticleEmitter->SetMaxEmitterRange(2.0f);
	m_pParticleEmitter->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.0f));
	crashParticle->AddComponent(m_pParticleEmitter);
	crashParticle->GetTransform()->Translate(-1300.0f, 105.0f, 1135.0f);
	AddChild(crashParticle);

	//BEGIN FLOOR -- SAND -- BEACH
	auto pBeginLevel00 = new GameObject();

	auto pMatBeginLevel00 = new DiffuseMaterial_Shadow();
	pMatBeginLevel00->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_00.jpeg");
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel00, 1000);
	pMatBeginLevel00->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	auto pObjBeginLevel00 = new ModelComponent(L"Resources/Game/Level/CB_Level01_00.ovm");
	pObjBeginLevel00->SetMaterial(1000);

	pBeginLevel00->AddComponent(pObjBeginLevel00);

	auto pRigidBeginLevel00 = new RigidBodyComponent(true);
	auto pConvexBeginLevel00 = ContentManager::Load<PxTriangleMesh>(L"Resources/Game/Level/CB_Level01_00.ovpt");
	std::shared_ptr<PxGeometry> pGeomBeginLevel00(new PxTriangleMeshGeometry(pConvexBeginLevel00, PxMeshScale(0.05f)));

	pBeginLevel00->AddComponent(pRigidBeginLevel00);
	pBeginLevel00->AddComponent(new ColliderComponent(pGeomBeginLevel00, *pDefaultMaterial));
	pBeginLevel00->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pBeginLevel00->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel00);


	//BEGIN FLOOR -- FLANK -- SAND - ROCK FORM
	auto pBeginLevel01 = new GameObject();
	auto pMatBeginLevel01 = new DiffuseMaterial_Shadow();
	pMatBeginLevel01->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_01.jpeg");
	pMatBeginLevel01->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel01, 1001);

	auto pObjBeginLevel01 = new ModelComponent(L"Resources/Game/Level/CB_Level01_01.ovm");
	pObjBeginLevel01->SetMaterial(1001);
	pBeginLevel01->AddComponent(pObjBeginLevel01);
	pObjBeginLevel01->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel01->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel01);

	//ROCKS -- MOUNTAINS -- SIDE
	auto pBeginLevel02 = new GameObject();
	auto pMatBeginLevel02 = new DiffuseMaterial_Shadow();
	pMatBeginLevel02->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_02.jpeg");
	pMatBeginLevel02->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel02, 1002);

	auto pObjBeginLevel02 = new ModelComponent(L"Resources/Game/Level/CB_Level01_02.ovm");
	pObjBeginLevel02->SetMaterial(1002);
	pBeginLevel02->AddComponent(pObjBeginLevel02);
	pObjBeginLevel02->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel02->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel02);

	//GRASS 
	auto pBeginLevel03 = new GameObject();
	auto pMatBeginLevel03 = new DiffuseMaterial_Shadow();
	pMatBeginLevel03->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_03.jpeg");
	pMatBeginLevel03->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel03, 1003);

	auto pObjBeginLevel03 = new ModelComponent(L"Resources/Game/Level/CB_Level01_03.ovm");
	pObjBeginLevel03->SetMaterial(1003);
	pBeginLevel03->AddComponent(pObjBeginLevel03);
	pObjBeginLevel03->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel03->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel03);

	//FLOOR - SIDES  
	auto pBeginLevel04 = new GameObject();
	auto pMatBeginLevel04 = new DiffuseMaterial_Shadow();
	pMatBeginLevel04->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_04.jpg");
	pMatBeginLevel04->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel04, 1004);

	auto pObjBeginLevel04 = new ModelComponent(L"Resources/Game/Level/CB_Level01_04.ovm");
	pObjBeginLevel04->SetMaterial(1004);
	pBeginLevel04->AddComponent(pObjBeginLevel04);
	pObjBeginLevel04->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel04->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel04);

	//FLOOR - MAIN  
	auto pBeginLevel05 = new GameObject();
	auto pMatBeginLevel05 = new DiffuseMaterial_Shadow();
	pMatBeginLevel05->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_05.jpg");
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel05, 1005);
	pMatBeginLevel05->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	auto pObjBeginLevel05 = new ModelComponent(L"Resources/Game/Level/CB_Level01_05.ovm");
	pObjBeginLevel05->SetMaterial(1005);
	pBeginLevel05->AddComponent(pObjBeginLevel05);
	pObjBeginLevel05->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel05->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel05);

	//SHIP - PART01  
	auto pBeginLevel06 = new GameObject();
	auto pMatBeginLevel06 = new DiffuseMaterial_Shadow();
	pMatBeginLevel06->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_06.jpeg");
	pMatBeginLevel06->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel06, 1006);

	auto pObjBeginLevel06 = new ModelComponent(L"Resources/Game/Level/CB_Level01_06.ovm");
	pObjBeginLevel06->SetMaterial(1006);
	pBeginLevel06->AddComponent(pObjBeginLevel06);
	pObjBeginLevel06->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel06->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel06);

	//SHIP - PART02
	auto pBeginLevel07 = new GameObject();
	auto pMatBeginLevel07 = new DiffuseMaterial_Shadow();
	pMatBeginLevel07->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_07.jpeg");
	pMatBeginLevel07->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel07, 1007);

	auto pObjBeginLevel07 = new ModelComponent(L"Resources/Game/Level/CB_Level01_07.ovm");
	pObjBeginLevel07->SetMaterial(1007);
	pBeginLevel07->AddComponent(pObjBeginLevel07);
	pObjBeginLevel07->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel07->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel07);

	//SHIP - PART03
	auto pBeginLevel08 = new GameObject();
	auto pMatBeginLevel08 = new DiffuseMaterial_Shadow();
	pMatBeginLevel08->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_08.jpeg");
	pMatBeginLevel08->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel08, 1008);

	auto pObjBeginLevel08 = new ModelComponent(L"Resources/Game/Level/CB_Level01_08.ovm");
	pObjBeginLevel08->SetMaterial(1008);
	pBeginLevel08->AddComponent(pObjBeginLevel08);
	pObjBeginLevel08->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel08->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel08);

	//SHIP - PART04
	auto pBeginLevel09 = new GameObject();
	auto pMatBeginLevel09 = new DiffuseMaterial_Shadow();
	pMatBeginLevel09->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_09.jpeg");
	pMatBeginLevel09->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel09, 1009);

	auto pObjBeginLevel09 = new ModelComponent(L"Resources/Game/Level/CB_Level01_09.ovm");
	pObjBeginLevel09->SetMaterial(1009);
	pBeginLevel09->AddComponent(pObjBeginLevel09);
	pObjBeginLevel09->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel09->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel09);

	//BIG TREE - PART 01
	auto pBeginLevel10 = new GameObject();
	auto pMatBeginLevel10 = new DiffuseMaterial_Shadow();
	pMatBeginLevel10->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_10.jpeg");
	pMatBeginLevel10->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel10, 1010);

	auto pObjBeginLevel10= new ModelComponent(L"Resources/Game/Level/CB_Level01_10.ovm");
	pObjBeginLevel10->SetMaterial(1010);
	pBeginLevel10->AddComponent(pObjBeginLevel10);
	pObjBeginLevel10->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel10->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel10);

	//BIG TREE - PART 02
	auto pBeginLevel11 = new GameObject();
	auto pMatBeginLevel11 = new DiffuseMaterial_Shadow();
	pMatBeginLevel11->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_11.jpeg");
	pMatBeginLevel11->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel11, 1011);

	auto pObjBeginLevel11 = new ModelComponent(L"Resources/Game/Level/CB_Level01_11.ovm");
	pObjBeginLevel11->SetMaterial(1011);
	pBeginLevel11->AddComponent(pObjBeginLevel11);
	pObjBeginLevel11->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel11->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel11);

	//BIG TREE - PART 03
	auto pBeginLevel12 = new GameObject();
	auto pMatBeginLevel12 = new DiffuseMaterial_Shadow();
	pMatBeginLevel12->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_12.png");
	pMatBeginLevel12->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel12, 1012);

	auto pObjBeginLevel12 = new ModelComponent(L"Resources/Game/Level/CB_Level01_12.ovm");
	pObjBeginLevel12->SetMaterial(1012);
	pBeginLevel12->AddComponent(pObjBeginLevel12);
	pObjBeginLevel12->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel12->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel12);

	//Medium TREE - PART 01
	auto pBeginLevel13 = new GameObject();
	auto pObjBeginLevel13 = new ModelComponent(L"Resources/Game/Level/CB_Level01_13.ovm");
	pObjBeginLevel13->SetMaterial(1010);
	pBeginLevel13->AddComponent(pObjBeginLevel13);
	pObjBeginLevel13->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel13->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel13);

	//Medium TREE - PART 02
	auto pBeginLevel14 = new GameObject();
	auto pObjBeginLevel14 = new ModelComponent(L"Resources/Game/Level/CB_Level01_14.ovm");
	pObjBeginLevel14->SetMaterial(1011);
	pBeginLevel14->AddComponent(pObjBeginLevel14);
	pObjBeginLevel14->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel14->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel14);

	//Medium TREE - PART 03
	auto pBeginLevel15 = new GameObject();
	auto pObjBeginLevel15 = new ModelComponent(L"Resources/Game/Level/CB_Level01_15.ovm");
	pObjBeginLevel15->SetMaterial(1012);
	pBeginLevel15->AddComponent(pObjBeginLevel15);
	pObjBeginLevel15->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel15->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel15);

	//Small TREE - PART 01
	auto pBeginLevel16 = new GameObject();
	auto pObjBeginLevel16 = new ModelComponent(L"Resources/Game/Level/CB_Level01_16.ovm");
	pObjBeginLevel16->SetMaterial(1011);
	pBeginLevel16->AddComponent(pObjBeginLevel16);
	pObjBeginLevel16->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel16->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel16);

	//Small TREE - PART 02
	auto pBeginLevel17 = new GameObject();
	auto pMatBeginLevel17 = new DiffuseMaterial_Shadow();
	pMatBeginLevel17->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_17.png");
	pMatBeginLevel17->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel17, 1017);

	auto pObjBeginLevel17 = new ModelComponent(L"Resources/Game/Level/CB_Level01_17.ovm");
	pObjBeginLevel17->SetMaterial(1017);
	pBeginLevel17->AddComponent(pObjBeginLevel17);
	pObjBeginLevel17->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel17->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel17);

	//Pit - PART 01
	auto pBeginLevel18 = new GameObject();
	auto pMatBeginLevel18 = new DiffuseMaterial_Shadow();
	pMatBeginLevel18->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_18.jpg");
	pMatBeginLevel18->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel18, 1018);

	auto pObjBeginLevel18 = new ModelComponent(L"Resources/Game/Level/CB_Level01_18.ovm");
	pObjBeginLevel18->SetMaterial(1018);
	pBeginLevel18->AddComponent(pObjBeginLevel18);
	pObjBeginLevel18->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel18->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel18);

	//Pit - PART 02
	auto pBeginLevel19 = new GameObject();
	auto pMatBeginLevel19 = new DiffuseMaterial_Shadow();
	pMatBeginLevel19->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_19.jpeg");
	pMatBeginLevel19->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel19, 1019);

	auto pObjBeginLevel19 = new ModelComponent(L"Resources/Game/Level/CB_Level01_19.ovm");
	pObjBeginLevel19->SetMaterial(1019);
	pBeginLevel19->AddComponent(pObjBeginLevel19);
	pObjBeginLevel19->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel19->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel19);

	//BIG ROCKS 01
	auto pBeginLevel20 = new GameObject();
	auto pMatBeginLevel20 = new DiffuseMaterial_Shadow();
	pMatBeginLevel20->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_20.jpeg");
	pMatBeginLevel20->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel20, 1020);

	auto pObjBeginLevel20 = new ModelComponent(L"Resources/Game/Level/CB_Level01_20.ovm");
	pObjBeginLevel20->SetMaterial(1020);
	pBeginLevel20->AddComponent(pObjBeginLevel20);
	pObjBeginLevel20->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel20->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel20);

	//BIG ROCKS 02
	auto pBeginLevel21 = new GameObject();
	auto pMatBeginLevel21 = new DiffuseMaterial_Shadow();
	pMatBeginLevel21->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_21.jpg");
	pMatBeginLevel21->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel21, 1021);

	auto pObjBeginLevel21 = new ModelComponent(L"Resources/Game/Level/CB_Level01_21.ovm");
	pObjBeginLevel21->SetMaterial(1021);
	pBeginLevel21->AddComponent(pObjBeginLevel21);
	pObjBeginLevel21->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel21->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel21);

	//PILLAR TYPE 01 - PART 01
	auto pBeginLevel22 = new GameObject();
	auto pMatBeginLevel22 = new DiffuseMaterial_Shadow();
	pMatBeginLevel22->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_22.jpeg");
	pMatBeginLevel22->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel22, 1022);

	auto pObjBeginLevel22 = new ModelComponent(L"Resources/Game/Level/CB_Level01_22.ovm");
	pObjBeginLevel22->SetMaterial(1022);
	pBeginLevel22->AddComponent(pObjBeginLevel22);
	pObjBeginLevel22->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel22->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel22);

	//PILLAR TYPE 01 - PART 02
	auto pBeginLevel23 = new GameObject();
	auto pMatBeginLevel23 = new DiffuseMaterial_Shadow();
	pMatBeginLevel23->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_23.jpeg");
	pMatBeginLevel23->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel23, 1023);

	auto pObjBeginLevel23 = new ModelComponent(L"Resources/Game/Level/CB_Level01_23.ovm");
	pObjBeginLevel23->SetMaterial(1023);
	pBeginLevel23->AddComponent(pObjBeginLevel23);
	pObjBeginLevel23->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel23->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel23);

	//PILLAR TYPE 01 - PART 03
	auto pBeginLevel24 = new GameObject();
	auto pMatBeginLevel24 = new DiffuseMaterial_Shadow();
	pMatBeginLevel24->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_24.jpeg");
	pMatBeginLevel24->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel24, 1024);

	auto pObjBeginLevel24 = new ModelComponent(L"Resources/Game/Level/CB_Level01_24.ovm");
	pObjBeginLevel24->SetMaterial(1024);
	pBeginLevel24->AddComponent(pObjBeginLevel24);
	pObjBeginLevel24->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel24->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel24);

	// GATES
	auto pBeginLevel25 = new GameObject();
	auto pMatBeginLevel25 = new DiffuseMaterial_Shadow();
	pMatBeginLevel25->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_25.jpeg");
	pMatBeginLevel25->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel25, 1025);

	auto pObjBeginLevel25 = new ModelComponent(L"Resources/Game/Level/CB_Level01_25.ovm");
	pObjBeginLevel25->SetMaterial(1025);
	pBeginLevel25->AddComponent(pObjBeginLevel25);
	pObjBeginLevel25->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel25->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel25);

	// FENCES
	auto pBeginLevel26 = new GameObject();
	auto pMatBeginLevel26 = new DiffuseMaterial_Shadow();
	pMatBeginLevel26->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_26.jpeg");
	pMatBeginLevel26->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel26, 1026);

	auto pObjBeginLevel26 = new ModelComponent(L"Resources/Game/Level/CB_Level01_26.ovm");
	pObjBeginLevel26->SetMaterial(1026);
	pBeginLevel26->AddComponent(pObjBeginLevel26);
	pObjBeginLevel26->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel26->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel26);

	// HOVER PLATFORM - PART 01
	auto pBeginLevel27 = new GameObject();
	auto pMatBeginLevel27 = new DiffuseMaterial_Shadow();
	pMatBeginLevel27->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_27.jpeg");
	pMatBeginLevel27->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel27, 1027);

	auto pObjBeginLevel27 = new ModelComponent(L"Resources/Game/Level/CB_Level01_27.ovm");
	pObjBeginLevel27->SetMaterial(1027);
	pBeginLevel27->AddComponent(pObjBeginLevel27);
	pObjBeginLevel27->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel27->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel27);

	// HOVER PLATFORM - PART 02
	auto pBeginLevel28 = new GameObject();
	auto pMatBeginLevel28 = new DiffuseMaterial_Shadow();
	pMatBeginLevel28->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_28.jpeg");
	pMatBeginLevel28->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel28, 1028);

	auto pObjBeginLevel28 = new ModelComponent(L"Resources/Game/Level/CB_Level01_28.ovm");
	pObjBeginLevel28->SetMaterial(1028);
	pBeginLevel28->AddComponent(pObjBeginLevel28);
	pObjBeginLevel28->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel28->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel28);

	// TOTEM 01 - PART 01
	auto pBeginLevel29 = new GameObject();
	auto pMatBeginLevel29 = new DiffuseMaterial_Shadow();
	pMatBeginLevel29->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_29.jpeg");
	pMatBeginLevel29->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel29, 1029);

	auto pObjBeginLevel29 = new ModelComponent(L"Resources/Game/Level/CB_Level01_29.ovm");
	pObjBeginLevel29->SetMaterial(1029);
	pBeginLevel29->AddComponent(pObjBeginLevel29);
	pObjBeginLevel29->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel29->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel29);

	// TOTEM 01 - PART 02
	auto pBeginLevel30 = new GameObject();
	auto pMatBeginLevel30 = new DiffuseMaterial_Shadow();
	pMatBeginLevel30->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_30.jpeg");
	pMatBeginLevel30->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel30, 1030);

	auto pObjBeginLevel30 = new ModelComponent(L"Resources/Game/Level/CB_Level01_30.ovm");
	pObjBeginLevel30->SetMaterial(1030);
	pBeginLevel30->AddComponent(pObjBeginLevel30);
	pObjBeginLevel30->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel30->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel30);


	// TOTEM 02 - PART 01
	auto pBeginLevel31 =new GameObject();
	auto pMatBeginLevel31 = new DiffuseMaterial_Shadow();
	pMatBeginLevel31->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_31.jpeg");
	pMatBeginLevel31->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel31, 1031);

	auto pObjBeginLevel31 = new ModelComponent(L"Resources/Game/Level/CB_Level01_31.ovm");
	pObjBeginLevel31->SetMaterial(1031);
	pBeginLevel31->AddComponent(pObjBeginLevel31);
	pObjBeginLevel31->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel31->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel31);

	// TOTEM 02 - PART 02
	auto pBeginLevel32 = new GameObject();
	auto pObjBeginLevel32 = new ModelComponent(L"Resources/Game/Level/CB_Level01_32.ovm");
	pObjBeginLevel32->SetMaterial(1030);
	pBeginLevel32->AddComponent(pObjBeginLevel32);
	pObjBeginLevel32->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel32->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel32);

	// TOTEM 03 - PART 01
	auto pBeginLevel33 = new GameObject();
	auto pMatBeginLevel33 = new DiffuseMaterial_Shadow();
	pMatBeginLevel33->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_33.jpeg");
	pMatBeginLevel33->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel33, 1033);

	auto pObjBeginLevel33 = new ModelComponent(L"Resources/Game/Level/CB_Level01_33.ovm");
	pObjBeginLevel33->SetMaterial(1033);
	pBeginLevel33->AddComponent(pObjBeginLevel33);
	pObjBeginLevel33->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel33->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel33);


	// TOTEM 03 - PART 02
	auto pBeginLevel34 = new GameObject();
	auto pMatBeginLevel34 = new DiffuseMaterial_Shadow();
	pMatBeginLevel34->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_34.jpeg");
	pMatBeginLevel34->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel34, 1034);

	auto pObjBeginLevel34 = new ModelComponent(L"Resources/Game/Level/CB_Level01_34.ovm");
	pObjBeginLevel34->SetMaterial(1034);
	pBeginLevel34->AddComponent(pObjBeginLevel34);
	pObjBeginLevel34->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel34->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel34);

	// TOTEM 03 - PART 03
	auto pBeginLevel35 = new GameObject();
	auto pMatBeginLevel35 = new DiffuseMaterial_Shadow();
	pMatBeginLevel35->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_35.jpeg");
	pMatBeginLevel35->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel35, 1035);

	auto pObjBeginLevel35 = new ModelComponent(L"Resources/Game/Level/CB_Level01_35.ovm");
	pObjBeginLevel35->SetMaterial(1035);
	pBeginLevel35->AddComponent(pObjBeginLevel35);
	pObjBeginLevel35->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel35->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel35);

	// TOTEM 03 - PART 04
	auto pBeginLevel36 = new GameObject();
	auto pObjBeginLevel36= new ModelComponent(L"Resources/Game/Level/CB_Level01_36.ovm");
	pObjBeginLevel36->SetMaterial(1030);
	pBeginLevel36->AddComponent(pObjBeginLevel36);
	pObjBeginLevel36->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel36->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel36);

	// WOODEN PLATFORM - PART 01
	auto pBeginLevel37 = new GameObject();
	auto pMatBeginLevel37 = new DiffuseMaterial_Shadow();
	pMatBeginLevel37->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_37.jpeg");
	pMatBeginLevel37->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel37, 1037);

	auto pObjBeginLevel37 = new ModelComponent(L"Resources/Game/Level/CB_Level01_37.ovm");
	pObjBeginLevel37->SetMaterial(1037);
	pBeginLevel37->AddComponent(pObjBeginLevel37);
	pObjBeginLevel37->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel37->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel37);

	// WOODEN PLATFORM - PART 02
	auto pBeginLevel38 = new GameObject();
	auto pMatBeginLevel38 = new DiffuseMaterial_Shadow();
	pMatBeginLevel38->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_38.jpeg");
	pMatBeginLevel38->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel38, 1038);

	auto pObjBeginLevel38 = new ModelComponent(L"Resources/Game/Level/CB_Level01_38.ovm");
	pObjBeginLevel38->SetMaterial(1038);
	pBeginLevel38->AddComponent(pObjBeginLevel38);
	pObjBeginLevel38->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel38->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel38);

	// WOODEN PLATFORM - PART 03
	auto pBeginLevel39 = new GameObject();
	auto pMatBeginLevel39 = new DiffuseMaterial_Shadow();
	pMatBeginLevel39->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_39.jpeg");
	pMatBeginLevel39->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel39, 1039);

	auto pObjBeginLevel39 = new ModelComponent(L"Resources/Game/Level/CB_Level01_39.ovm");
	pObjBeginLevel39->SetMaterial(1039);
	pBeginLevel39->AddComponent(pObjBeginLevel39);
	pObjBeginLevel39->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel39->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel39);

	// TOTEMS - 04 
	auto pBeginLevel40 = new GameObject();
	auto pMatBeginLevel40 = new DiffuseMaterial_Shadow();
	pMatBeginLevel40->SetDiffuseTexture(L"Resources/Game/Level/CB_Level01_40.jpeg");
	pMatBeginLevel40->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(pMatBeginLevel40, 1040);

	auto pObjBeginLevel40 = new ModelComponent(L"Resources/Game/Level/CB_Level01_40.ovm");
	pObjBeginLevel40->SetMaterial(1040);
	pBeginLevel40->AddComponent(pObjBeginLevel40);
	pObjBeginLevel40->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pObjBeginLevel40->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pBeginLevel40);
}

void Level::LevelExtras()
{
	//PITFALLS
	if (m_pPitfalls01->GetActive())
	{
		m_pPitfalls01->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
		{
			UNREFERENCED_PARAMETER(trigger);
			UNREFERENCED_PARAMETER(action);
			if (receive->GetTag() == L"CRASH")
			{
				PitFallDead();
			}
		});
	}

	if (m_pPitfalls02->GetActive())
	{
		m_pPitfalls02->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
		{
			UNREFERENCED_PARAMETER(trigger);
			UNREFERENCED_PARAMETER(action);
			if (receive->GetTag() == L"CRASH")
			{
				PitFallDead();
			}
		});
	}

	if (m_pPitfalls03->GetActive())
	{
		m_pPitfalls03->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
		{
			UNREFERENCED_PARAMETER(trigger);
			UNREFERENCED_PARAMETER(action);
			if (receive->GetTag() == L"CRASH")
			{
				PitFallDead();
			}
		});
	}

	if (m_pPitfalls04->GetActive())
	{
		m_pPitfalls04->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
		{
			UNREFERENCED_PARAMETER(trigger);
			UNREFERENCED_PARAMETER(action);
			if (receive->GetTag() == L"CRASH")
			{
				PitFallDead();
			}
		});
	}

	//CAMERA
	if (m_pTriggerCam01->GetActive())
	{
		m_pTriggerCam01->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
		{
			UNREFERENCED_PARAMETER(trigger);
			UNREFERENCED_PARAMETER(action);
			if (receive->GetTag() == L"CRASH")
			{
				m_CamIndex = 1;
	
			}
		});
	}

	if (m_pTriggerCam02->GetActive())
	{
		m_pTriggerCam02->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
		{
			UNREFERENCED_PARAMETER(trigger);
			UNREFERENCED_PARAMETER(action);
			if (receive->GetTag() == L"CRASH")
			{
				m_CamIndex = 0;
			}
		});
	}

	if (m_pTriggerCam03->GetActive())
	{
		m_pTriggerCam03->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
		{
			UNREFERENCED_PARAMETER(trigger);
			UNREFERENCED_PARAMETER(action);
			if (receive->GetTag() == L"CRASH")
			{
				m_CamIndex = 0;
			}
		});
	}

	if (m_pTriggerCam04->GetActive())
	{
		m_pTriggerCam04->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
		{
			UNREFERENCED_PARAMETER(trigger);
			UNREFERENCED_PARAMETER(action);
			if (receive->GetTag() == L"CRASH")
			{
				m_CamIndex = 1;
			}
		});
	}
	
		if (m_pEndingBox->GetActive())
		{
			m_pEndingBox->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
			{
				UNREFERENCED_PARAMETER(trigger);
				UNREFERENCED_PARAMETER(action);
				if (receive->GetTag() == L"CRASH")
				{
					PlayerInventory::GetInstance()->SetGameOver(true);
				}
			});
		}

}

void Level::LevelCollision()
{
	const auto gameContext = GetGameContext();
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial *pDefaultMaterial = physX->createMaterial(0.5f, 0.0f, 0.5f);

	//CAM CHANGES
	m_pTriggerCam01 = new GameObject();
	m_pTriggerCam01->AddComponent(new RigidBodyComponent(true));//12.0f, 40.0f, 970.0f
	std::shared_ptr<physx::PxGeometry> geomCam01 = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 50.0f,100.0f,2.0f });
	auto camTrigger01 = new ColliderComponent(geomCam01, *pDefaultMaterial, physx::PxTransform(10.0f, -25.0f, 1080.0f));
	camTrigger01->EnableTrigger(true);
	m_pTriggerCam01->AddComponent(camTrigger01);
	AddChild(m_pTriggerCam01);

	m_pTriggerCam02 = new GameObject();
	m_pTriggerCam02->AddComponent(new RigidBodyComponent(true));//12.0f, 40.0f, 970.0f
	std::shared_ptr<physx::PxGeometry> geomCam02 = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 50.0f,100.0f,2.0f });
	auto camTrigger02 = new ColliderComponent(geomCam02, *pDefaultMaterial, physx::PxTransform(10.0f, -25.0f, 1070.0f));
	camTrigger02->EnableTrigger(true);
	m_pTriggerCam02->AddComponent(camTrigger02);
	AddChild(m_pTriggerCam02);

	m_pTriggerCam03 = new GameObject();
	m_pTriggerCam03->AddComponent(new RigidBodyComponent(true));//12.0f, 40.0f, 970.0f
	std::shared_ptr<physx::PxGeometry> geomCam03 = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 20.0f,50.0f,1.0f });
	auto camTrigger03 = new ColliderComponent(geomCam03, *pDefaultMaterial, physx::PxTransform(-1032.0f, 100.0f, 1125.0f));
	camTrigger03->EnableTrigger(true);
	m_pTriggerCam03->AddComponent(camTrigger03);
	AddChild(m_pTriggerCam03);

	m_pTriggerCam04 = new GameObject();
	m_pTriggerCam04->AddComponent(new RigidBodyComponent(true));//12.0f, 40.0f, 970.0f
	std::shared_ptr<physx::PxGeometry> geomCam04 = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 20.0f,50.0f,1.0f });
	auto camTrigger04 = new ColliderComponent(geomCam04, *pDefaultMaterial, physx::PxTransform(-1032.0f, 100.0f, 1115.0f));
	camTrigger04->EnableTrigger(true);
	m_pTriggerCam04->AddComponent(camTrigger04);
	AddChild(m_pTriggerCam04);

	//WALL BLOCKADES
	auto pCollision00 = new GameObject();
	auto pRigidpCollision00 = new RigidBodyComponent(true);
	auto pConvexCollision00 = ContentManager::Load<PxTriangleMesh>(L"Resources/Game/Level/Collision/CB_Level01_Collision_00.ovpt");
	std::shared_ptr<PxGeometry> pGeomCollision00(new PxTriangleMeshGeometry(pConvexCollision00, PxMeshScale(0.05f)));

	pCollision00->AddComponent(pRigidpCollision00);
	pCollision00->AddComponent(new ColliderComponent(pGeomCollision00, *pDefaultMaterial));
	pCollision00->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pCollision00->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pCollision00);

	//FLOOR COLLISION
	auto pCollision01 = new GameObject();
	auto pRigidpCollision01= new RigidBodyComponent(true);
	auto pConvexCollision01 = ContentManager::Load<PxTriangleMesh>(L"Resources/Game/Level/Collision/CB_Level01_Collision_01.ovpt");
	std::shared_ptr<PxGeometry> pGeomCollision01(new PxTriangleMeshGeometry(pConvexCollision01, PxMeshScale(0.05f)));

	pCollision01->AddComponent(pRigidpCollision01);
	pCollision01->AddComponent(new ColliderComponent(pGeomCollision01, *pDefaultMaterial));
	pCollision01->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pCollision01->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pCollision01);

	//FLOOR COLLISION (between sand & land) 
	auto pCollision02 = new GameObject();
	auto pRigidpCollision02 = new RigidBodyComponent(true);
	auto pConvexCollision02 = ContentManager::Load<PxTriangleMesh>(L"Resources/Game/Level/Collision/CB_Level01_Collision_02.ovpt");
	std::shared_ptr<PxGeometry> pGeomCollision02(new PxTriangleMeshGeometry(pConvexCollision02, PxMeshScale(0.05f)));

	pCollision02->AddComponent(pRigidpCollision02);
	pCollision02->AddComponent(new ColliderComponent(pGeomCollision02, *pDefaultMaterial));
	pCollision02->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pCollision02->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pCollision02);

	//SIDE COLLISION (between sand & land) 
	auto pCollision03 = new GameObject();
	auto pRigidpCollision03 = new RigidBodyComponent(true);
	auto pConvexCollision03 = ContentManager::Load<PxTriangleMesh>(L"Resources/Game/Level/Collision/CB_Level01_Collision_03.ovpt");
	std::shared_ptr<PxGeometry> pGeomCollision03(new PxTriangleMeshGeometry(pConvexCollision03, PxMeshScale(0.05f)));

	pCollision03->AddComponent(pRigidpCollision03);
	pCollision03->AddComponent(new ColliderComponent(pGeomCollision03, *pDefaultMaterial));
	pCollision03->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pCollision03->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pCollision03);

	//PIT FALL DEATH COLLISION
	m_pPitfalls01 = new GameObject();
	m_pPitfalls01->AddComponent(new RigidBodyComponent(true));
	std::shared_ptr<physx::PxGeometry> geomPitfall01 = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 50.0f,50.0f,50.0f });
	auto pitfallTrigger01 = new ColliderComponent(geomPitfall01, *pDefaultMaterial, physx::PxTransform(-20.0f, -90.0f, 360.0f));
	pitfallTrigger01->EnableTrigger(true);
	m_pPitfalls01->AddComponent(pitfallTrigger01);
	AddChild(m_pPitfalls01);

	m_pPitfalls02 = new GameObject();
	m_pPitfalls02->AddComponent(new RigidBodyComponent(true));
	std::shared_ptr<physx::PxGeometry> geomPitfall02 = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 80.0f,50.0f,300.0f });
	auto pitfallTrigger02 = new ColliderComponent(geomPitfall02, *pDefaultMaterial, physx::PxTransform(0.0f, -70.0, 1000.0f));
	pitfallTrigger02->EnableTrigger(true);
	m_pPitfalls02->AddComponent(pitfallTrigger02);
	AddChild(m_pPitfalls02);

	m_pPitfalls03 = new GameObject();
	m_pPitfalls03->AddComponent(new RigidBodyComponent(true));
	std::shared_ptr<physx::PxGeometry> geomPitfall03 = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 100.0f,50.0f,300.0f });
	auto pitfallTrigger03 = new ColliderComponent(geomPitfall03, *pDefaultMaterial, physx::PxTransform(-660.0f, -20.0f, 1000.0f));
	pitfallTrigger03->EnableTrigger(true);
	m_pPitfalls03->AddComponent(pitfallTrigger03);
	AddChild(m_pPitfalls03);

	m_pPitfalls04 = new GameObject();//-1260.0f, 95.0f, 1135.0f
	m_pPitfalls04->AddComponent(new RigidBodyComponent(true));
	std::shared_ptr<physx::PxGeometry> geomPitfall04 = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 200.0f,30.0f,100.0f });
	auto pitfallTrigger04 = new ColliderComponent(geomPitfall04, *pDefaultMaterial, physx::PxTransform(-1200.0f, 40.0f, 1200.0f));
	pitfallTrigger04->EnableTrigger(true);
	m_pPitfalls04->AddComponent(pitfallTrigger04);
	AddChild(m_pPitfalls04);

	m_pEndingBox = new GameObject();//-1260.0f, 95.0f, 1135.0f
	m_pEndingBox->AddComponent(new RigidBodyComponent(true));
	std::shared_ptr<physx::PxGeometry> geomEnding = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 5.0f,10.0f,7.0f });
	auto endingTrigger = new ColliderComponent(geomEnding, *pDefaultMaterial, physx::PxTransform(-1300.0f, 105.0f, 1135.0f));
	endingTrigger->EnableTrigger(true);
	m_pEndingBox->AddComponent(endingTrigger);
	AddChild(m_pEndingBox);

	

	//PITS COLLISION - PART 01
	auto pCollision04 = new GameObject();
	auto pRigidpCollision04 = new RigidBodyComponent(true);
	auto pConvexCollision04 = ContentManager::Load<PxTriangleMesh>(L"Resources/Game/Level/Collision/CB_Level01_Collision_04.ovpt");
	std::shared_ptr<PxGeometry> pGeomCollision04(new PxTriangleMeshGeometry(pConvexCollision04, PxMeshScale(0.05f)));

	pCollision04->AddComponent(pRigidpCollision04);
	pCollision04->AddComponent(new ColliderComponent(pGeomCollision04, *pDefaultMaterial));
	pCollision04->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pCollision04->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pCollision04);

	//PITS COLLISION - PART 02
	auto pCollision05 = new GameObject();
	auto pRigidpCollision05 = new RigidBodyComponent(true);
	auto pConvexCollision05 = ContentManager::Load<PxTriangleMesh>(L"Resources/Game/Level/Collision/CB_Level01_Collision_05.ovpt");
	std::shared_ptr<PxGeometry> pGeomCollision05(new PxTriangleMeshGeometry(pConvexCollision05, PxMeshScale(0.05f)));

	pCollision05->AddComponent(pRigidpCollision05);
	pCollision05->AddComponent(new ColliderComponent(pGeomCollision05, *pDefaultMaterial));
	pCollision05->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pCollision05->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pCollision05);

	//PLATFORM 1 (STONE)
	auto pCollision06 = new GameObject();
	auto pRigidpCollision06 = new RigidBodyComponent(true);
	auto pConvexCollision06 = ContentManager::Load<PxTriangleMesh>(L"Resources/Game/Level/Collision/CB_Level01_Collision_06.ovpt");
	std::shared_ptr<PxGeometry> pGeomCollision06(new PxTriangleMeshGeometry(pConvexCollision06, PxMeshScale(0.05f)));

	pCollision06->AddComponent(pRigidpCollision06);
	pCollision06->AddComponent(new ColliderComponent(pGeomCollision06, *pDefaultMaterial));
	pCollision06->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pCollision06->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pCollision06);

	//RAMP
	auto pCollision07 = new GameObject();
	auto pRigidpCollision07 = new RigidBodyComponent(true);
	auto pConvexCollision07 = ContentManager::Load<PxTriangleMesh>(L"Resources/Game/Level/Collision/CB_Level01_Collision_07.ovpt");
	std::shared_ptr<PxGeometry> pGeomCollision07(new PxTriangleMeshGeometry(pConvexCollision07, PxMeshScale(0.05f)));

	pCollision07->AddComponent(pRigidpCollision07);
	pCollision07->AddComponent(new ColliderComponent(pGeomCollision07, *pDefaultMaterial));
	pCollision07->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pCollision07->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pCollision07);

	//WOOD PLATFORM
	auto pCollision08 = new GameObject();
	auto pRigidpCollision08= new RigidBodyComponent(true);
	auto pConvexCollision08 = ContentManager::Load<PxTriangleMesh>(L"Resources/Game/Level/Collision/CB_Level01_Collision_08.ovpt");
	std::shared_ptr<PxGeometry> pGeomCollision08(new PxTriangleMeshGeometry(pConvexCollision08, PxMeshScale(0.05f)));

	pCollision08->AddComponent(pRigidpCollision08);
	pCollision08->AddComponent(new ColliderComponent(pGeomCollision08, *pDefaultMaterial));
	pCollision08->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pCollision08->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	AddChild(pCollision08);
}

void Level::SetItemsPositions()
{
	//-- CRATES --/
#pragma region CheckpointCrates

	m_CratesCheckPointPos.push_back(XMFLOAT3{ 7.5f, 6.0f, 650.0f });
	m_CratesCheckPointPos.push_back(XMFLOAT3{ 7.5f, 6.0f, 950.0f });
	m_CratesCheckPointPos.push_back(XMFLOAT3{ 7.5f, 6.0f, 1070.0f }); 
	m_CratesCheckPointPos.push_back(XMFLOAT3{ -500.0f, 47.0f, 1033.0f });
	m_CratesCheckPointPos.push_back(XMFLOAT3{ -760.0f, 47.0f, 1033.0f });
	m_CratesCheckPointPos.push_back(XMFLOAT3{ -1040.0f, 104.0f, 1135.0f });
	for (size_t i = 0; i < m_CratesCheckPointPos.size(); i++)
	{
		auto crateCheckPoint = new CrateCheckPoint{ m_pCharacter,m_CratesCheckPointPos[i] };
		m_pCratesCheckPoint.push_back(crateCheckPoint);
		AddChild(m_pCratesCheckPoint[i]);
	}
	m_TotalBoxes += m_pCratesCheckPoint.size();

#pragma endregion 

#pragma region JumpCrates
	//-- Jump CRATES --//
	//begin pit
	m_CratesJumpPos.push_back(XMFLOAT3{ 0.0f, -35.0f, 335.0f });
	m_CratesJumpPos.push_back(XMFLOAT3{ -10.0f, -35.0f, 335.0f });
	m_CratesJumpPos.push_back(XMFLOAT3{ 10.0f, -35.0f, 335.0f });
	m_CratesJumpPos.push_back(XMFLOAT3{ 20.0f, -35.0f, 335.0f });

	//three pits
	m_CratesJumpPos.push_back(XMFLOAT3{ 7.5f, 6.0f, 750.0f });
	m_CratesJumpPos.push_back(XMFLOAT3{ 7.5f, 6.0f, 780.0f });
	m_CratesJumpPos.push_back(XMFLOAT3{ 7.5f, 6.0f, 825.0f });
	m_CratesJumpPos.push_back(XMFLOAT3{ 7.5f, 6.0f, 872.0f });


	for (size_t i = 0; i < m_CratesJumpPos.size(); i++)
	{
		auto crateJump = new CrateJump{ m_pCharacter,m_CratesJumpPos[i] };
		m_pCratesJump.push_back(crateJump);
		AddChild(m_pCratesJump[i]);
	}
	m_TotalBoxes += m_pCratesJump.size();
#pragma endregion 

#pragma region TNTCrates
	//-- TNT CRATES --//
	m_CratesTNTPos.push_back(XMFLOAT3{ -10, -35.0f, 290.0f });
	m_CratesTNTPos.push_back(XMFLOAT3{ 22.0f, -35.0f, 290.0f });
	m_CratesTNTPos.push_back(XMFLOAT3{ -10, -35.0f, 270.0f });
	m_CratesTNTPos.push_back(XMFLOAT3{ 22.0f, -35.0f, 270.0f });
	m_CratesTNTPos.push_back(XMFLOAT3{ -10, -35.0f, 250.0f });
	m_CratesTNTPos.push_back(XMFLOAT3{ 22.0f, -35.0f, 250.0f });


	m_CratesTNTPos.push_back(XMFLOAT3{ -10.5f, 6.0f, 1090.0f });
	m_CratesTNTPos.push_back(XMFLOAT3{ -30.5f, 6.0f, 1115.0f });
	m_CratesTNTPos.push_back(XMFLOAT3{ -45.5f, 6.0f, 1105.0f });
	m_CratesTNTPos.push_back(XMFLOAT3{ -60.5f, 6.0f, 1120.0f });
	m_CratesTNTPos.push_back(XMFLOAT3{ -80.5f, 6.0f, 1105.0f });

	m_CratesTNTPos.push_back(XMFLOAT3{ -1200.0f, 90.0f, 1135.0f });
	m_CratesTNTPos.push_back(XMFLOAT3{ -1215.0f, 90.0f, 1135.0f });
	m_CratesTNTPos.push_back(XMFLOAT3{ -1260.0f, 95.0f, 1135.0f });
	for (size_t i = 0; i < m_CratesTNTPos.size(); i++)
	{
		auto crateTNT = new CrateTNT{ m_pCharacter,m_CratesTNTPos[i] };
		m_pCratesTNT.push_back(crateTNT);
		AddChild(m_pCratesTNT[i]);
	}
	m_TotalBoxes += m_pCratesTNT.size();
#pragma endregion 

#pragma region NormalCrates
	//-- NORMAL CRATES --//
	
	//begin - sand beach
	m_CratesNormalPos.push_back(XMFLOAT3{ -20, 3.0f, 80.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -15.0f, 3.0f, 80.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -17.5f, 3.0f, 75.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -17.5f, 8.0f, 77.5f });

	//entrance hall stacked 	
	m_CratesNormalPos.push_back(XMFLOAT3{ -10.0f, -35.0f, 420.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -10.0f, -30.0f, 420.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -10.0f, -25.0f, 420.0f });

	m_CratesNormalPos.push_back(XMFLOAT3{ 20.0f, 5.0f, 1050.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ 20.0f, 10.0f, 1050.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ 20.0f, 15.0f, 1050.0f });

	//wumpa hall
	m_CratesNormalPos.push_back(XMFLOAT3{ -210.0f, 6.0f, 1020.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -230.0f, 6.0f, 1020.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -250.0f, 6.0f, 1020.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -270.0f, 6.0f, 1020.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -290.0f, 6.0f, 1020.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -310.0f, 6.0f, 1020.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -330.0f, 6.0f, 1020.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -350.0f, 6.0f, 1020.0f });

	m_CratesNormalPos.push_back(XMFLOAT3{ -210.0f, 6.0f, 1045.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -230.0f, 6.0f, 1045.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -250.0f, 6.0f, 1045.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -270.0f, 6.0f, 1045.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -290.0f, 6.0f, 1045.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -310.0f, 6.0f, 1045.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -330.0f, 6.0f, 1045.0f });
	m_CratesNormalPos.push_back(XMFLOAT3{ -350.0f, 6.0f, 1045.0f });

	for (size_t i = 0; i < m_CratesNormalPos.size(); i++)
	{
		auto crateNormal = new CrateNormal{ m_pCharacter,m_CratesNormalPos[i] };
		m_pCratesNormal.push_back(crateNormal);
		AddChild(m_pCratesNormal[i]);
	}
	m_TotalBoxes += m_pCratesNormal.size();
#pragma endregion

#pragma region NitroCrates
	//-- NITRO CRATES --//
	
	//entrance 
	m_CratesNitroPos.push_back(XMFLOAT3{ -10.0f, 3.0f, 156.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 24, 3.0f, 156.0f });

	//entrance hall 2 NITRO TUNNEL A MATTI
	m_CratesNitroPos.push_back(XMFLOAT3{ 10.0f, -35.0f, 435.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 15.0f, -35.0f, 435.0f });

	m_CratesNitroPos.push_back(XMFLOAT3{ -5.0f, -35.0f, 455.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -10.0f, -35.0f, 455.0f });

	m_CratesNitroPos.push_back(XMFLOAT3{ 10.0f, -35.0f, 475.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 15.0f, -35.0f, 475.0f });

	m_CratesNitroPos.push_back(XMFLOAT3{ 0.0f, -35.0f, 495.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -5.0f, -35.0f, 495.0f });

	m_CratesNitroPos.push_back(XMFLOAT3{ 10.0f, -35.0f, 515.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 15.0f, -35.0f, 515.0f });

	//first pit -- left
	m_CratesNitroPos.push_back(XMFLOAT3{ -35.0f, 6.0f, 750.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -25.0f, 6.0f,750.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -15.0f, 6.0f, 750.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{  -5.0f, 6.0f,750.0f });

	//first pit -- right
	m_CratesNitroPos.push_back(XMFLOAT3{ 50.0f, 6.0f, 750.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 40.0f, 6.0f,750.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 30.0f, 6.0f, 750.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 20.0f, 6.0f,750.0f });


	//second pit  - left 
	m_CratesNitroPos.push_back(XMFLOAT3{ -35.0f, 6.0f, 780.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -25.0f, 6.0f,780.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -15.0f, 6.0f, 780.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -5.0f, 6.0f,780.0f });

	//second pit  - right
	m_CratesNitroPos.push_back(XMFLOAT3{ 50.0f, 6.0f, 780.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 40.0f, 6.0f,780.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 30.0f, 6.0f, 780.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 20.0f, 6.0f,780.0f });
	
	//third pit  left
	m_CratesNitroPos.push_back(XMFLOAT3{ -35.0f, 6.0f, 825.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -25.0f, 6.0f,825.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -15.0f, 6.0f, 825.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -5.0f, 6.0f,825.0f });

	//third pit  - right
	m_CratesNitroPos.push_back(XMFLOAT3{ 50.0f, 6.0f, 825.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 40.0f, 6.0f,825.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 30.0f, 6.0f, 825.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 20.0f, 6.0f,825.0f });

	//fourth pit  left
	m_CratesNitroPos.push_back(XMFLOAT3{ -35.0f, 6.0f, 872.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -25.0f, 6.0f,872.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -15.0f, 6.0f, 872.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -5.0f, 6.0f,872.0f });

	//fourth pit  - right
	m_CratesNitroPos.push_back(XMFLOAT3{ 50.0f, 6.0f, 872.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 40.0f, 6.0f,872.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 30.0f, 6.0f, 872.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 20.0f, 6.0f,872.0f });

	//fifth pit  left
	m_CratesNitroPos.push_back(XMFLOAT3{ -35.0f, 6.0f, 920.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -25.0f, 6.0f,920.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -15.0f, 6.0f, 920.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -5.0f, 6.0f,920.0f });

	//fifth pit  - right
	m_CratesNitroPos.push_back(XMFLOAT3{ 50.0f, 6.0f, 920.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 40.0f, 6.0f,920.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 30.0f, 6.0f, 920.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 20.0f, 6.0f,920.0f });

	//CHECKPOINT02
	m_CratesNitroPos.push_back(XMFLOAT3{ 50.0f, 6.0f, 970.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 40.0f, 6.0f,970.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 30.0f, 6.0f, 970.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 20.0f, 6.0f,970.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 10.0f, 6.0f,970.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 0.0f, 6.0f,970.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -10.0f, 6.0f,970.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -20.0f, 6.0f,970.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -30.0f, 6.0f, 970.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -35.0f, 6.0f, 970.0f });

	m_CratesNitroPos.push_back(XMFLOAT3{ 50.0f, 6.0f, 990.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 40.0f, 6.0f,990.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 30.0f, 6.0f, 990.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 20.0f, 6.0f,990.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 10.0f, 6.0f,990.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 0.0f, 6.0f,990.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -10.0f, 6.0f,990.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -20.0f, 6.0f,990.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -30.0f, 6.0f, 990.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -35.0f, 6.0f, 990.0f });

	m_CratesNitroPos.push_back(XMFLOAT3{ 50.0f, 6.0f, 1010.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 40.0f, 6.0f,1010.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 30.0f, 6.0f, 1010.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 20.0f, 6.0f,1010.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 10.0f, 6.0f,1010.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 0.0f, 6.0f,1010.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -10.0f, 6.0f,1010.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -20.0f, 6.0f,1010.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -30.0f, 6.0f, 1010.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -35.0f, 6.0f, 1010.0f });

	m_CratesNitroPos.push_back(XMFLOAT3{ 50.0f, 6.0f, 1030.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 40.0f, 6.0f,1030.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 30.0f, 6.0f, 1030.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 20.0f, 6.0f,1030.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 10.0f, 6.0f,1030.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ 0.0f, 6.0f,1030.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -10.0f, 6.0f,1030.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -20.0f, 6.0f,1030.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -30.0f, 6.0f, 1030.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -35.0f, 6.0f, 1030.0f });

	//checkpoint 3 -- hall
	m_CratesNitroPos.push_back(XMFLOAT3{ -100.0f, 6.0f, 1120.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -120.0f, 6.0f, 1100.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -140.0, 6.0f, 1005.0f });

	m_CratesNitroPos.push_back(XMFLOAT3{ -145, 6.0f, 1050.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -155.0f, 6.0f, 1045.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -165.0f, 6.0f, 1045.0f });

	//second last checkpoint
	m_CratesNitroPos.push_back(XMFLOAT3{ -500.0f, 47.0f, 1000.0f});
	m_CratesNitroPos.push_back(XMFLOAT3{ -520.0f, 47.0f, 1000.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -540.0f, 47.0f, 1000.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -560.0f, 47.0f, 1000.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -580.0f, 47.0f, 1000.0f });

	m_CratesNitroPos.push_back(XMFLOAT3{ -500.0f, 47.0f, 1060.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -520.0f, 47.0f, 1060.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -540.0f, 47.0f, 1060.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -560.0f, 47.0f, 1060.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -580.0f, 47.0f, 1060.0f });

	//last part
	m_CratesNitroPos.push_back(XMFLOAT3{ -780.0f, 47.0f, 1000.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -780.0f, 47.0f, 1060.0f });

	//wood

	m_CratesNitroPos.push_back(XMFLOAT3{ -1120.0f, 90.0f, 1135.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -1140.0f, 90.0f, 1135.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -1160.0f, 90.0f, 1135.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -1246.0f, 95.0f, 1137.0f });
	m_CratesNitroPos.push_back(XMFLOAT3{ -1276.0f, 102.0f, 1137.0f });
	for (size_t i = 0; i < m_CratesNitroPos.size(); i++)
	{
		auto crateNitro = new CrateNitro{ m_pCharacter,m_CratesNitroPos[i] };
		m_pCratesNitro.push_back(crateNitro);
		AddChild(m_pCratesNitro[i]);
	}
#pragma endregion

#pragma region LifeCrates
	//-- LIFE CRATES --//

	//entrance hall stacked 	
	m_CratesLifePos.push_back(XMFLOAT3{ 20.0f, -35.0f, 390.0f });
	m_CratesLifePos.push_back(XMFLOAT3{ 20.0f, -30.0f, 390.0f });
	m_CratesLifePos.push_back(XMFLOAT3{ 20.0f, -25.0f, 390.0f });

	//NITRO JUMP ATTACK
	m_CratesLifePos.push_back(XMFLOAT3{ -15.0f, 5.0f, 720.0f });
	m_CratesLifePos.push_back(XMFLOAT3{ -15.0f, 10.0f, 720.0f });
	m_CratesLifePos.push_back(XMFLOAT3{ -15.0f, 15.0f, 720.0f });

	m_CratesLifePos.push_back(XMFLOAT3{ 20.0f, 5.0f, 730.0f });
	m_CratesLifePos.push_back(XMFLOAT3{ 20.0f, 10.0f, 730.0f });
	m_CratesLifePos.push_back(XMFLOAT3{ 20.0f, 15.0f, 730.0f });
	
	m_CratesLifePos.push_back(XMFLOAT3{ -20.0f, 5.0f, 1060.0f });
	m_CratesLifePos.push_back(XMFLOAT3{ -20.0f, 10.0f, 1060.0f });
	m_CratesLifePos.push_back(XMFLOAT3{ -20.0f, 15.0f, 1060.0f });

	m_CratesLifePos.push_back(XMFLOAT3{ -1020.0f, 104, 1140.0f });
	m_CratesLifePos.push_back(XMFLOAT3{ -1020.0f, 109.0f, 1140.0f });
	m_CratesLifePos.push_back(XMFLOAT3{ -1020.0f, 114.0f, 1140.0f });

	//-1032.0f, 100.0f, 1115.0f


	for (size_t i = 0; i < m_CratesLifePos.size(); i++)
	{
		auto crateLife = new CrateLife{ m_pCharacter,m_CratesLifePos[i] };
		m_pCratesLife.push_back(crateLife);
		AddChild(m_pCratesLife[i]);
	}
	m_TotalBoxes += m_pCratesLife.size();
#pragma endregion 

	//-- PICK-UPS --//
#pragma region WumpaFruit
	//-- WUMPA FRUITS --//

	//first hallway
	m_WumpaFruitPos.push_back(XMFLOAT3{ 5.0f, 5.0f, 155.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 5.0f, 0.0f, 175.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 5.0f, -5.0f, 195.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 5.0f, -15.0f, 215.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 5.0f, -25.0f, 235 });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 5.0f, -28.0f, 255.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 5.0f, -30.0f, 275.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 5.0f, -30.0f, 295.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 5.0f, -30.0f, 315.0f });


	m_WumpaFruitPos.push_back(XMFLOAT3{ 7.0f, -30.0f, 530.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 7.0f, -20.0f, 550.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 7.0f, -10.0f, 570.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 7.0f, 0.0f, 590.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 7.0f, 5.0f, 610 });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 7.0f, 10.0f, 630.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 7.0f, 12.0f, 650 });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 7.0f, 12.0f, 680 });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 7.0f, 12.0f, 700.0f });

	//nitro fest pit one - left
	m_WumpaFruitPos.push_back(XMFLOAT3{ -35.0f, 10.0f, 790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -30.0f, 10.0f, 790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -25.0f, 10.0f,790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -20.0f, 10.0f,790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -15.0f, 10.0f, 790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -10.0f, 10.0f, 790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -5.0f, 10.0f,790.0f });

	//nitro fest pit one - right
	m_WumpaFruitPos.push_back(XMFLOAT3{ 20.0f, 10.0f, 790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 25.0f, 10.0f, 790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 30.0f, 10.0f,790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 35.0f, 10.0f,790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 40.0f, 10.0f, 790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{  45.0f, 10.0f, 790.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 50.0f, 10.0f,790.0f });

	//nitro fest pit two - left
	m_WumpaFruitPos.push_back(XMFLOAT3{ -35.0f, 10.0f, 835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -30.0f, 10.0f, 835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -25.0f, 10.0f,835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -20.0f, 10.0f,835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -15.0f, 10.0f, 835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -10.0f, 10.0f, 835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -5.0f, 10.0f,835.0f });

	//nitro fest pit two - right
	m_WumpaFruitPos.push_back(XMFLOAT3{ 20.0f, 10.0f, 835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 25.0f, 10.0f, 835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 30.0f, 10.0f,835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 35.0f, 10.0f,835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 40.0f, 10.0f, 835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 45.0f, 10.0f, 835.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 50.0f, 10.0f,835.0f });

	//nitro fest pit three - left
	m_WumpaFruitPos.push_back(XMFLOAT3{ -35.0f, 10.0f, 885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -30.0f, 10.0f, 885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -25.0f, 10.0f,885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -20.0f, 10.0f,885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -15.0f, 10.0f, 885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -10.0f, 10.0f, 885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -5.0f, 10.0f,885.0f });

	//nitro fest pit three - right
	m_WumpaFruitPos.push_back(XMFLOAT3{ 20.0f, 10.0f, 885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 25.0f, 10.0f, 885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 30.0f, 10.0f,885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 35.0f, 10.0f,885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 40.0f, 10.0f, 885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 45.0f, 10.0f, 885.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 50.0f, 10.0f,885.0f });

	//nitro fest pit four - left
	m_WumpaFruitPos.push_back(XMFLOAT3{ -35.0f, 10.0f, 930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -30.0f, 10.0f, 930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -25.0f, 10.0f,930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -20.0f, 10.0f,930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -15.0f, 10.0f, 930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -10.0f, 10.0f, 930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -5.0f, 10.0f,930.0f });

	//nitro fest pit four - right
	m_WumpaFruitPos.push_back(XMFLOAT3{ 20.0f, 10.0f, 930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 25.0f, 10.0f, 930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 30.0f, 10.0f,930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 35.0f, 10.0f,930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 40.0f, 10.0f, 930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 45.0f, 10.0f, 930.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ 50.0f, 10.0f,930.0f });

	//after checkpoint 3 hall
	m_WumpaFruitPos.push_back(XMFLOAT3{ -130.0f, 10.0f, 1060.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -135, 10.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -155, 10.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -175, 10.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -195, 10.0f, 1035.0f });

	m_WumpaFruitPos.push_back(XMFLOAT3{ -135, 10.0f, 1065.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -155, 10.0f, 1065.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -175, 10.0f, 1065.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -195, 10.0f, 1065.0f });

	m_WumpaFruitPos.push_back(XMFLOAT3{ -210.0f, 10.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -230.0f, 10.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -250.0f, 10.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -270.0f, 10.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -290.0f, 10.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -310.0f, 10.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -330.0f, 10.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -350.0f, 10.0f, 1035.0f });

	//last piece
	m_WumpaFruitPos.push_back(XMFLOAT3{ -800.0f, 50.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -820.0f, 50.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -840.0f, 50.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -860.0f, 50.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -880.0f, 50.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -900.0f, 50.0f, 1035.0f });

	//hill
	m_WumpaFruitPos.push_back(XMFLOAT3{ -920.0f, 55.0f, 1035.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -940.0f, 70.0f, 1038.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -960.0f, 75.0f, 1040.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -980.0f, 80.0f, 1045.0f });
	m_WumpaFruitPos.push_back(XMFLOAT3{ -1000.0f, 85.0f, 1050.0f });

	for (size_t i = 0; i < m_WumpaFruitPos.size(); i++)
	{
		auto wumpa = new WumpaFruit{ m_pCharacter,m_WumpaFruitPos[i] };
		m_pWumpaFruits.push_back(wumpa);
		AddChild(m_pWumpaFruits[i]);
	}

#pragma endregion


	auto crystal = new Crystal{ XMFLOAT3{-640.0f, 53.f, 1034.0f} };
	AddChild(crystal);

}

void Level::PitFallDead()
{
	m_pSoundManager->GetSystem()->playSound(m_pSoundFalling, 0, false, 0);
	m_pCharacter->SetCrashMovement(CrashBandicoot::dead);
	if (!PlayerInventory::GetInstance()->IsPlayerDead())
	{
		PlayerInventory::GetInstance()->SetHealth(-1);
	}

}

void Level::ChangeCam()
{
	const auto gameContext = GetGameContext();

	switch (m_CamIndex)
	{
	case 0:
		if (m_CamRotator >= 0.0f)
		{
			--m_CamRotator;
			m_pFollowCam->SetCamRot({ 0.0f,-m_CamRotator,0.0f });
		}
		break;
	case 1:
		if (m_CamRotator <= 90.0f)
		{
			++m_CamRotator ;
			m_pFollowCam->SetCamRot({ 0.0f,-m_CamRotator,0.0f });
		}
	
		break;
	}
}
