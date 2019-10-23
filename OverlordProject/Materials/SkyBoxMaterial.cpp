#include "stdafx.h"
#include "ContentManager.h"
#include "SkyboxMaterial.h"
#include "TextureData.h"

TextureData* SkyboxMaterial::m_pSkyboxTexture{ nullptr };
ID3DX11EffectShaderResourceVariable* SkyboxMaterial::m_pSkyboxTextureVariable{ nullptr };

SkyboxMaterial::SkyboxMaterial()
	: Material{ L"./Resources/Effects/SkyBox.fx" }
{
	if (!m_pSkyboxTexture)
	{
		m_pSkyboxTexture = ContentManager::Load<TextureData>(L"Resources/Game/Texture/SkyBox.dds");
	}
}

SkyboxMaterial::~SkyboxMaterial()
{
}

void SkyboxMaterial::LoadEffectVariables()
{
	if (!m_pSkyboxTextureVariable)
	{
		m_pSkyboxTextureVariable = GetEffect()->GetVariableByName("m_CubeMap")->AsShaderResource();
		if (!m_pSkyboxTextureVariable->IsValid())
		{
			Logger::LogWarning(L"SkyboxMaterial::LoadEffectVariables() > \'m_pSkyboxTexture\' variable not found!");
			m_pSkyboxTextureVariable = nullptr;
		}
	}
}

void SkyboxMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* modelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(modelComponent);
	m_pSkyboxTextureVariable->SetResource(m_pSkyboxTexture->GetShaderResourceView());
}
