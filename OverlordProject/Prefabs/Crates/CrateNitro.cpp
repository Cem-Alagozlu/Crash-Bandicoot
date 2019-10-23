#include "stdafx.h"
#include "CrateNitro.h"
#include "PhysxManager.h"
#include "ContentManager.h"
#include "Components.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "../../Materials/DiffuseMaterial.h"
#include "../Player/PlayerInventory.h"
#include "../Character/CrashBandicoot.h"


CrateNitro::CrateNitro(GameObject* crashBandicoot, DirectX::XMFLOAT3 cratePos)
	:m_Pos(cratePos)
{
	m_IsPlayerInTrigger = false;
	m_pCrashBandicoot = crashBandicoot;
}



void CrateNitro::Initialize(const GameContext& gameContext)
{
	//-- Essentials --//
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial *pDefaultMaterial = physX->createMaterial(0.5f, 0.0f, 0.5f);


	//-- MATERIALS --//
	if (!gameContext.pMaterialManager->GetMaterial(5009))
	{
		auto pNormalCrateMat = new DiffuseMaterial();
		pNormalCrateMat->SetDiffuseTexture(L"Resources/Game/Texture/crate_nitro.bmp");
		gameContext.pMaterialManager->AddMaterial(pNormalCrateMat, 5009);
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
	m_pNormalBox->GetComponent<ModelComponent>()->SetMaterial(5009);
	AddChild(m_pNormalBox);

	//-- TRIGGER BOX GAME OBJECT --//
	m_pTriggerBox = new GameObject();
	m_pTriggerBox->AddComponent(new RigidBodyComponent());
	std::shared_ptr<physx::PxGeometry> geomBox = std::make_shared<physx::PxBoxGeometry>(physx::PxVec3{ 4.0f,4.0f,4.0f });
	m_ColliderBoxTrigger = new ColliderComponent(geomBox, *pDefaultMaterial, physx::PxTransform(0.0f, 3.f, 0.0f));
	m_ColliderBoxTrigger->EnableTrigger(true);
	m_pTriggerBox->GetComponent<RigidBodyComponent>()->SetKinematic(true);
	m_pTriggerBox->AddComponent(m_ColliderBoxTrigger);
	m_pTriggerBox->GetTransform()->Translate(m_Pos);
	AddChild(m_pTriggerBox);

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

	m_pSoundManager = SoundManager::GetInstance();
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/nitro_explosion.wav", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSound);
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/dead.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundDead);
}

void CrateNitro::Update(const GameContext& )
{
	if (m_IsPlayerInTrigger)
	{
		m_pSoundManager->GetSystem()->playSound(m_pSound, 0, false, 0);
		PlayerInventory::GetInstance()->SetHealth(-1);
		static_cast<CrashBandicoot*>(m_pCrashBandicoot)->SetCrashMovement(CrashBandicoot::CrashMovement::dead);
		m_pSoundManager->GetSystem()->playSound(m_pSoundDead, 0, false, 0);
		RemoveAllObjects();
		m_IsPlayerInTrigger = false;
	}
}

void CrateNitro::RemoveAllObjects()
{
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
	if (m_pTriggerBox == nullptr && m_pNormalBox == nullptr)
	{
		SceneManager::GetInstance()->GetActiveScene()->RemoveChild(this);
	}
}
