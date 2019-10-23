#include "stdafx.h"
#include "PlayerInventory.h"


PlayerInventory::PlayerInventory()
{

}

PlayerInventory::~PlayerInventory()
{
}

void PlayerInventory::Initialize()
{
	m_Lives = 3;
	m_SmashedBoxes = 0;
	m_WumpaFruits = 0;
	m_TotalBoxes = 99;
	m_HasCrystal = false;
	m_HasPlayerDied = false;

	m_pSoundManager = SoundManager::GetInstance();
	m_pSoundManager->GetSystem()->createSound("./Resources/Game/Sound/SFX/LifeSound.mp3", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pSoundLife);
}

void PlayerInventory::SetHealth(int health)
{
	if (m_Lives > 0)
	{
		m_Lives += health;
	}
	else
	{
		m_GameOver = true;
	}
	
}

int PlayerInventory::GetHealth()
{
	return m_Lives;
}

void PlayerInventory::SetWumpaFruits(int wumpaFruits)
{
	if (m_WumpaFruits < 99)
	{
		m_WumpaFruits += wumpaFruits;
	}
	else
	{
		m_pSoundManager->GetSystem()->playSound(m_pSoundLife, 0, false, 0);
		++m_Lives;
		m_WumpaFruits = 0;
	}

}

int PlayerInventory::GetWumpaFruits()
{
	return m_WumpaFruits;
}

void PlayerInventory::SetSmashedBoxes(int smashedBoxes)
{

		m_SmashedBoxes += smashedBoxes;
}

int PlayerInventory::GetSmashedBoxes()
{
	return m_SmashedBoxes;
}

void PlayerInventory::SetMaxBoxes(int maxBoxes)
{
	m_TotalBoxes = maxBoxes;
}

int PlayerInventory::GetMaxBoxes()
{
	return m_TotalBoxes;
}

bool PlayerInventory::GetCrystalStatus()
{
	return m_HasCrystal;
}

void PlayerInventory::SetCrystalStatus(bool hasCrystal)
{
	m_HasCrystal = hasCrystal;
}

bool PlayerInventory::GetGemStatus()
{
	return m_HasGem;
}

void PlayerInventory::SetGemStatus(bool hasGem)
{
	m_HasGem = hasGem;
}

bool PlayerInventory::IsPlayerDead()
{
	return m_HasPlayerDied;
}

void PlayerInventory::SetPlayerDead(bool isPlayerDead)
{
	m_HasPlayerDied = isPlayerDead;
}

void PlayerInventory::SetGameOver(bool isGameOver)
{
	m_GameOver = isGameOver;
}

bool PlayerInventory::IsGameOver()
{
	return m_GameOver;
}
