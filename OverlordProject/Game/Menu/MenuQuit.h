#pragma once
#include "GameScene.h"
#include "SpriteFont.h"
#include "ButtonComponent.h"

class MenuQuit : public GameScene
{
public:
	enum ButtonIndex : UINT
	{
		increment = 0,
		decrement,
		goBack,
		confirm,
		confirmMouse
	};

	MenuQuit();
	virtual ~MenuQuit() = default;

	void GoBackMain();
	void HandleInputMouse();
	void HandleInputGamepad();

	MenuQuit(const MenuQuit& other) = delete;
	MenuQuit(MenuQuit&& other) noexcept = delete;
	MenuQuit& operator=(const MenuQuit& other) = delete;
	MenuQuit& operator=(MenuQuit&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	GameObject* m_pBackground;
	SpriteFont* m_pFont;
	int m_MenuMainIndex;
	DirectX::XMFLOAT4 m_YellowColor, m_RedColor;
};

