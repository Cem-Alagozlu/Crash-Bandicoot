#include "stdafx.h"
#include "CrashBandicoot.h"
#include "PhysxManager.h"
#include "FixedCamera.h"
#include "Components.h"
#include "ModelAnimator.h"
#include "FollowCam.h"
#include "../Player/PlayerInventory.h"
#include "../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"

using namespace DirectX;

CrashBandicoot::CrashBandicoot(float radius, float height, float moveSpeed) :
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pCamera(nullptr),
	m_pController(nullptr),
	m_TotalPitch(0),
	m_TotalYaw(0),
	m_RotationSpeed(70.0f),
	m_MaxRunVelocity(40.0f),
	m_TerminalVelocity(20),
	m_Gravity(3.0f),
	m_RunAccelerationTime(0.3f),
	m_JumpAccelerationTime(0.7f),
	m_RunAcceleration(m_MaxRunVelocity / m_RunAccelerationTime),
	m_JumpAcceleration(m_Gravity / m_JumpAccelerationTime),
	m_RunVelocity(0),
	m_JumpVelocity(0),
	m_IsSpinning(false),
	m_Velocity(0, 0, 0)
{
	m_IsJumping = false;
	m_JumpedOnBox = false;
	m_IsDead = false;
	m_SpawnPoint = XMFLOAT3{ 0.0f,0.0f,0.0f };
	m_RespawnTimer = 0.0f;
}


void CrashBandicoot::Initialize(const GameContext& gameContext) {
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto pDefaultMaterial = physX->createMaterial(0.0f, 0.0f, 1.0f);

	m_pController = new ControllerComponent(pDefaultMaterial);
	AddComponent(m_pController);



	m_pObj = new GameObject();
	m_pRigid = new RigidBodyComponent();

	std::shared_ptr<physx::PxGeometry> geom = std::make_shared<physx::PxBoxGeometry>(1.35f, 3.f, 1.35f);
	auto collider = new ColliderComponent(geom, *pDefaultMaterial);

	m_pObj->AddComponent(m_pRigid);
	m_pObj->AddComponent(collider);
	m_pObj->SetTag(L"CRASH");
	AddChild(m_pObj);


	auto skinnedDiffuseMaterial = new SkinnedDiffuseMaterial_Shadow();
	skinnedDiffuseMaterial->SetDiffuseTexture(L"./Resources/Game/Texture/TEX_CRASH.png");
	skinnedDiffuseMaterial->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	gameContext.pMaterialManager->AddMaterial(skinnedDiffuseMaterial, 0);
	m_pModel = new ModelComponent(L"./Resources/Game/Models/Crash.ovm");
	m_pModel->SetMaterial(0);

	auto obj = new GameObject();
	obj->AddComponent(m_pModel);
	AddChild(obj);
	m_pModel->GetTransform()->Translate(0.0f, -8.f, 0.0f);
	m_pModel->GetTransform()->Scale(0.5f, 0.5f, 0.5f);
	obj->SetTag(L"CRASH");

	m_IsJumping = false;

	gameContext.pInput->AddInputAction(InputAction{ CrashMovement::left,InputTriggerState::Down,'Q',-1,XINPUT_GAMEPAD_DPAD_LEFT });
	gameContext.pInput->AddInputAction(InputAction{ CrashMovement::right,InputTriggerState::Down,'D',-1,XINPUT_GAMEPAD_DPAD_RIGHT });
	gameContext.pInput->AddInputAction(InputAction{ CrashMovement::forward,InputTriggerState::Down,'Z',-1,XINPUT_GAMEPAD_DPAD_UP });
	gameContext.pInput->AddInputAction(InputAction{ CrashMovement::backward,InputTriggerState::Down,'S',-1,XINPUT_GAMEPAD_DPAD_DOWN });
	gameContext.pInput->AddInputAction(InputAction{ CrashMovement::spin,InputTriggerState::Pressed,'G',-1,XINPUT_GAMEPAD_X });
	gameContext.pInput->AddInputAction(InputAction{ CrashMovement::jump,InputTriggerState::Pressed,VK_SPACE,-1,XINPUT_GAMEPAD_A });
	gameContext.pInput->AddInputAction(InputAction{ CrashMovement::dance,InputTriggerState::Pressed,'P',-1,XINPUT_GAMEPAD_RIGHT_SHOULDER });
	//gameContext.pInput->AddInputAction(InputAction{ CrashMovement::slam,InputTriggerState::Down,'H',-1,XINPUT_GAMEPAD_B });

		//-- SOUND EFFECTS --//
	m_pSoundManager = SoundManager::GetInstance();
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/spin.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundSpin);
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/crash_jump_0.wav", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundJump);
}

