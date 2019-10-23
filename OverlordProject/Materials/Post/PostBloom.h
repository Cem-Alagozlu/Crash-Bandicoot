#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;
class PostBloom : public PostProcessingMaterial
{
public:
	PostBloom();
	virtual ~PostBloom()  = default;

	PostBloom(const PostBloom& other) = delete;
	PostBloom(PostBloom&& other) noexcept = delete;
	PostBloom& operator=(const PostBloom& other) = delete;
	PostBloom& operator=(PostBloom&& other) noexcept = delete;

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
};

