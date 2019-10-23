#pragma once
#include "GameObject.h"
#include "SoundManager.h"
#include "ColliderComponent.h"

class CrateCheckPoint  final : public GameObject
{
public:
	CrateCheckPoint(GameObject* crashBandicoot, DirectX::XMFLOAT3 cratePos = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f });

	CrateCheckPoint(const CrateCheckPoint& other) = delete;
	CrateCheckPoint(CrateCheckPoint&& other) noexcept = delete;
	CrateCheckPoint& operator=(const CrateCheckPoint& other) = delete;
	CrateCheckPoint& operator=(CrateCheckPoint&& other) noexcept = delete;
	virtual ~CrateCheckPoint() = default;
	void RemoveAllObjects();
protected:
	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;

private:
	DirectX::XMFLOAT3 m_Pos;
	GameObject* m_pTriggerTop, *m_pTriggerBox, *m_pNormalBox,*m_pCheckPointBox;
	ColliderComponent* m_ColliderTopTrigger, *m_ColliderBoxTrigger;
	bool m_IsTriggeredTop, m_IsPlayerInTrigger, m_IsSpinning;
	GameObject* m_pCrashBandicoot;
	FMOD::Sound * m_pSound;
	SoundManager* m_pSoundManager;
};

