#include "stdafx.h"
#include "MenuQuit.h"
#include "GameObject.h"
#include "SpriteComponent.h"
#include "ContentManager.h"
#include "Components.h"
#include "TextRenderer.h"
#include "SceneManager.h"


MenuQuit::MenuQuit()
	:GameScene{ L"MenuQuit" }
{
	m_MenuMainIndex = 2;
}



void MenuQuit::Initialize()
{
	auto gameContext = GetGameContext();

#pragma region Background
	//load background
	m_pBackground = new GameObject();
	m_pBackground->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Menu/Main/Quit_Menu.png"));
	m_pBackground->GetTransform()->Translate(0.0f, 0.0f, 0.96f);
	AddChild(m_pBackground);
#pragma endregion

#pragma region FontInits
	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/Game/UI/Font/font00.fnt");
	m_YellowColor = DirectX::XMFLOAT4(251.0f / 255.0f, 224.0f / 255.0f, 2.0f / 255.0f, 1.0f);
	m_RedColor = DirectX::XMFLOAT4(220.0f / 255.0f, 20.0f / 255.0f, 60.0f / 255.0f, 1.0f);
#pragma endregion 

#pragma region InputDesc
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::decrement,InputTriggerState::Pressed,VK_DOWN,-1,XINPUT_GAMEPAD_DPAD_LEFT });
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::increment,InputTriggerState::Pressed,VK_UP,-1,XINPUT_GAMEPAD_DPAD_RIGHT });
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::goBack,InputTriggerState::Pressed,-1,-1,XINPUT_GAMEPAD_B });
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::confirm,InputTriggerState::Pressed,VK_RETURN,-1,XINPUT_GAMEPAD_A });
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::confirmMouse,InputTriggerState::Pressed,-1,VK_LBUTTON });
#pragma endregion 
}

void MenuQuit::Update()
{
	HandleInputMouse();
	HandleInputGamepad();
}

void MenuQuit::Draw()
{
	switch (m_MenuMainIndex)
	{
	case 0:
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Yes", DirectX::XMFLOAT2(380, 520), m_RedColor);
		break;
	case 1:
		TextRenderer::GetInstance()->DrawText(m_pFont, L"No", DirectX::XMFLOAT2(800, 520), m_RedColor);;
		break;
	}
	TextRenderer::GetInstance()->DrawText(m_pFont, L"No", DirectX::XMFLOAT2(800, 520), m_YellowColor);
	TextRenderer::GetInstance()->DrawText(m_pFont, L"Yes", DirectX::XMFLOAT2(380, 520), m_YellowColor);
}

void MenuQuit::GoBackMain()
{
	SceneManager::GetInstance()->SetActiveGameScene(L"MenuMain");
}

void MenuQuit::HandleInputMouse()
{
	auto gameContext = GetGameContext();
	float xPos = float(gameContext.pInput->GetMousePosition().x);
	float yPos = float(gameContext.pInput->GetMousePosition().y);

	if (yPos >= 500 && yPos <= 600)
	{
		if (xPos <= 590 && xPos >= 295)
		{
			m_MenuMainIndex = 0;
			if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
			{
				PostQuitMessage(0);
			}
		}
		else if (xPos <= 995 && xPos >= 695)
		{
			m_MenuMainIndex = 1;
			if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
			{
				GoBackMain();
			}
		}
	}
}

void MenuQuit::HandleInputGamepad()
{
	auto gameContext = GetGameContext();
	if (gameContext.pInput->IsActionTriggered(ButtonIndex::decrement))
	{
		--m_MenuMainIndex;
		if (m_MenuMainIndex < 0)
		{
			m_MenuMainIndex = 1;
		}
	}
	else if (gameContext.pInput->IsActionTriggered(ButtonIndex::increment))
	{
		++m_MenuMainIndex;
		if (m_MenuMainIndex > 1)
		{
			m_MenuMainIndex = 0;
		}
	}


	switch (m_MenuMainIndex)
	{
	case 0:
		if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirm))
		{
			PostQuitMessage(0);
		}
		break;
	case 1:
		if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirm))
		{
			GoBackMain();
		}
		break;
	default:
		break;
	};

	if (gameContext.pInput->IsActionTriggered(ButtonIndex::goBack))
	{
		GoBackMain();
	}
}
