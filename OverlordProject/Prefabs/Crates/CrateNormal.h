#pragma once
#include "GameObject.h"
#include "ColliderComponent.h"
#include "SoundManager.h"


class CrateNormal final : public GameObject
{
public:
	CrateNormal(GameObject* crashBandicoot, DirectX::XMFLOAT3 cratePos = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f });

	CrateNormal(const CrateNormal& other) = delete;
	CrateNormal(CrateNormal&& other) noexcept = delete;
	CrateNormal& operator=(const CrateNormal& other) = delete;
	CrateNormal& operator=(CrateNormal&& other) noexcept = delete;
	virtual ~CrateNormal() = default ;

	void SetWumpaFruits();
	void RemoveAllObjects();
protected:
	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;
private:
	DirectX::XMFLOAT3 m_Pos;
	GameObject* m_pTriggerTop, *m_pTriggerBox,*m_pNormalBox;
	ColliderComponent* m_ColliderTopTrigger,*m_ColliderBoxTrigger;
	bool m_IsTriggeredTop, m_IsSpinning, m_IsPlayerInTrigger;
	GameObject* m_pCrashBandicoot;
	FMOD::Sound * m_pSound;
	SoundManager* m_pSoundManager;
	std::vector<GameObject*> m_pWumpaFruits;
};

