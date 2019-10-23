#pragma once
#include "Material.h"

class TextureData;

class SkyboxMaterial : public Material
{
public:
	SkyboxMaterial();
	~SkyboxMaterial();
protected:
	virtual void LoadEffectVariables() override;
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	static TextureData* m_pSkyboxTexture;
	static ID3DX11EffectShaderResourceVariable* m_pSkyboxTextureVariable;
};