void CrashBandicoot::PostInitialize(const GameContext& /*gameContext*/)
{
}



void CrashBandicoot::Update(const GameContext& gameContext)
{
	float elapsedSec = gameContext.pGameTime->GetElapsed();
	XMFLOAT3 forwardMov{ m_FollowCam->GetTransform()->GetForward() };
	XMFLOAT3 fowardRight{ m_FollowCam->GetTransform()->GetRight() };
	XMFLOAT3 moveDir{ 0.0f,0.0f,0.0f };
	XMFLOAT3 nullVec{ 0.0f,0.0f,0.0f };

	m_pObj->GetTransform()->Translate(this->GetTransform()->GetWorldPosition());
	m_pObj->GetTransform()->Rotate(0.0f, 0.0f, 0.0f);

	ModelAnimator* animator = m_pModel->GetAnimator();

if (!m_IsDead && !PlayerInventory::GetInstance()->IsGameOver())
{
		if (!m_IsSpinning && !m_IsDancing)
		{
			if (m_Velocity.x == 0.0f   && m_Velocity.z == 0.0f && !m_IsJumping)
			{
				animator->Play();
				m_CrashMovement = CrashMovement::idle;
			}
			else
			{
				animator->Play();
				m_CrashMovement = CrashMovement::walking;
			}
		}
		else
		{
			animator->SetAnimationSpeed(2.0f);
			if (!animator->IsPlaying())
			{
				m_IsSpinning = false;
				m_CrashMovement = CrashMovement::idle;
				animator->Play();
			}
		}
	

	if (gameContext.pInput->IsActionTriggered(CrashMovement::spin) && !m_IsSpinning)
	{
		m_CrashMovement = CrashMovement::spin;
		m_IsSpinning = true;
		m_pSoundManager->GetSystem()->playSound(m_pSoundSpin, 0, false, 0);
	}

	if (gameContext.pInput->IsActionTriggered(CrashMovement::dance))
	{
		m_CrashMovement = CrashMovement::dance;
		m_IsDancing = true;
	}


	//MOVE FORWARD LOGIC
	if (gameContext.pInput->IsActionTriggered(CrashMovement::left))
	{
		moveDir.x += fowardRight.x * -1.0f;
		moveDir.z += fowardRight.z * -1.0f;
	}

	//MOVE BACKWARD LOGIC -1
	if (gameContext.pInput->IsActionTriggered(CrashMovement::right))
	{
		moveDir.x += fowardRight.x;
		moveDir.z += fowardRight.z;
	}

	if (gameContext.pInput->IsActionTriggered(CrashMovement::forward))
	{
		moveDir.x += forwardMov.x;
		moveDir.z += forwardMov.z;
	}

	if (gameContext.pInput->IsActionTriggered(CrashMovement::backward))
	{
		moveDir.x += forwardMov.x * -1.0f;
		moveDir.z += forwardMov.z * -1.0f;
	}
	

	if (!(moveDir.x == 0.0f) || !(moveDir.y == 0.0f) || !(moveDir.z == 0.0f))
	{
		m_RunVelocity += elapsedSec * m_RunAcceleration;
		Clamp(m_RunVelocity, m_MaxRunVelocity, -m_MaxRunVelocity);
		m_IsDancing = false;
		float velocityY = m_Velocity.y;

		m_Velocity = { moveDir.x * m_RunVelocity,moveDir.y * m_RunVelocity, moveDir.z * m_RunVelocity };
		m_Velocity.y = velocityY;
	}
	else
	{
		m_Velocity.x = 0.0f;
		m_Velocity.z = 0.0f;
	}

	if (m_Velocity.x != nullVec.x && m_Velocity.z != nullVec.z)
	{
		float angleRot = atan2(m_Velocity.x, m_Velocity.z);
		m_pModel->GetTransform()->Rotate(0.0f, angleRot + XM_PI, 0.0f, false);
	}

	//JUMPING LOGICS
	if (!m_pController->GetCollisionFlags().isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		m_JumpVelocity -= m_JumpAcceleration * elapsedSec;
		Clamp(m_JumpVelocity, m_TerminalVelocity, -m_TerminalVelocity);
	}
	else if (!m_IsDead && gameContext.pInput->IsActionTriggered(CrashMovement::jump) || m_JumpedOnBox)
	{
		m_CrashMovement = CrashMovement::jump;
		m_pSoundManager->GetSystem()->playSound(m_pSoundJump, 0, false, 0);
		animator->Reset(false);
		m_IsJumping = true;
		m_JumpVelocity = 0.0f;
		m_Velocity.y = 90.0f;
	}
	else
	{
		m_IsJumping = false;

	}

	if (m_pController->GetCollisionFlags().isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		m_JumpedOnBox = false;
		m_IsJumping = false;
	}

	if (m_IsJumping)
	{
		animator->SetAnimationSpeed(1.0f);
		m_CrashMovement = CrashMovement::jump;
	}

	m_Velocity.y += m_JumpVelocity;
	m_pController->Move(XMFLOAT3{ m_Velocity.x * elapsedSec,m_Velocity.y * elapsedSec,m_Velocity.z * elapsedSec });
}

	if (m_IsDead)
	{
		m_CrashMovement = CrashMovement::dead;
		Respawn(elapsedSec);
	}

	switch (m_CrashMovement)
	{
	case CrashMovement::idle:
		if (animator->GetClipName() != L"IdleAnim")
		{
			animator->SetAnimation(0);
		}
		break;
	case CrashMovement::walking:
		if (animator->GetClipName() != L"RunAnim")
		{
			animator->SetAnimation(1);
		}
		break;
	case CrashMovement::spin:
		m_IsSpinning = true;
		if (animator->GetClipName() != L"SpinAnim")
		{
			animator->SetAnimation(2,false);
		}
		break;
	case CrashMovement::jump:
		m_IsJumping = true;
		if (animator->GetClipName() != L"JumpAnim")
		{
			animator->SetAnimation(3,false);
		}
		break;
	case CrashMovement::dance:
		if (animator->GetClipName() != L"DanceAnim")
		{
			animator->SetAnimation(5, false);
		}
		break;
	}

	if(PlayerInventory::GetInstance()->IsGameOver())
	{
		m_CrashMovement = CrashMovement::idle;
	}
}

