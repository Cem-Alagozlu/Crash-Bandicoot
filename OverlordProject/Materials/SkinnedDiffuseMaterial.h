#pragma once
#include "Material.h"

class TextureData;

class SkinnedDiffuseMaterial final : public Material
{
public:
	SkinnedDiffuseMaterial();
	~SkinnedDiffuseMaterial() = default;

	SkinnedDiffuseMaterial(const SkinnedDiffuseMaterial& other) = delete;
	SkinnedDiffuseMaterial(SkinnedDiffuseMaterial&& other) noexcept = delete;
	SkinnedDiffuseMaterial& operator=(const SkinnedDiffuseMaterial& other) = delete;
	SkinnedDiffuseMaterial& operator=(SkinnedDiffuseMaterial&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	TextureData* m_pDiffuseTexture;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	static ID3DX11EffectMatrixVariable* m_pBoneTransforms;
};

