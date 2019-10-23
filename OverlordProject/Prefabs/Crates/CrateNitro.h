#pragma once
#include "GameObject.h"
#include "ColliderComponent.h"
#include "SoundManager.h"

class CrateNitro final : public GameObject
{
public:
	CrateNitro(GameObject* crashBandicoot, DirectX::XMFLOAT3 cratePos = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f });
	CrateNitro(const CrateNitro& other) = delete;
	CrateNitro(CrateNitro&& other) noexcept = delete;
	CrateNitro& operator=(const CrateNitro& other) = delete;
	CrateNitro& operator=(CrateNitro&& other) noexcept = delete;
	virtual ~CrateNitro() = default;
	void RemoveAllObjects();

protected:
	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;

private:
	DirectX::XMFLOAT3 m_Pos;
	GameObject *m_pTriggerBox, *m_pNormalBox;
	ColliderComponent* m_ColliderBoxTrigger;
	GameObject* m_pCrashBandicoot;
	SoundManager* m_pSoundManager;
	FMOD::Sound * m_pSound, *m_pSoundDead;
	bool m_IsPlayerInTrigger;
};

