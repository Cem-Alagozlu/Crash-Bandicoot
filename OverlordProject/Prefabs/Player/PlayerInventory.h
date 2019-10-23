#pragma once
#include "Singleton.h"
#include "SoundManager.h"

class PlayerInventory final : public Singleton<PlayerInventory>
{
public:
	PlayerInventory();
	~PlayerInventory() ;

	void Initialize();

	PlayerInventory(const PlayerInventory& other) = delete;
	PlayerInventory(PlayerInventory&& other) noexcept = delete;
	PlayerInventory& operator=(const PlayerInventory& other) = delete;
	PlayerInventory& operator=(PlayerInventory&& other) noexcept = delete;

	void SetHealth(int health);
	int GetHealth();

	void SetWumpaFruits(int wumpaFruits);
	int GetWumpaFruits();

	void SetSmashedBoxes(int smashedBoxes);
	int GetSmashedBoxes();

	void SetMaxBoxes(int maxBoxes);
	int GetMaxBoxes();

	bool GetCrystalStatus();
	void SetCrystalStatus(bool hasCrystal);

	bool GetGemStatus();
	void SetGemStatus(bool hasGem);

	bool IsPlayerDead();
	void SetPlayerDead(bool isPlayerDead);

	void SetGameOver(bool isGameOver);
	bool IsGameOver();

private:
	int m_Lives;
	int m_WumpaFruits;
	int m_SmashedBoxes;
	int m_TotalBoxes;
	bool m_HasPlayerDied;
	bool m_HasCrystal;
	bool m_GameOver;
	bool m_HasGem;

	FMOD::Sound * m_pSoundLife;
	SoundManager* m_pSoundManager;
};

