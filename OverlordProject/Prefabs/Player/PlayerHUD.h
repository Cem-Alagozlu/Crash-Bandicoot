#pragma once
#include "GameObject.h"
#include "SpriteFont.h"
#include "SoundManager.h"

class PlayerHUD final : public GameObject
{
public:
	PlayerHUD();
	~PlayerHUD();

protected:
	virtual void Initialize(const GameContext&) override;
	virtual void Draw(const GameContext&) override;
	virtual void Update(const GameContext&) override;

private:
	GameObject* m_pMainIMG, *m_pYoudeadIMG, *m_pGameOverIMG,*m_pCrystalIMG,*m_pGemIMG;
	SpriteFont* m_pFont;
	DirectX::XMFLOAT4 m_YellowColor;
	bool m_Crystal;
	bool m_Gem;
	int m_Lives;
	int m_WumpaFruits;
	int m_MaxBoxes;
	int m_SmashedBoxes;

	FMOD::Sound * m_pSoundGem;
	SoundManager* m_pSoundManager;
};

