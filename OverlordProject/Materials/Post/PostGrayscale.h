#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostGrayscale : public PostProcessingMaterial
{
public:
	PostGrayscale();
	PostGrayscale(const PostGrayscale& other) = delete;
	PostGrayscale(PostGrayscale&& other) noexcept = delete;
	PostGrayscale& operator=(const PostGrayscale& other) = delete;
	PostGrayscale& operator=(PostGrayscale&& other) noexcept = delete;
	virtual ~PostGrayscale() = default;

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
};
