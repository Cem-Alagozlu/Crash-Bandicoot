#pragma once
#include "GameScene.h"
#include "SpriteFont.h"

class MenuExtra : public GameScene
{
public:
	enum class CurrentExtraMenu
	{
		main,
		story,
		information,
		credits
	};
	enum ButtonIndex : UINT
	{
		increment = 0,
		decrement,
		goBack,
		confirm,
		confirmMouse
	};

	MenuExtra();
	virtual ~MenuExtra() = default;

	void HandleInputGamepad();
	void HandleMouseInput();
	void GoBackToMain(float x, float y);

	void TransformCredits();
	void TransformInformation();
	void TransformStory();

	MenuExtra(const MenuExtra& other) = delete;
	MenuExtra(MenuExtra&& other) noexcept = delete;
	MenuExtra& operator=(const MenuExtra& other) = delete;
	MenuExtra& operator=(MenuExtra&& other) noexcept = delete;

protected:
	void Initialize() override;

	void Update() override;
	void Draw() override;

private:
	std::vector<GameObject*> m_pBackgrounds;
	static CurrentExtraMenu m_CurrentMenuState;
	SpriteFont* m_pFont;
	bool m_UsingGamepad = false;
	int m_MenuExtraIndex;
	DirectX::XMFLOAT4 m_YellowColor, m_RedColor;
};

