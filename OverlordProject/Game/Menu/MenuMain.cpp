#include "stdafx.h"
#include "MenuMain.h"
#include "GameObject.h"
#include "SpriteComponent.h"
#include "ContentManager.h"
#include "Components.h"
#include "SceneManager.h"
#include "TextRenderer.h"
#include "SoundManager.h"

MenuMain::MenuMain()
	:GameScene{ L"MenuMain" }
{
	m_MenuMainIndex = 3;
	m_PlayIndex = 0;
	m_UsingGamepad = false;
}

void MenuMain::Initialize()
{
	auto gameContext = GetGameContext();

#pragma region Background
	m_pBackground = new GameObject();
	m_pBackground->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Menu/Main/Main_Menu.png"));
	m_pBackground->GetTransform()->Translate(0.0f, 0.0f, 0.95f);
	AddChild(m_pBackground);

	m_pPlayIMG = new GameObject();
	m_pPlayIMG->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Menu/Main/play01.png"));
	m_pPlayIMG->GetTransform()->Translate(0.0f, 0.0f, 0.96f);
	AddChild(m_pPlayIMG);
	m_pPlayIMG->SetActive(false);
#pragma endregion 

#pragma region FontInits
	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/Game/UI/Font/font00.fnt");
	m_YellowColor = DirectX::XMFLOAT4(251.0f / 255.0f, 224.0f / 255.0f, 2.0f / 255.0f, 1.0f);
	m_RedColor = DirectX::XMFLOAT4(220.0f / 255.0f, 20.0f / 255.0f, 60.0f / 255.0f, 1.0f);
#pragma endregion 

#pragma region InputDesc
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::decrement,InputTriggerState::Pressed,VK_DOWN,-1,XINPUT_GAMEPAD_DPAD_DOWN });
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::increment,InputTriggerState::Pressed,VK_UP,-1,XINPUT_GAMEPAD_DPAD_UP });
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::confirm,InputTriggerState::Pressed,VK_RETURN,-1,XINPUT_GAMEPAD_A });
	gameContext.pInput->AddInputAction(InputAction{ ButtonIndex::confirmMouse,InputTriggerState::Pressed,-1,VK_LBUTTON });
#pragma endregion
	m_pSoundManager = SoundManager::GetInstance();
	m_pSoundManager->GetSystem()->createStream("./Resources/Game/Sound/Music/sound01.wav", FMOD_LOOP_NORMAL | FMOD_2D, 0, &m_pSongMenu);
	m_pSoundManager->GetSystem()->playSound(m_pSongMenu,0,false,0);


};

void MenuMain::Update()
{

	HandleInputGamepad();
	HandleInputMouse();
}

void MenuMain::Draw()
{
	if (m_PlayIndex < 1)
	{

		switch (m_MenuMainIndex)
		{
		case 0:
			TextRenderer::GetInstance()->DrawText(m_pFont, L"Exit", DirectX::XMFLOAT2(575, 500), m_RedColor);
			break;
		case 1:
			TextRenderer::GetInstance()->DrawText(m_pFont, L"Extra", DirectX::XMFLOAT2(552, 400), m_RedColor);
			break;
		case 2:
			TextRenderer::GetInstance()->DrawText(m_pFont, L"Play", DirectX::XMFLOAT2(570, 300), m_RedColor);
			break;
		default:
			break;
		}

		TextRenderer::GetInstance()->DrawText(m_pFont, L"Exit", DirectX::XMFLOAT2(575, 500), m_YellowColor);
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Extra", DirectX::XMFLOAT2(552, 400), m_YellowColor);
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Play", DirectX::XMFLOAT2(570, 300), m_YellowColor);
	}
	else
	{
		m_pBackground->SetActive(false);
		m_pPlayIMG->SetActive(true);
	}


}

void MenuMain::HandleInputMouse()
{
	auto gameContext = GetGameContext();
	float xPos = float(gameContext.pInput->GetMousePosition().x);
	float yPos = float(gameContext.pInput->GetMousePosition().y);


	if (xPos >= 520 && xPos <= 770 && !m_UsingGamepad)
	{
		if (yPos <= 380 && yPos >= 300)
		{
			m_MenuMainIndex = 2;
			if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
			{
				if (m_PlayIndex <=1)
				{
					++m_PlayIndex;
				}
				if (m_PlayIndex == 2)
				{
					m_pSongMenu->release();
					SceneManager::GetInstance()->SetActiveGameScene(L"Level");
				}
			}

		}
		else if (yPos <= 480 && yPos >= 400  && !m_PlayIndex == 1)
		{
			m_MenuMainIndex = 1;
			if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
			{
				MenuExtra();
			}
		}
		else if (yPos <= 580 && yPos >= 500 && !m_PlayIndex == 1)
		{
			m_MenuMainIndex = 0;
			if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
			{
				MenuQuit();
			}
		}
	}

	if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirmMouse))
	{
		m_UsingGamepad = false;
	}
}

void MenuMain::HandleInputGamepad()
{
	auto gameContext = GetGameContext();

	switch (m_MenuMainIndex)
	{
	case 0:
		if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirm) &&!m_PlayIndex == 1)
		{
			MenuQuit();
		}
		break;
	case 1:
		if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirm) && !m_PlayIndex == 1)
		{
			MenuExtra();
		}
		break;
	case 2:
		if (gameContext.pInput->IsActionTriggered(ButtonIndex::confirm) )
		{
			if (m_PlayIndex <= 1)
			{
				++m_PlayIndex;
			}
			if (m_PlayIndex == 2)
			{
				m_pSongMenu->release();
				SceneManager::GetInstance()->SetActiveGameScene(L"Level");
			}
		}
		break;
	}

	if (gameContext.pInput->IsActionTriggered(ButtonIndex::decrement))
	{
		if (m_PlayIndex == 0)
		{
			m_UsingGamepad = true;
			--m_MenuMainIndex;
			if (m_MenuMainIndex < 0 )
			{
				m_MenuMainIndex = 2;
			}
		}
	}
	else if (gameContext.pInput->IsActionTriggered(ButtonIndex::increment))
	{
		if (m_PlayIndex == 0)
		{
			m_UsingGamepad = true;
			++m_MenuMainIndex;
			if (m_MenuMainIndex > 2)
			{
				m_MenuMainIndex = 0;
			}
		}
	}
}

void MenuMain::MenuQuit()
{
	SceneManager::GetInstance()->SetActiveGameScene(L"MenuQuit");
}

void MenuMain::MenuExtra()
{
	SceneManager::GetInstance()->SetActiveGameScene(L"MenuExtra");
}
