#include "stdafx.h"
#include "ButtonComponent.h"
#include "GameObject.h"	
#include "SpriteComponent.h"
#include "Components.h"

ButtonComponent::ButtonComponent(DirectX::XMFLOAT3 pos, bool visible)
	:m_ButtonPos{ pos },
	m_IsVisible{ visible }
{
	//load button texture
	m_pButton = new GameObject();
	m_pButton->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Menu/Buttons/Button_Menu.png"));
	AddChild(m_pButton);
}

void ButtonComponent::Draw()
{
	if (m_IsVisible)
	{
		m_pButton->GetTransform()->Translate(m_ButtonPos);
	}
	else
	{
		m_pButton->GetTransform()->Scale(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	}
}

void ButtonComponent::Update()
{
}
