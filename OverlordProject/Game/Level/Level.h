#pragma once
#include "GameScene.h"
#include "../../Prefabs/PickUps/WumpaFruit.h"
#include "../../Prefabs/Character/CrashBandicoot.h"
#include "../../Prefabs/Crates/CrateNitro.h"
#include "../../Prefabs/Crates/CrateNormal.h"
#include "../../Prefabs/Crates/CrateTNT.h"
#include "../../Prefabs/Crates/CrateJump.h"
#include "../../Prefabs/Crates/CrateLife.h"
#include "../../Prefabs/Crates/CrateCheckPoint.h"
#include "ParticleEmitterComponent.h"

class Level final : public GameScene
{
public:
	Level();
	virtual ~Level() = default;

	void LevelLayout();
	void LevelExtras();
	void LevelCollision();
	void SetItemsPositions();
	void PitFallDead();
	void ChangeCam();


	Level(const Level& other) = delete;
	Level(Level&& other) noexcept = delete;
	Level& operator=(const Level& other) = delete;
	Level& operator=(Level&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	//ESSENTIALS
	ParticleEmitterComponent* m_pParticleEmitter;
	CrashBandicoot* m_pCharacter;
	GameObject* m_pPitfalls01,*m_pPitfalls02, *m_pPitfalls03,*m_pPitfalls04,*m_pEndingBox;
	SoundManager* m_pSoundManager;
	FMOD::Sound * m_pSoundFalling,*m_pSongGameOver,*m_pSongLevel;
	bool m_IsSongPlaying, m_StartedGame;

	//CAM ROT CHANGES
	FollowCam* m_pFollowCam;
	GameObject* m_pTriggerCam01, *m_pTriggerCam02, *m_pTriggerCam03, *m_pTriggerCam04;
	int m_CamIndex;
	float m_CamRotator;
	int m_TotalBoxes;

	DirectX::XMFLOAT3 m_ShadowPos{ };
	DirectX::XMFLOAT3 m_ShadowFor{};

	//CRATES
	std::vector<CrateNitro*> m_pCratesNitro;
	std::vector<DirectX::XMFLOAT3> m_CratesNitroPos;

	std::vector<CrateNormal*> m_pCratesNormal;
	std::vector<DirectX::XMFLOAT3> m_CratesNormalPos;

	std::vector<CrateTNT*> m_pCratesTNT;
	std::vector<DirectX::XMFLOAT3> m_CratesTNTPos;

	std::vector<CrateJump*> m_pCratesJump;
	std::vector<DirectX::XMFLOAT3> m_CratesJumpPos;

	std::vector<CrateLife*> m_pCratesLife;
	std::vector<DirectX::XMFLOAT3> m_CratesLifePos;

	std::vector<CrateCheckPoint*> m_pCratesCheckPoint;
	std::vector<DirectX::XMFLOAT3> m_CratesCheckPointPos;

	//PICK-UPS
	std::vector<WumpaFruit*> m_pWumpaFruits;
	std::vector<DirectX::XMFLOAT3> m_WumpaFruitPos;
}; 

