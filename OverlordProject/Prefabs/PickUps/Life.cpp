#include "stdafx.h"
#include "Life.h"
#include "Components.h"
#include "PhysxManager.h"
#include "../../Materials/DiffuseMaterial.h"
#include "../Player/PlayerInventory.h"
#include "ContentManager.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "../Character/CrashBandicoot.h"

Life::Life(GameObject* crashBandicoot, DirectX::XMFLOAT3 lifePosition)
	:m_Pos(lifePosition)
{
	m_IsRemoved = false;
	m_IsTaken = false;
	m_pCrashBandicoot = crashBandicoot;
}

void Life::Initialize(const GameContext& gameContext)
{
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial *pDefaultMaterial = physX->createMaterial(0.5f, 0.0f, 0.5f);

	if (!gameContext.pMaterialManager->GetMaterial(9001))
	{
		auto pLifeMat = new DiffuseMaterial();
		pLifeMat->SetDiffuseTexture(L"Resources/Game/Texture/TEX_LIFE.png");
		gameContext.pMaterialManager->AddMaterial(pLifeMat, 9001);
	}

	m_pLife = new ModelComponent(L"Resources/Game/Models/Life.ovm", true);
	auto rigidBody = new RigidBodyComponent();
	rigidBody->SetKinematic(true);
	AddComponent(rigidBody);

	auto pConvex = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Game/Models/Life.ovpc");
	std::shared_ptr<physx::PxGeometry> geom(new physx::PxConvexMeshGeometry(pConvex, physx::PxMeshScale(0.1f)));
	auto collider = new ColliderComponent(geom, *pDefaultMaterial);

	collider->EnableTrigger(true);


	m_pLife->SetMaterial(9001);
	AddComponent(collider);
	AddComponent(m_pLife);



	m_pLife->GetTransform()->Scale(0.1f,0.1f,0.1f);
	m_pLife->GetTransform()->Translate(m_Pos);

	this->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
	{
		UNREFERENCED_PARAMETER(trigger);
		if (receive->GetTag() == L"CRASH" && action == GameObject::TriggerAction::ENTER)
		{
			if (static_cast<CrashBandicoot*>(m_pCrashBandicoot)->IsSpinning())
			{
				m_IsRemoved = true;
			}
			else
			{
				m_IsTaken = true;
			}


		}
	});

	m_pSoundManager = SoundManager::GetInstance();
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/LifeSound.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundTaken);
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/WumpaFruit02.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundRemoved);
}

void Life::Update(const GameContext& gameContext)
{
	float rotateSpeed = 200.0f;
	
	m_pLife->GetTransform()->Rotate(0.0f, gameContext.pGameTime->GetTotal() * rotateSpeed, 0.0f, true);

	if (m_IsTaken)
	{
		m_pSoundManager->GetSystem()->playSound(m_pSoundTaken, 0, false, 0);
		PlayerInventory::GetInstance()->SetHealth(1);
		SceneManager::GetInstance()->GetActiveScene()->RemoveChild(this);
	}

	if (m_IsRemoved)
	{
		m_pSoundManager->GetSystem()->playSound(m_pSoundRemoved, 0, false, 0);
		SceneManager::GetInstance()->GetActiveScene()->RemoveChild(this);
	}


}


