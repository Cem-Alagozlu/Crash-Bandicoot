#include "stdafx.h"
#include "MenuExtra.h"
#include "GameObject.h"
#include "SpriteComponent.h"
#include "ContentManager.h"
#include "TextRenderer.h"
#include "Components.h"
#include "SceneManager.h"

MenuExtra::CurrentExtraMenu MenuExtra::m_CurrentMenuState{ CurrentExtraMenu::main };

MenuExtra::MenuExtra()
	:GameScene{ L"MenuExtra" }
{
	m_MenuExtraIndex = 2;
	m_CurrentMenuState = CurrentExtraMenu::main;
	m_UsingGamepad = false;
}

void MenuExtra::Initialize()
{
	auto gameContext = GetGameContext();

	for (size_t i = 0; i < 4; i++)
	{
		m_pBackgrounds.push_back(new GameObject());
	}

	m_pBackgrounds[0]->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Menu/Main/Extra_Menu.png"));
	m_pBackgrounds[1]->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Menu/Main/Extra_Story_Menu.png"));
	m_pBackgrounds[2]->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Menu/Main/Extra_Information_Menu.png"));
	m_pBackgrounds[3]->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Menu/Main/Extra_Credits_Menu.png"));

	for (size_t i = 0; i < 4; i++)
	{
		m_pBackgrounds[i]->GetTransform()->Translate(0.0f, 0.0f, 0.96f);
		AddChild(m_pBackgrounds[i]) ;
	}

	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/Game/UI/Font/font00.fnt");
	m_YellowColor = DirectX::XMFLOAT4(251.0f / 255.0f, 224.0f / 255.0f, 2.0f / 255.0f, 1.0f);
	m_RedColor = DirectX::XMFLOAT4(220.0f / 255.0f, 20.0f / 255.0f, 60.0f / 255.0f, 1.0f);


	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::decrement,InputTriggerState::Pressed,VK_DOWN,-1,XINPUT_GAMEPAD_DPAD_DOWN });
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::increment,InputTriggerState::Pressed,VK_UP,-1,XINPUT_GAMEPAD_DPAD_UP });
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::confirm,InputTriggerState::Pressed,VK_RETURN,-1,XINPUT_GAMEPAD_A });
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::goBack,InputTriggerState::Pressed,-1,-1,XINPUT_GAMEPAD_B });
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::confirmMouse,InputTriggerState::Pressed,-1,VK_LBUTTON });
}

void MenuExtra::Update()
{
	HandleInputGamepad();
	HandleMouseInput();
}

void MenuExtra::Draw()
{
	switch (m_CurrentMenuState)
	{
	case  MenuExtra::CurrentExtraMenu::main:
		switch (m_MenuExtraIndex)
		{
		case 0:
			TextRenderer::GetInstance()->DrawText(m_pFont, L"Credits", DirectX::XMFLOAT2(525, 530), m_RedColor);
			break;
		case 1:
			TextRenderer::GetInstance()->DrawText(m_pFont, L"Info", DirectX::XMFLOAT2(570, 350), m_RedColor);
			break;
		case 2:
			TextRenderer::GetInstance()->DrawText(m_pFont, L"Story", DirectX::XMFLOAT2(550, 178), m_RedColor);
			break;
		}
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Story", DirectX::XMFLOAT2(550, 178), m_YellowColor);
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Info", DirectX::XMFLOAT2(570, 350), m_YellowColor);
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Credits", DirectX::XMFLOAT2(525, 530), m_YellowColor);
		break;
	case  MenuExtra::CurrentExtraMenu::story:
		break;
	case  MenuExtra::CurrentExtraMenu::information:
		break;
	case  MenuExtra::CurrentExtraMenu::credits:
		break;
	}

	if (m_MenuExtraIndex == 3)
	{
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Back", DirectX::XMFLOAT2(110, 60), m_RedColor);
	}

	TextRenderer::GetInstance()->DrawText(m_pFont, L"Back", DirectX::XMFLOAT2(110, 60), m_YellowColor);
}


