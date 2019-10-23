#pragma once
#include "GameObject.h"
#include "ControllerComponent.h"
#include "ModelComponent.h"
#include "RigidBodyComponent.h"
#include "SoundManager.h"

class FollowCam;
class CrashBandicoot : public GameObject
{
public:
	enum CrashMovement : UINT
	{
		idle,
		walking,
		left,
		right,
		jump,
		forward,
		backward,
		spin,
		slam,
		dance,
		dead
	};

	CrashBandicoot(float radius = 2, float height = 5, float moveSpeed = 100);
	virtual ~CrashBandicoot() = default;

	CrashBandicoot(const CrashBandicoot& other) = delete;
	CrashBandicoot(CrashBandicoot&& other) noexcept = delete;
	CrashBandicoot& operator=(const CrashBandicoot& other) = delete;
	CrashBandicoot& operator=(CrashBandicoot&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

	CameraComponent* GetCamera() const { return m_pCamera; }

	void SetCrashMovement(CrashMovement crashMovement);
	void SetJump(bool isJumping);

	bool IsSpinning();
	void SetSpinning(bool isSpinning);

	void SetCamera(FollowCam* cam);
	void Respawn(float elapsedSec);
	void SetRespawnTarget(DirectX::XMFLOAT3 respawnLocation);


protected:
	CameraComponent* m_pCamera;
	ControllerComponent* m_pController;

	float m_TotalPitch, m_TotalYaw;
	float m_MoveSpeed, m_RotationSpeed;
	float m_Radius, m_Height;

	//Running
	float m_MaxRunVelocity,
		m_TerminalVelocity,
		m_Gravity,
		m_RunAccelerationTime,
		m_JumpAccelerationTime,
		m_RunAcceleration,
		m_JumpAcceleration,
		m_RunVelocity,
		m_JumpVelocity;

	bool m_IsSpinning;
	bool m_IsJumping;
	bool m_JumpedOnBox;
	bool m_IsDead;
	bool m_IsDancing;
	float m_RespawnTimer;

	DirectX::XMFLOAT3 m_Velocity, m_SpawnPoint;
	CrashMovement m_CrashMovement = CrashMovement::idle;
	FMOD::Sound * m_pSoundSpin, *m_pSoundJump;
	SoundManager* m_pSoundManager;
	ModelComponent* m_pModel;
	GameObject* m_pObj;
	int m_AnimationIndex = 0;
	float m_SavedAngle{ 0 };
	RigidBodyComponent* m_pRigid;
	FollowCam* m_FollowCam;
};

