#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostBlur : public PostProcessingMaterial
{
public:
	PostBlur();
	virtual ~PostBlur() = default;

	PostBlur(const PostBlur& other) = delete;
	PostBlur(PostBlur&& other) noexcept = delete;
	PostBlur& operator=(const PostBlur& other) = delete;
	PostBlur& operator=(PostBlur&& other) noexcept = delete;
protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
};
