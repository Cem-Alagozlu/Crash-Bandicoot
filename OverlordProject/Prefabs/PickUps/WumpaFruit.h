#pragma once
#include "GameObject.h"
#include "ModelComponent.h"
#include "SoundManager.h"

class WumpaFruit final : virtual public GameObject
{
public:
	WumpaFruit(GameObject* crashBandicoot,DirectX::XMFLOAT3 wumpaPosition = DirectX::XMFLOAT3{0.0f,0.0f,0.0f});

	WumpaFruit(const WumpaFruit& other) = delete;
	WumpaFruit(WumpaFruit&& other) noexcept = delete;
	WumpaFruit& operator=(const WumpaFruit& other) = delete;
	WumpaFruit& operator=(WumpaFruit&& other) noexcept = delete;
	virtual ~WumpaFruit() = default;

protected:
	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;
private:
	ModelComponent* m_pWumpaFruit;
	FMOD::Sound * m_pSoundTaken,*m_pSoundRemoved;
	SoundManager* m_pSoundManager;
	DirectX::XMFLOAT3 m_Pos;
	GameObject* m_pCrashBandicoot;
	bool m_IsTaken,m_IsRemoved;
};

