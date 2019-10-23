#include "stdafx.h"
#include "PlayerHUD.h"
#include "SpriteComponent.h"
#include "ContentManager.h"
#include "TextRenderer.h"
#include "Components.h"
#include "PlayerInventory.h"

PlayerHUD::PlayerHUD()
{
}


PlayerHUD::~PlayerHUD()
{
}

void PlayerHUD::Initialize(const GameContext&)
{
	m_pMainIMG = new GameObject();
	m_pMainIMG->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Hud/playerHud.png"));
	m_pMainIMG->GetTransform()->Translate(0.0f, 0.0f, 0.95f);
	AddChild(m_pMainIMG);

	m_pYoudeadIMG = new GameObject();
	m_pYoudeadIMG->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Hud/dead.png"));
	m_pYoudeadIMG->GetTransform()->Translate(0.0f, 0.0f, 0.96f);
	AddChild(m_pYoudeadIMG);

	m_pGameOverIMG = new GameObject();
	m_pGameOverIMG->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Hud/gameover.png"));
	m_pGameOverIMG->GetTransform()->Translate(0.0f, 0.0f, 0.96f);
	AddChild(m_pGameOverIMG);

	m_pCrystalIMG = new GameObject();
	m_pCrystalIMG->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Hud/crystal.png"));
	m_pCrystalIMG->GetTransform()->Translate(0.0f, 0.0f, 0.97f);
	AddChild(m_pCrystalIMG);
	m_pCrystalIMG->SetActive(false);

	m_pGemIMG = new GameObject();
	m_pGemIMG->AddComponent(new SpriteComponent(L"./Resources/Game/UI/Hud/gem.png"));
	m_pGemIMG->GetTransform()->Translate(0.0f, 0.0f, 0.98f);
	AddChild(m_pGemIMG);
	m_pGemIMG->SetActive(false);
	

	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/Game/UI/Font/font00.fnt");
	m_YellowColor = DirectX::XMFLOAT4(251.0f / 255.0f, 94.0f / 255.0f, 2.0f / 255.0f, 1.0f);

	m_WumpaFruits = PlayerInventory::GetInstance()->GetWumpaFruits();
	m_MaxBoxes = PlayerInventory::GetInstance()->GetMaxBoxes();
	m_SmashedBoxes = PlayerInventory::GetInstance()->GetSmashedBoxes();
	m_Lives = PlayerInventory::GetInstance()->GetHealth();

	m_pSoundManager = SoundManager::GetInstance();
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/gem.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundGem);
}

void PlayerHUD::Draw(const GameContext&)
{
	if (!PlayerInventory::GetInstance()->IsPlayerDead() && !PlayerInventory::GetInstance()->IsGameOver())
	{
		TextRenderer::GetInstance()->DrawText(m_pFont, std::to_wstring(m_WumpaFruits), DirectX::XMFLOAT2(120, 10), m_YellowColor);
		TextRenderer::GetInstance()->DrawText(m_pFont, std::to_wstring(m_SmashedBoxes), DirectX::XMFLOAT2(680.0f, 10), m_YellowColor);
		TextRenderer::GetInstance()->DrawText(m_pFont, L"/", DirectX::XMFLOAT2(750, 10), m_YellowColor);
		TextRenderer::GetInstance()->DrawText(m_pFont, std::to_wstring(m_MaxBoxes), DirectX::XMFLOAT2(790, 10), m_YellowColor);
		TextRenderer::GetInstance()->DrawText(m_pFont, std::to_wstring(m_Lives), DirectX::XMFLOAT2(1180, 10), m_YellowColor);
	}
}

void PlayerHUD::Update(const GameContext& )
{
	if (PlayerInventory::GetInstance()->IsPlayerDead())
	{
		m_pMainIMG->SetActive(false);
		m_pGameOverIMG->SetActive(false);
		m_pYoudeadIMG->SetActive(true);
		m_pCrystalIMG->SetActive(false);
		m_pGemIMG->SetActive(false);
	}
	else
	{
		m_pGameOverIMG->SetActive(false);
		m_pMainIMG->SetActive(true);
		m_pYoudeadIMG->SetActive(false);
		m_pCrystalIMG->SetActive(m_Crystal);
		m_pGemIMG->SetActive(m_Gem);
	}

	if (PlayerInventory::GetInstance()->IsGameOver())
	{
		m_pMainIMG->SetActive(false);
		m_pYoudeadIMG->SetActive(false);
		m_pGameOverIMG->SetActive(true);
		m_pCrystalIMG->SetActive(false);
		m_pGemIMG->SetActive(false);
	}

	m_WumpaFruits = PlayerInventory::GetInstance()->GetWumpaFruits();
	m_MaxBoxes = PlayerInventory::GetInstance()->GetMaxBoxes();
	m_SmashedBoxes = PlayerInventory::GetInstance()->GetSmashedBoxes();
	m_Lives = PlayerInventory::GetInstance()->GetHealth();
	m_Crystal = PlayerInventory::GetInstance()->GetCrystalStatus();

	if (m_SmashedBoxes == m_MaxBoxes && !m_Gem)
	{
		m_Gem = true;
		m_pSoundManager->GetSystem()->playSound(m_pSoundGem, 0, false, 0);
	}
}