void MenuExtra::HandleInputGamepad()
{
	auto gameContext = GetGameContext();
	if (gameContext.pInput->IsActionTriggered(ButtonIndex::decrement) && m_CurrentMenuState == CurrentExtraMenu::main)
	{
		m_UsingGamepad = true;
		--m_MenuExtraIndex;
		if (m_MenuExtraIndex < 0)
		{
			m_MenuExtraIndex = 3;
		}
	}
	else if (gameContext.pInput->IsActionTriggered(ButtonIndex::increment) && m_CurrentMenuState == CurrentExtraMenu::main)
	{
		m_UsingGamepad = true;
		++m_MenuExtraIndex;
		if (m_MenuExtraIndex > 3)
		{
			m_MenuExtraIndex = 0;
		}
	}

	switch (m_CurrentMenuState)
	{
	case MenuExtra::CurrentExtraMenu::main:
		if (gameContext.pInput->IsActionTriggered(ButtonIndex::goBack))
		{
			SceneManager::GetInstance()->SetActiveGameScene(L"MenuMain");
		}
		switch (m_MenuExtraIndex)
		{
		case 0:
			if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirm))
			{
				m_CurrentMenuState = CurrentExtraMenu::credits;
			}
			break;
		case 1:
			if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirm))
			{
				m_CurrentMenuState = CurrentExtraMenu::information;
			}
			break;
		case 2:
			if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirm))
			{
				m_CurrentMenuState = CurrentExtraMenu::story;
			}
			break;
		case 3:
			if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirm))
			{
				SceneManager::GetInstance()->SetActiveGameScene(L"MenuMain");
			}
			break;
		}
		break;
	case MenuExtra::CurrentExtraMenu::story:
		m_MenuExtraIndex = 3;
		m_pBackgrounds[1]->GetTransform()->Translate(0.0f, 0.0f, 0.94f);
		if (gameContext.pInput->IsActionTriggered(ButtonIndex::goBack)
			|| gameContext.pInput->IsActionTriggered(ButtonIndex::confirm))
		{
			TransformStory();
		}
		break;
	case MenuExtra::CurrentExtraMenu::information:
		m_MenuExtraIndex = 3;
		m_pBackgrounds[2]->GetTransform()->Translate(0.0f, 0.0f, 0.94f);
		if (gameContext.pInput->IsActionTriggered(ButtonIndex::goBack)
			|| gameContext.pInput->IsActionTriggered(ButtonIndex::confirm))
		{
			TransformInformation();
		}
		break;
	case MenuExtra::CurrentExtraMenu::credits:
		m_MenuExtraIndex = 3;
		m_pBackgrounds[3]->GetTransform()->Translate(0.0f, 0.0f, 0.94f);
		if (gameContext.pInput->IsActionTriggered(ButtonIndex::goBack)
			|| gameContext.pInput->IsActionTriggered(ButtonIndex::confirm))
		{
			TransformCredits();
		}
		break;
	}


}

void MenuExtra::HandleMouseInput()
{
	auto gameContext = GetGameContext();
	float xPos = float(gameContext.pInput->GetMousePosition().x);
	float yPos = float(gameContext.pInput->GetMousePosition().y);


	switch (m_CurrentMenuState)
	{
	case MenuExtra::CurrentExtraMenu::main:
		if (yPos <= 150 && yPos >= 40 && xPos <= 335 && xPos >= 25 && !m_UsingGamepad)
		{
			m_MenuExtraIndex = 3;
			if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
			{
				SceneManager::GetInstance()->SetActiveGameScene(L"MenuMain");
			}
		}

		if (xPos >= 485 && xPos <= 800 && !m_UsingGamepad)
		{
			if (yPos <= 265 && yPos >= 165)
			{
				m_MenuExtraIndex = 2;
				if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
				{
					m_CurrentMenuState = CurrentExtraMenu::story;
				}
			}

			if (yPos <= 440 && yPos >= 340)
			{
				m_MenuExtraIndex = 1;
				if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
				{
					m_CurrentMenuState = CurrentExtraMenu::information;
				}
			}

			if (yPos <= 620 && yPos >= 520)
			{
				m_MenuExtraIndex = 0;
				if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
				{
					m_CurrentMenuState = CurrentExtraMenu::credits;
				}
			}
		}
		break;
	case MenuExtra::CurrentExtraMenu::story:
		GoBackToMain(xPos, yPos);
		break;
	case MenuExtra::CurrentExtraMenu::information:
		GoBackToMain(xPos, yPos);
		break;
	case MenuExtra::CurrentExtraMenu::credits:
		GoBackToMain(xPos, yPos);
		break;
	}

	if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
	{
		m_UsingGamepad = false;
	}
}

void MenuExtra::GoBackToMain(float x, float y)
{
	auto gameContext = GetGameContext();
	if (y <= 150 && y >= 40 && x <= 335 && x >= 25 && !m_UsingGamepad)
	{
		if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
		{
			switch (m_CurrentMenuState)
			{
			case MenuExtra::CurrentExtraMenu::story:
				TransformStory();
				break;
			case MenuExtra::CurrentExtraMenu::information:
				TransformInformation();
				break;
			case MenuExtra::CurrentExtraMenu::credits:
				TransformCredits();
				break;
			}
			m_CurrentMenuState = CurrentExtraMenu::main;
		}
	}
}

void MenuExtra::TransformCredits()
{
	m_pBackgrounds[3]->GetTransform()->Translate(0.0f, 0.0f, 1.0f);
	m_MenuExtraIndex = 0;
	m_CurrentMenuState = CurrentExtraMenu::main;
}

void MenuExtra::TransformInformation()
{
	m_pBackgrounds[2]->GetTransform()->Translate(0.0f, 0.0f, 1.0f);
	m_MenuExtraIndex = 1;
	m_CurrentMenuState = CurrentExtraMenu::main;
}

void MenuExtra::TransformStory()
{
	m_pBackgrounds[1]->GetTransform()->Translate(0.0f, 0.0f, 1.0f);
	m_MenuExtraIndex = 2;
	m_CurrentMenuState = CurrentExtraMenu::main;
}
