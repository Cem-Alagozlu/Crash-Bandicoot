#pragma once
#include "GameObject.h"
#include "SoundManager.h"

class Crystal final : public GameObject
{
public:
	Crystal(DirectX::XMFLOAT3 crystalPos = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f });

	Crystal(const Crystal& other) = delete;
	Crystal(Crystal&& other) noexcept = delete;
	Crystal& operator=(const Crystal& other) = delete;
	Crystal& operator=(Crystal&& other) noexcept = delete;
	virtual ~Crystal() = default;

protected:
	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;
private:
	DirectX::XMFLOAT3 m_Pos;
	ModelComponent* m_pCrystal;
	FMOD::Sound * m_pSound;
	SoundManager* m_pSoundManager;
	bool m_IsTaken;
};

