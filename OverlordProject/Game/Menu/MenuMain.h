#pragma once
#include "GameScene.h"
#include "SpriteFont.h"
#include "ButtonComponent.h"
#include "SoundManager.h"

class MenuMain : public GameScene
{
public:
	enum ButtonIndex : UINT
	{
	increment = 0,
	decrement,
	confirm,
	confirmMouse
	};

	MenuMain();
	virtual ~MenuMain() = default;

	void HandleInputMouse();
	void HandleInputGamepad();
	void MenuQuit();
	void MenuExtra();

	MenuMain(const MenuMain& other) = delete;
	MenuMain(MenuMain&& other) noexcept = delete;
	MenuMain& operator=(const MenuMain& other) = delete;
	MenuMain& operator=(MenuMain&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	FMOD::Sound * m_pSongMenu, *m_pSongLevel;
	SoundManager* m_pSoundManager;
	GameObject* m_pBackground, *m_pPlayIMG;
	SpriteFont* m_pFont;
	DirectX::XMFLOAT4 m_YellowColor, m_RedColor;
	int m_MenuMainIndex,m_PlayIndex;
	bool m_UsingGamepad;
};

