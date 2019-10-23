#pragma once
#include "GameObject.h"
#include "SoundManager.h"
#include "ColliderComponent.h"

class CrateJump final : public GameObject
{
public:
	CrateJump(GameObject* crashBandicoot, DirectX::XMFLOAT3 cratePos = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f });


	CrateJump(const CrateJump& other) = delete;
	CrateJump(CrateJump&& other) noexcept = delete;
	CrateJump& operator=(const CrateJump& other) = delete;
	CrateJump& operator=(CrateJump&& other) noexcept = delete;
	virtual ~CrateJump() = default;

	void RemoveAllObjects();
protected:
	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;
private:
	DirectX::XMFLOAT3 m_Pos;
	GameObject* m_pTriggerTop, *m_pTriggerBox, *m_pNormalBox;
	ColliderComponent* m_ColliderTopTrigger, *m_ColliderBoxTrigger;
	bool m_IsTriggeredTop, m_HasSmashed, m_IsSpinning;
	GameObject* m_pCrashBandicoot;
	FMOD::Sound * m_pSoundBreak,*m_pSoundJump;
	SoundManager* m_pSoundManager;
};

