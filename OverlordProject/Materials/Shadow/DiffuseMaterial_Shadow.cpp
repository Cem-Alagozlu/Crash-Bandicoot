//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "DiffuseMaterial_Shadow.h"
#include "GeneralStructs.h"
#include "Logger.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "Components.h"

ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow::m_pShadowSRVvariable = nullptr;
ID3DX11EffectVectorVariable* DiffuseMaterial_Shadow::m_pLightDirectionVariable = nullptr;
ID3DX11EffectMatrixVariable* DiffuseMaterial_Shadow::m_pLightWVPvariable = nullptr;

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow() : Material(L"./Resources/Effects/Shadow/PosNormTex3D_Shadow.fx"),
	m_pDiffuseTexture(nullptr)
{}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void DiffuseMaterial_Shadow::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	m_LightDirection = dir;
	
}

void DiffuseMaterial_Shadow::LoadEffectVariables()
{
	m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseSRVvariable->IsValid())
	{
		Logger::LogWarning(L"DiffusedMaterial_Shadow::LoadEffectVariables() > \'gDiffuseMap\' variable not found!");
		m_pDiffuseSRVvariable = nullptr;
	}

	m_pShadowSRVvariable = GetEffect()->GetVariableByName("gShadowMap")->AsShaderResource();
	if (!m_pShadowSRVvariable->IsValid())
	{
		Logger::LogWarning(L"DiffusedMaterial_Shadow::LoadEffectVariables() > \'gShadowMap\' variable not found!");
		m_pShadowSRVvariable = nullptr;
	}

	m_pLightDirectionVariable = GetEffect()->GetVariableByName("gLightDirection")->AsVector();
	if (!m_pLightDirectionVariable->IsValid())
	{
		Logger::LogWarning(L"DiffusedMaterial_Shadow::LoadEffectVariables() > \'gLightDirection\' variable not found!");
		m_pLightDirectionVariable = nullptr;
	}

	m_pLightWVPvariable = GetEffect()->GetVariableByName("gWorldViewProj_Light")->AsMatrix();
	if (!m_pLightWVPvariable->IsValid())
	{
		Logger::LogWarning(L"DiffusedMaterial_Shadow::LoadEffectVariables() > \'gWorldViewProj_Light\' variable not found!");
		m_pLightWVPvariable = nullptr;
	}
}

void DiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	m_pShadowSRVvariable->SetResource(gameContext.pShadowMapper->GetShadowMap());
	m_pLightDirectionVariable->SetFloatVector(reinterpret_cast<const float*>(&m_LightDirection));

	DirectX::XMFLOAT4X4 lightVP = gameContext.pShadowMapper->GetLightVP();
	DirectX::XMFLOAT4X4 world = pModelComponent->GetTransform()->GetWorld();
	DirectX::XMFLOAT4X4 LVPW{};
	//
	DirectX::XMStoreFloat4x4(&LVPW, DirectX::XMLoadFloat4x4(&world) * DirectX::XMLoadFloat4x4(&lightVP));
	m_pLightWVPvariable->SetMatrix(reinterpret_cast<float*>(&LVPW));
}