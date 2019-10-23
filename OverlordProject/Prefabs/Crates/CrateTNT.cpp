#include "stdafx.h"
#include "CrateTNT.h"
#include "PhysxManager.h"
#include "ContentManager.h"
#include "Components.h"
#include "../../Materials/DiffuseMaterial.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "../Character/CrashBandicoot.h"
#include "../Player/PlayerInventory.h"

CrateTNT::CrateTNT(GameObject* crashBandicoot, DirectX::XMFLOAT3 cratePos)
	:m_Pos(cratePos)
{
	m_BlowUpTimer = 0.0f;
	m_pCrashBandicoot = crashBandicoot;
	m_IsTriggeredTop = false;
	m_IsPlayerInRadius = false;
	m_IsSpinning = false;
}

void CrateTNT::Initialize(const GameContext& gameContext)
{
	//-- Essentials --//
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial *pDefaultMaterial = physX->createMaterial(0.5f, 0.0f, 0.5f);


	//-- MATERIALS --//
	if (!gameContext.pMaterialManager->GetMaterial(5005))
	{
		auto pNormalCrateMat = new DiffuseMaterial();
		pNormalCrateMat->SetDiffuseTexture(L"Resources/Game/Texture/crate_tnt.bmp");
		gameContext.pMaterialManager->AddMaterial(pNormalCrateMat, 5005);
	}

	if (!gameContext.pMaterialManager->GetMaterial(5006))
	{
		auto pTNT01MAT = new DiffuseMaterial();
		pTNT01MAT->SetDiffuseTexture(L"Resources/Game/Texture/crate_tnt_one.bmp");
		gameContext.pMaterialManager->AddMaterial(pTNT01MAT, 5006);
	}

	if (!gameContext.pMaterialManager->GetMaterial(5007))
	{
		auto pTNT02MAT = new DiffuseMaterial();
		pTNT02MAT->SetDiffuseTexture(L"Resources/Game/Texture/crate_tnt_two.bmp");
		gameContext.pMaterialManager->AddMaterial(pTNT02MAT, 5007);
	}

	if (!gameContext.pMaterialManager->GetMaterial(5008))
	{
		auto pTNT03MAT = new DiffuseMaterial();
		pTNT03MAT->SetDiffuseTexture(L"Resources/Game/Texture/crate_tnt_three.bmp");
		gameContext.pMaterialManager->AddMaterial(pTNT03MAT, 5008);
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
	m_pNormalBox->GetComponent<ModelComponent>()->SetMaterial(5005);
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


	//-- TRIGGER BLAST RADIUS GAME OBJECT --//
	m_pBlastRadius = new GameObject();
	m_pBlastRadius->AddComponent(new RigidBodyComponent());
	std::shared_ptr<physx::PxGeometry> geomBlast = std::make_shared<physx::PxSphereGeometry>(7.5f);
	m_ColliderBlastRadiusTrigger = new ColliderComponent(geomBlast, *pDefaultMaterial, physx::PxTransform(0.0f, 3.0f, 0.0f));
	m_ColliderBlastRadiusTrigger->EnableTrigger(true);
	m_pBlastRadius->GetComponent<RigidBodyComponent>()->SetKinematic(true);
	m_pBlastRadius->AddComponent(m_ColliderBlastRadiusTrigger);
	m_pBlastRadius->GetTransform()->Translate(m_Pos);
	AddChild(m_pBlastRadius);

	//CRASH JUMPS ON CRATE TRIGGER
	if (m_pTriggerTop->GetActive() && m_BlowUpTimer == 0.0f)
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

	//BLAST RADIUS
	if (m_pBlastRadius->GetActive() && m_pBlastRadius != nullptr)
	{
		m_pBlastRadius->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
		{
			UNREFERENCED_PARAMETER(trigger);
			if (receive->GetTag() == L"CRASH" && action == GameObject::TriggerAction::ENTER)
			{
				m_IsPlayerInRadius = true;
			}
			else
			{
				m_IsPlayerInRadius = false;
			}
		});
	}

	m_pSoundManager = SoundManager::GetInstance(); 
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/TNT.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSound);
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/tnt_explosion_0.wav", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundBOOM);
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/dead.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundDead);
}

void CrateTNT::Update(const GameContext& gameContext)
{
	m_IsSpinning = static_cast<CrashBandicoot*>(m_pCrashBandicoot)->IsSpinning();


	//CRASH JUMPS ON CRATE LOGIC
	if (m_IsTriggeredTop)
	{
			if (m_BlowUpTimer == 0.0f)
			{
				m_pSoundManager->GetSystem()->playSound(m_pSound, 0, false, 0);
				static_cast<CrashBandicoot*>(m_pCrashBandicoot)->SetJump(true);
				m_pNormalBox->GetComponent<ModelComponent>()->SetMaterial(5008);
			}

			if (m_BlowUpTimer > 1.0f && m_BlowUpTimer < 2.0f)
			{
				m_pNormalBox->GetComponent<ModelComponent>()->SetMaterial(5007);
			}

			if (m_BlowUpTimer > 2.0f && m_BlowUpTimer < 2.7f)
			{
				m_pNormalBox->GetComponent<ModelComponent>()->SetMaterial(5006);
			}
			if (m_BlowUpTimer > 2.8f)
			{
				m_IsTriggeredTop = false;
				if (m_IsPlayerInRadius)
				{
					PlayerInventory::GetInstance()->SetHealth(-1);
					static_cast<CrashBandicoot*>(m_pCrashBandicoot)->SetCrashMovement(CrashBandicoot::CrashMovement::dead);
					m_pSoundManager->GetSystem()->playSound(m_pSoundDead, 0, false, 0);
				}
				PlayerInventory::GetInstance()->SetSmashedBoxes(1);
				RemoveAllObjects();
			}

			m_BlowUpTimer += gameContext.pGameTime->GetElapsed() * 0.75f;

	}

	//CRASH SPINS ON CRATE
	if (m_IsSpinning && m_IsPlayerInRadius &&!m_IsTriggeredTop)
	{
		m_pSoundManager->GetSystem()->playSound(m_pSoundBOOM, 0, false, 0);
		PlayerInventory::GetInstance()->SetHealth(-1);
		PlayerInventory::GetInstance()->SetSmashedBoxes(1);
		static_cast<CrashBandicoot*>(m_pCrashBandicoot)->SetCrashMovement(CrashBandicoot::CrashMovement::dead);
		m_pSoundManager->GetSystem()->playSound(m_pSoundDead, 0, false, 0);
		RemoveAllObjects();
		m_IsSpinning = false;
		m_IsPlayerInRadius = false;
	}

}

void CrateTNT::RemoveAllObjects()
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

	if (m_pBlastRadius != nullptr)
	{
		m_pBlastRadius->SetActive(false);
		delete m_pBlastRadius;
		RemoveChild(m_pBlastRadius);
	}
	
}