void CrashBandicoot::SetRespawnTarget(DirectX::XMFLOAT3 respawnLocation)
{
	m_SpawnPoint = respawnLocation;
}

void CrashBandicoot::SetCrashMovement(CrashMovement crashMovement)
{
	m_CrashMovement = crashMovement;

	if (crashMovement == CrashMovement::dead)
	{
		m_IsDead = true;
	}
}

void CrashBandicoot::SetJump(bool isJumping)
{
	m_JumpedOnBox = isJumping;
}

bool CrashBandicoot::IsSpinning()
{
	return m_IsSpinning;
}

void CrashBandicoot::SetSpinning(bool isSpinning)
{
	m_IsSpinning = isSpinning;
}

void CrashBandicoot::SetCamera(FollowCam* cam)
{
	m_FollowCam = cam;
}

void CrashBandicoot::Respawn(float elapsedSec)
{
	m_RespawnTimer += elapsedSec;
	PlayerInventory::GetInstance()->SetPlayerDead(true);
	GetComponent<ControllerComponent>()->Translate({ m_SpawnPoint });

	if (m_RespawnTimer >= 3.0f)
	{
		PlayerInventory::GetInstance()->SetPlayerDead(false);
		m_IsDead = false;
		m_RespawnTimer = 0.0f;
	}
}
