#include "stdafx.h"
#include "../OverlordEngine/SkyBoxPrefab.h"
#include "Components.h"
#include "../OverlordProject/Materials/SkyBoxMaterial.h"

SkyboxPrefab::SkyboxPrefab()
{
}


SkyboxPrefab::~SkyboxPrefab()
{
}

void SkyboxPrefab::Initialize(const GameContext& gameContext)
{
	auto skyboxMaterial = new SkyboxMaterial{};
	gameContext.pMaterialManager->AddMaterial(skyboxMaterial, 1207);

	auto modelComponent = new ModelComponent{ L"Resources/Game/Models/Box.ovm" };
	modelComponent->SetMaterial(1207);
	AddComponent(modelComponent);
}

