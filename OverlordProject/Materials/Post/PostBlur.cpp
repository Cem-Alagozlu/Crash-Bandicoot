#include "stdafx.h"
#include "PostBlur.h"
#include "RenderTarget.h"

PostBlur::PostBlur()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Blur.fx", 2),
	m_pTextureMapVariabele(nullptr)
{
}

void PostBlur::LoadEffectVariables()
{
	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariabele->IsValid())
	{
		Logger::LogWarning(L"LoadEffectVariables error!: gTexture <-> PostBlur");
	}
}

void PostBlur::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
}