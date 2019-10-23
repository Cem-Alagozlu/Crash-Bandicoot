#pragma once
#include "GameObject.h"
#include "ColliderComponent.h"
#include "SoundManager.h"

class CrateLife final : public GameObject
{
public:
	CrateLife(GameObject* crashBandicoot, DirectX::XMFLOAT3 cratePos = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f });

	CrateLife(const CrateLife& other) = delete;
	CrateLife(CrateLife&& other) noexcept = delete;
	CrateLife& operator=(const CrateLife& other) = delete;
	CrateLife& operator=(CrateLife&& other) noexcept = delete;
	virtual ~CrateLife() = default;

	void RemoveAllObjects();
	void SetLife();
protected:
	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;

private:
	DirectX::XMFLOAT3 m_Pos;
	GameObject* m_pTriggerTop, *m_pTriggerBox, *m_pNormalBox;
	ColliderComponent* m_ColliderTopTrigger, *m_ColliderBoxTrigger;
	bool m_IsTriggeredTop, m_HasSmashed,m_IsSpinning,m_IsPlayerInTrigger;
	GameObject* m_pCrashBandicoot;
	FMOD::Sound * m_pSound;
	SoundManager* m_pSoundManager;
	GameObject* m_pLife;
};

