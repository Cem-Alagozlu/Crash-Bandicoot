#pragma once
#include "GameObject.h"
class ButtonComponent final : public GameObject
{
public:
	explicit ButtonComponent(DirectX::XMFLOAT3 pos, bool visible);
	~ButtonComponent() = default;


	void Draw();
	void Update();



	ButtonComponent(const ButtonComponent& other) = delete;
	ButtonComponent(ButtonComponent&& other) noexcept = delete;
	ButtonComponent& operator=(const ButtonComponent& other) = delete;
	ButtonComponent& operator=(ButtonComponent&& other) noexcept = delete;

private:
	GameObject* m_pButton;
	bool m_IsVisible;
	DirectX::XMFLOAT3 m_ButtonPos;
};

