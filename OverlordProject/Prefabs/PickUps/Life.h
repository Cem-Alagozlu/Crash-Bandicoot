#pragma once
#include "GameObject.h"
#include "SoundManager.h"

class Life final : public GameObject
{
public:
	Life(GameObject* crashBandicoot,DirectX::XMFLOAT3 lifePosition = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f });

	Life(const Life& other) = delete;
	Life(Life&& other) noexcept = delete;
	Life& operator=(const Life& other) = delete;
	Life& operator=(Life&& other) noexcept = delete;
	virtual ~Life() = default;

protected:
	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;

private:
	DirectX::XMFLOAT3 m_Pos;
	ModelComponent* m_pLife;
	FMOD::Sound * m_pSoundTaken, *m_pSoundRemoved;
	GameObject* m_pCrashBandicoot;
	SoundManager* m_pSoundManager;
	bool m_IsTaken, m_IsRemoved;
};

