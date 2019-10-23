#include "stdafx.h"
#include "CrateLife.h"
#include "PhysxManager.h"
#include "ContentManager.h"
#include "Components.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "../../Materials/DiffuseMaterial.h"
#include "../PickUps/Life.h"
#include "../Character/CrashBandicoot.h"
#include "../Player/PlayerInventory.h"


CrateLife::CrateLife(GameObject* crashBandicoot, DirectX::XMFLOAT3 cratePos)
	:m_Pos(cratePos)
{
	m_pCrashBandicoot = crashBandicoot;
	m_IsTriggeredTop = false;
	m_HasSmashed = false;
	m_IsPlayerInTrigger = false;
	m_IsSpinning = false;
}

void CrateLife::Initialize(const GameContext& gameContext)
{
	//-- Essentials --//
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial *pDefaultMaterial = physX->createMaterial(0.5f, 0.0f, 0.5f);


	//-- MATERIALS --//
	if (!gameContext.pMaterialManager->GetMaterial(5002))
	{
		auto pNormalCrateMat = new DiffuseMaterial();
		pNormalCrateMat->SetDiffuseTexture(L"Resources/Game/Texture/crate_crash.bmp");
		gameContext.pMaterialManager->AddMaterial(pNormalCrateMat, 5002);
	}

	//-- MAIN BOX GAME OBJECT --//
	m_pNormalBox = new GameObject{};
	m_pNormalBox->AddComponent(new ModelComponent{ L"Resources/Game/Models/Crate_Normal.ovm",true });
	m_pNormalBox->AddComponent(new RigidBodyComponent());
	m_pNormalBox->GetComponent<RigidBodyComponent>()->SetKinematic(true);
	auto pConvex = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Game/Models/Crate_Normal.ovpc");
	std::shared_ptr<physx::PxGeometry> geom(new physx::PxConvexMeshGeometry(pConvex, physx::PxMeshScale(0.1f)));
	auto collider = new ColliderComponent(geom, *pDefaultMaterial);
	m_pNormalBox->AddComponent(collider);
	m_pNormalBox->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	m_pNormalBox->GetTransform()->Translate(m_Pos);
	m_pNormalBox->GetTransform()->Rotate(90.0f, 0.0f, 0.0f);
	m_pNormalBox->GetComponent<ModelComponent>()->SetMaterial(5002);
	AddChild(m_pNormalBox);

	//-- TOP TRIGGER BOX GAME OBJECT --//
	m_pTriggerTop = new GameObject();
	m_pTriggerTop->AddComponent(new RigidBodyComponent());
	std::shared_ptr<physx::PxGeometry> geomTop = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 2.0f,1.0f,2.0f });
	m_ColliderTopTrigger = new ColliderComponent(geomTop, *pDefaultMaterial, physx::PxTransform(0.0f, 7.0f, 0.0f));
	m_ColliderTopTrigger->EnableTrigger(true);
	m_pTriggerTop->GetComponent<RigidBodyComponent>()->SetKinematic(true);
	m_pTriggerTop->AddComponent(m_ColliderTopTrigger);
	m_pTriggerTop->GetTransform()->Translate(m_Pos);
	AddChild(m_pTriggerTop);


	//-- TRIGGER BOX GAME OBJECT --//
	m_pTriggerBox = new GameObject();
	m_pTriggerBox->AddComponent(new RigidBodyComponent());
	std::shared_ptr<physx::PxGeometry> geomBox = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 4.0f,2.0f,4.0f });
	m_ColliderBoxTrigger = new ColliderComponent(geomBox, *pDefaultMaterial, physx::PxTransform(0.0f, 3.0f, 0.0f));
	m_ColliderBoxTrigger->EnableTrigger(true);
	m_pTriggerBox->GetComponent<RigidBodyComponent>()->SetKinematic(true);
	m_pTriggerBox->AddComponent(m_ColliderBoxTrigger);
	m_pTriggerBox->GetTransform()->Translate(m_Pos);
	AddChild(m_pTriggerBox);

	
	//CRASH JUMPS ON CRATE TRIGGER
	if (m_pTriggerTop->GetActive())
	{
		m_pTriggerTop->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
		{
			UNREFERENCED_PARAMETER(trigger);
			if (receive->GetTag() == L"CRASH" && action == GameObject::TriggerAction::ENTER)
			{
				m_IsTriggeredTop = true;
			}
		});
	}


	//CRASH SPINS ON CRATE TRIGGER
	if (m_pTriggerBox->GetActive())
	{
		m_pTriggerBox->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
		{
			UNREFERENCED_PARAMETER(trigger);
			UNREFERENCED_PARAMETER(action);
			if (receive->GetTag() == L"CRASH" && action == GameObject::TriggerAction::ENTER)
			{
				m_IsPlayerInTrigger = true;
			}
			else
			{
				m_IsPlayerInTrigger = false;

			}
		});
	}
	//-- SOUND EFFECTS --//
	m_pSoundManager = SoundManager::GetInstance();
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/CrateBreakSound.wav", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSound);
}

void CrateLife::Update(const GameContext&)
{
	m_IsSpinning = static_cast<CrashBandicoot*>(m_pCrashBandicoot)->IsSpinning();

	//CRASH JUMPS ON CRATE LOGIC
	if (m_IsTriggeredTop)
	{
		static_cast<CrashBandicoot*>(m_pCrashBandicoot)->SetJump(true);
		m_pSoundManager->GetSystem()->playSound(m_pSound, 0, false, 0);
		RemoveAllObjects();
		SetLife();
		m_IsTriggeredTop = false;
	}

	//CRASH SPINS ON CRATE
	
		if (m_IsPlayerInTrigger)
		{
			if (m_IsSpinning)
			{
				m_pSoundManager->GetSystem()->playSound(m_pSound, 0, false, 0);
				RemoveAllObjects();
				m_IsPlayerInTrigger = false;
				SetLife();
			}
		}



	if (m_pTriggerTop == nullptr && m_pTriggerBox == nullptr && m_pTriggerBox == nullptr)
	{
		SceneManager::GetInstance()->GetActiveScene()->RemoveChild(this);

	}
}

void CrateLife::RemoveAllObjects()
{
	if (m_pTriggerTop != nullptr)
	{
		m_pTriggerTop->SetActive(false);
		delete m_pTriggerTop;
		RemoveChild(m_pTriggerTop);
	}
	if (m_pNormalBox != nullptr)
	{
		m_pNormalBox->SetActive(false);
		delete m_pNormalBox;
		RemoveChild(m_pNormalBox);
	}
	if (m_pTriggerBox != nullptr)
	{
		m_pTriggerBox->SetActive(false);
		delete m_pTriggerBox;
		RemoveChild(m_pTriggerBox);
	}
}

void CrateLife::SetLife()
{
	m_pLife = new Life{ m_pCrashBandicoot };
	SceneManager::GetInstance()->GetActiveScene()->AddChild(m_pLife);
	m_pLife->GetTransform()->Translate(m_Pos.x, m_Pos.y + 3.f, m_Pos.z);
	PlayerInventory::GetInstance()->SetSmashedBoxes(1);
}
