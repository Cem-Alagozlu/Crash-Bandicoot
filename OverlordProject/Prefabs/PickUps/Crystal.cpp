#include "stdafx.h"
#include "Crystal.h"
#include "PhysxManager.h"
#include "Components.h"
#include "../../Materials/DiffuseMaterial.h"
#include "ContentManager.h"
#include "../Player/PlayerInventory.h"
#include "SceneManager.h"
#include "GameScene.h"

Crystal::Crystal(DirectX::XMFLOAT3 crystalPos)
	:m_Pos(crystalPos)
{
	m_IsTaken = false;
}

void Crystal::Initialize(const GameContext& gameContext)
{
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial *pDefaultMaterial = physX->createMaterial(0.5f, 0.0f, 0.5f);

	auto pCrystalMat = new DiffuseMaterial();
	pCrystalMat->SetDiffuseTexture(L"Resources/Game/Texture/TEX_CRYSTAL.png");
	gameContext.pMaterialManager->AddMaterial(pCrystalMat, 9002);


	m_pCrystal = new ModelComponent(L"Resources/Game/Models/Crystal.ovm", true);
	auto rigidBody = new RigidBodyComponent();
	rigidBody->SetKinematic(true);
	AddComponent(rigidBody);

	auto pConvex = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Game/Models/Crystal.ovpc");
	std::shared_ptr<physx::PxGeometry> geom(new physx::PxConvexMeshGeometry(pConvex, physx::PxMeshScale(0.1f)));
	auto collider = new ColliderComponent(geom, *pDefaultMaterial);

	collider->EnableTrigger(true);


	m_pCrystal->SetMaterial(9002);
	AddComponent(collider);
	AddComponent(m_pCrystal);

	this->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* receive, GameObject::TriggerAction action)
	{
		UNREFERENCED_PARAMETER(trigger);
		UNREFERENCED_PARAMETER(receive);
		if (receive->GetTag() == L"CRASH" && action == GameObject::TriggerAction::ENTER)
		{
			m_IsTaken = true;

		}

	});

	m_pCrystal->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	m_pCrystal->GetTransform()->Translate(m_Pos);


	m_pSoundManager = SoundManager::GetInstance();
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/CrystalSound.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSound);
}

void Crystal::Update(const GameContext& gameContext)
{
	float rotateSpeed = 200.0f;
	float elapsedSec = gameContext.pGameTime->GetTotal();

	m_pCrystal->GetTransform()->Rotate(90.0f, elapsedSec * rotateSpeed, 0.0f, true);

	if (m_IsTaken)
	{
		m_pSoundManager->GetSystem()->playSound(m_pSound, 0, false, 0);
		PlayerInventory::GetInstance()->SetCrystalStatus(true);
		SceneManager::GetInstance()->GetActiveScene()->RemoveChild(this);
	}
}