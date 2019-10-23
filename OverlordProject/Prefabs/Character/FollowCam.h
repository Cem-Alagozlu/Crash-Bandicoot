#pragma once
#include "GameObject.h"
#include "CrashBandicoot.h"

class FollowCam final : public GameObject
{
public:
	FollowCam(GameObject* crashBandicoot);
	FollowCam();

	FollowCam(const FollowCam& other) = delete;
	FollowCam(FollowCam&& other) noexcept = delete;
	FollowCam& operator=(const FollowCam& other) = delete;
	FollowCam& operator=(FollowCam&& other) noexcept = delete;
	virtual ~FollowCam() = default;

	void SetCamRot(DirectX::XMFLOAT3 rot = { 0.0f,0.0f,0.0f });
	DirectX::XMFLOAT3 GetCamRot();
	void ResetCamRot();

protected:
	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;

private:
	GameObject* m_pCrashBandicoot = nullptr;
	DirectX::XMFLOAT3 m_Rot;
};

