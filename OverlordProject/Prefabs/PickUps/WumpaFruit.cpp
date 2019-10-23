#include "stdafx.h"
#include "Components.h"
#include "../OverlordProject/Materials/DiffuseMaterial.h"
#include "WumpaFruit.h"
#include "ContentLoader.h"
#include "ContentManager.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "SoundManager.h"
#include "../Player/PlayerInventory.h"
#include "SceneManager.h"
#include "../Character/CrashBandicoot.h"


WumpaFruit::WumpaFruit(GameObject* crashBandicoot,DirectX::XMFLOAT3 wumpaPosition)
	:m_Pos(wumpaPosition)
{
	m_IsRemoved = false;
	m_IsTaken = false;
	m_pCrashBandicoot = crashBandicoot;
}


void WumpaFruit::Initialize(const GameContext & gameContext)
{
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial *pDefaultMaterial = physX->createMaterial(0.5f, 0.0f, 0.5f);

	if (!gameContext.pMaterialManager->GetMaterial(9000))
	{
		auto pWumpaMat = new DiffuseMaterial();
		pWumpaMat->SetDiffuseTexture(L"Resources/Game/Texture/TEX_WUMPA.png");
		gameContext.pMaterialManager->AddMaterial(pWumpaMat, 9000);
	}

	m_pWumpaFruit = new ModelComponent(L"Resources/Game/Models/WumpaFruit.ovm", true);
	auto rigidBody = new RigidBodyComponent();
	rigidBody->SetKinematic(true);
	AddComponent(rigidBody);
	
	std::shared_ptr<physx::PxGeometry> geom = std::make_shared<physx::PxSphereGeometry>(1.0f);
	auto collider = new ColliderComponent(geom, *pDefaultMaterial);
	
	collider->EnableTrigger(true);




	m_pWumpaFruit->SetMaterial(9000);
	AddComponent(collider);
	AddComponent(m_pWumpaFruit);
	


	m_pWumpaFruit->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	m_pWumpaFruit->GetTransform()->Translate(m_Pos);

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


	m_pSoundManager  = SoundManager::GetInstance();
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/WumpaFruit01.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundTaken);
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/WumpaFruit02.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundRemoved);
	m_IsTaken = false;

}

void WumpaFruit::Update(const GameContext & gameContext)
{
	float rotateSpeed = 200.0f;

	m_pWumpaFruit->GetTransform()->Rotate(90.0f, gameContext.pGameTime->GetTotal() * rotateSpeed, 0.0f, true);


	if (m_IsTaken)
	{
	m_pSoundManager->GetSystem()->playSound(m_pSoundTaken, 0, false, 0);
	PlayerInventory::GetInstance()->SetWumpaFruits(1);
	SceneManager::GetInstance()->GetActiveScene()->RemoveChild(this);
	}

	else if (m_IsRemoved)
	{
		m_pSoundManager->GetSystem()->playSound(m_pSoundRemoved, 0, false, 0);
		SceneManager::GetInstance()->GetActiveScene()->RemoveChild(this);
	}

}