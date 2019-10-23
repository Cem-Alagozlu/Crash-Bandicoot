#pragma once
#include "GameObject.h"

class SkyboxPrefab : public GameObject
{
public:
	SkyboxPrefab();
	~SkyboxPrefab();
protected:
	virtual void Initialize(const GameContext& gameContext) override;
};

