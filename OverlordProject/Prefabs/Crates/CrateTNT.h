#pragma once
#include "GameObject.h"
#include "ColliderComponent.h"
#include "SoundManager.h"

class CrateTNT final : public GameObject
{
public:
	CrateTNT(GameObject* crashBandicoot, DirectX::XMFLOAT3 cratePos = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f });
	CrateTNT(const CrateTNT& other) = delete;
	CrateTNT(CrateTNT&& other) noexcept = delete;
	CrateTNT& operator=(const CrateTNT& other) = delete;
	CrateTNT& operator=(CrateTNT&& other) noexcept = delete;
	virtual ~CrateTNT() = default;
	void RemoveAllObjects();

protected:
	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;

private:
	DirectX::XMFLOAT3 m_Pos;
	GameObject* m_pTriggerTop, *m_pNormalBox,*m_pBlastRadius;
	ColliderComponent* m_ColliderTopTrigger,*m_ColliderBlastRadiusTrigger;
	bool m_IsTriggeredTop, m_IsSpinning, m_IsPlayerInRadius;
	GameObject* m_pCrashBandicoot;
	FMOD::Sound * m_pSound,*m_pSoundBOOM,*m_pSoundDead;
	SoundManager* m_pSoundManager;
	float m_BlowUpTimer;

};

