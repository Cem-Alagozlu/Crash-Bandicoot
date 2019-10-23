#include "stdafx.h"
#include "UberMaterial.h"
#include "ContentManager.h"
#include "Components.h"
#include "TextureDataLoader.h"
#include "GeneralStructs.h"
#include "Logger.h"
#include "TextureData.h"


//LIGHT
//*****
ID3DX11EffectVectorVariable* UberMaterial::m_pLightDirectionVariable = nullptr;

//DIFFUSE
//*******
ID3DX11EffectScalarVariable* UberMaterial::m_pUseDiffuseTextureVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectVectorVariable* UberMaterial::m_pDiffuseColorVariable = nullptr;

//SPECULAR
//********
ID3DX11EffectVectorVariable* UberMaterial::m_pSpecularColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseSpecularLevelTextureVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pSpecularLevelSRVvariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pShininessVariable = nullptr;

//AMBIENT
//*******
ID3DX11EffectVectorVariable* UberMaterial::m_pAmbientColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pAmbientIntensityVariable = nullptr;

//NORMAL MAPPING
//**************
ID3DX11EffectScalarVariable* UberMaterial::m_pFlipGreenChannelVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseNormalMappingVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pNormalMappingSRVvariable = nullptr;

//ENVIRONMENT MAPPING
//*******************
ID3DX11EffectScalarVariable* UberMaterial::m_pUseEnvironmentMappingVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pEnvironmentSRVvariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pReflectionStrengthVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pRefractionStrengthVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pRefractionIndexVariable = nullptr;

//OPACITY
//***************

 ID3DX11EffectScalarVariable* UberMaterial::m_pOpacityVariable = nullptr;
 ID3DX11EffectScalarVariable*UberMaterial::m_pUseOpacityMapVariable = nullptr;
 ID3DX11EffectShaderResourceVariable* UberMaterial::m_pOpacitySRVvariable = nullptr;

//SPECULAR MODELS
//***************
ID3DX11EffectScalarVariable* UberMaterial::m_pUseBlinnVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUsePhongVariable = nullptr;

//FRESNEL FALLOFF
//***************
ID3DX11EffectScalarVariable* UberMaterial::m_pUseFresnelFalloffVariable = nullptr;
ID3DX11EffectVectorVariable* UberMaterial::m_pFresnelColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelPowerVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelMultiplierVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelHardnessVariable = nullptr;

UberMaterial::UberMaterial()
	:Material(L"./Resources/Effects/UberShader.fx")
	, m_LightDirection{ 0,-1,0 }
	, m_bDiffuseTexture{ false }
	, m_pDiffuseTexture{ nullptr }
	, m_ColorDiffuse{ 1,1,1,1 }
	, m_ColorSpecular{ 1,1,1,1 }
	, m_bSpecularLevelTexture{ false }
	, m_pSpecularLevelTexture{ nullptr }
	, m_Shininess{ 15 }
	, m_ColorAmbient{ 0,0,0,1 }
	, m_AmbientIntensity{ 0.0f }
	, m_bFlipGreenChannel{ false }
	, m_bNormalMapping{ false }
	, m_pNormalMappingTexture{ nullptr }
	, m_bEnvironmentMapping{ false }
	, m_pEnvironmentCube{ nullptr }
	, m_ReflectionStrength{ 0.0f }
	, m_RefractionStrength{ 0.0f }
	, m_RefractionIndex{ 0.3f }
	, m_Opacity{ 1 }
	, m_bOpacityMap{ false }
	, m_pOpacityMap{ nullptr }
	, m_bSpecularBlinn{ false }
	, m_bSpecularPhong{ true }
	, m_bFresnelFaloff{ false }
	, m_ColorFresnel{ 1,1,1,1 }
	, m_FresnelPower{ 1 }
	, m_FresnelMultiplier{ 1 }
	, m_FresnelHardness{ 0 }
{

}

UberMaterial::~UberMaterial()
{

}

void UberMaterial::SetLightDirection(DirectX::XMFLOAT3 direction)
{
	m_LightDirection = direction;
}

void UberMaterial::EnableDiffuseTexture(bool enable)
{
	m_bDiffuseTexture = enable;
}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetDiffuseColor(DirectX::XMFLOAT4 color)
{
	m_ColorDiffuse = color;
}

void UberMaterial::SetSpecularColor(DirectX::XMFLOAT4 color)
{
	m_ColorSpecular = color;
}

void UberMaterial::EnableSpecularLevelTexture(bool enable)
{
	m_bSpecularLevelTexture = enable;
}

void UberMaterial::SetSpecularLevelTexture(const std::wstring& assetFile)
{
	m_pSpecularLevelTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetShininess(int shininess)
{
	m_Shininess = shininess;
}

void UberMaterial::SetAmbientColor(DirectX::XMFLOAT4 color)
{
	m_ColorAmbient = color;
}

void UberMaterial::SetAmbientIntensity(float intensity)
{
	m_AmbientIntensity = intensity;
}

void UberMaterial::FlipNormalGreenCHannel(bool flip)
{
	m_bFlipGreenChannel = flip;
}

void UberMaterial::EnableNormalMapping(bool enable)
{
	m_bNormalMapping = enable;
}

void UberMaterial::SetNormalMapTexture(const std::wstring& assetFile)
{
	m_pNormalMappingTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::EnableEnvironmentMapping(bool enable)
{
	m_bEnvironmentMapping = enable;
}

void UberMaterial::SetEnvironmentCube(const std::wstring& assetFile)
{
	m_pEnvironmentCube = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetReflectionStrength(float strength)
{
	m_ReflectionStrength = strength;
}

void UberMaterial::SetRefractionStrength(float strength)
{
	m_ReflectionStrength = strength;
}

void UberMaterial::SetRefractionIndex(float index)
{
	m_RefractionIndex = index;
}

void UberMaterial::SetOpacity(float opacity)
{
	m_Opacity = opacity;
}

void UberMaterial::EnableOpacityMap(bool enable)
{
	m_bOpacityMap = enable;
}

void UberMaterial::SetOpacityTexture(const std::wstring& assetFile)
{
	m_pOpacityMap = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::EnableSpecularBlinn(bool enable)
{
	m_bSpecularBlinn = enable;
}

void UberMaterial::EnableSpecularPhong(bool enable)
{
	m_bSpecularPhong = enable;
}

void UberMaterial::EnableFresnelFaloff(bool enable)
{
	m_bFresnelFaloff = enable;
}

void UberMaterial::SetFresnelColor(DirectX::XMFLOAT4 color)
{
	m_ColorFresnel = color;
}

void UberMaterial::SetFresnelPower(float power)
{
	m_FresnelPower = power;
}

void UberMaterial::SetFresnelMultiplier(float multiplier)
{
	m_FresnelMultiplier = multiplier;
}

void UberMaterial::SetFresnelHardness(float hardness)
{
	m_FresnelHardness = hardness;
}

void UberMaterial::LoadEffectVariables()
{
	//LIGHT
	//*****
	m_pLightDirectionVariable = Material::GetEffect()->GetVariableByName("gLightDirection")->AsVector();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gLightDirection!"); }

	//DIFFUSE
	//*******
	m_pUseDiffuseTextureVariable = Material::GetEffect()->GetVariableByName("gUseTextureDiffuse")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gUseTextureDiffuse!"); }

	m_pDiffuseSRVvariable = Material::GetEffect()->GetVariableByName("gTextureDiffuse")->AsShaderResource();
	if (!m_pLightDirectionVariable->IsValid()) {Logger::LogWarning(L"Couldn't find gTextureDiffuse!"); }

	m_pDiffuseColorVariable = Material::GetEffect()->GetVariableByName("gColorDiffuse")->AsVector();
	if (!m_pLightDirectionVariable->IsValid()) {Logger::LogWarning(L"Couldn't find gColorDiffuse!");}

	//SPECULAR
	//********
	m_pSpecularColorVariable = Material::GetEffect()->GetVariableByName("gColorSpecular")->AsVector();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gColorSpecular!"); }

	m_pUseSpecularLevelTextureVariable = Material::GetEffect()->GetVariableByName("gUseTextureSpecularIntensity")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gUseTextureSpecularIntensity!"); }

	m_pSpecularLevelSRVvariable = Material::GetEffect()->GetVariableByName("gTextureSpecularIntensity")->AsShaderResource();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gTextureSpecularIntensity!"); }

	m_pShininessVariable = Material::GetEffect()->GetVariableByName("gShininess")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gShininess!"); }

	//AMBIENT
	//*******
	m_pAmbientColorVariable = Material::GetEffect()->GetVariableByName("gColorAmbient")->AsVector();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gColorAmbient!"); }

	m_pAmbientIntensityVariable = Material::GetEffect()->GetVariableByName("gAmbientIntensity")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gAmbientIntensity!"); }


	//NORMAL MAPPING
	//**************
	m_pFlipGreenChannelVariable = Material::GetEffect()->GetVariableByName("gFlipGreenChannel")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gFlipGreenChannel!"); }

	m_pUseNormalMappingVariable = Material::GetEffect()->GetVariableByName("gUseTextureNormal")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gUseTextureNormal!"); }

	m_pNormalMappingSRVvariable = Material::GetEffect()->GetVariableByName("gTextureNormal")->AsShaderResource();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gTextureNormal!"); }


	//ENVIRONMENT MAPPING
	//*******************
	m_pUseEnvironmentMappingVariable = Material::GetEffect()->GetVariableByName("gUseTextureEnvironment")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gUseTextureEnvironment!"); }

	m_pEnvironmentSRVvariable = Material::GetEffect()->GetVariableByName("gCubeEnvironment")->AsShaderResource();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gCubeEnvironment!"); }

	m_pReflectionStrengthVariable = Material::GetEffect()->GetVariableByName("gReflectionStrength")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gReflectionStrength!"); }

	m_pRefractionStrengthVariable = Material::GetEffect()->GetVariableByName("gRefractionStrength")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gRefractionStrength!"); }

	m_pRefractionIndexVariable = Material::GetEffect()->GetVariableByName("gRefractionIndex")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gRefractionIndex!"); }

	//OPACITY
	//***************
	m_pOpacityVariable = Material::GetEffect()->GetVariableByName("gOpacityIntensity")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gOpacityIntensity!"); }

	m_pUseOpacityMapVariable = Material::GetEffect()->GetVariableByName("gUseTextureOpacity")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gUseTextureOpacity!"); }

	m_pOpacitySRVvariable = Material::GetEffect()->GetVariableByName("gTextureOpacity")->AsShaderResource();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gTextureOpacity!"); }

	//SPECULAR MODELS
	//***************
	m_pUseBlinnVariable = Material::GetEffect()->GetVariableByName("gUseSpecularBlinn")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gUseSpecularBlinn!"); }

	m_pUsePhongVariable = Material::GetEffect()->GetVariableByName("gUseSpecularPhong")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gUseSpecularPhong!"); }

	//FRESNEL FALLOFF
	//***************
	m_pUseFresnelFalloffVariable = Material::GetEffect()->GetVariableByName("gUseFresnelFalloff")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gUseFresnelFalloff!"); }

	m_pFresnelColorVariable = Material::GetEffect()->GetVariableByName("gColorFresnel")->AsVector();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gColorFresnel!"); }

	m_pFresnelPowerVariable = Material::GetEffect()->GetVariableByName("gFresnelPower")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gFresnelPower!"); }

	m_pFresnelMultiplierVariable = Material::GetEffect()->GetVariableByName("gFresnelMultiplier")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gFresnelMultiplier!"); }

	m_pFresnelHardnessVariable = Material::GetEffect()->GetVariableByName("gFresnelHardness")->AsScalar();
	if (!m_pLightDirectionVariable->IsValid()) { Logger::LogWarning(L"Couldn't find gFresnelHardness!"); }

}

void UberMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);
	//LIGHT
	//*****
	m_pLightDirectionVariable->SetFloatVector(reinterpret_cast<float*>(&m_LightDirection));

	//DIFFUSE
	//*******
	m_pUseDiffuseTextureVariable->SetBool(m_bDiffuseTexture);
	if (m_bDiffuseTexture) m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	m_pDiffuseColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_ColorDiffuse));

	//SPECULAR
	//********
	m_pSpecularColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_ColorSpecular));
	m_pUseSpecularLevelTextureVariable->SetBool(m_bSpecularLevelTexture);
	if (m_bSpecularLevelTexture) m_pSpecularLevelSRVvariable->SetResource(m_pSpecularLevelTexture->GetShaderResourceView());
	m_pShininessVariable->SetInt(m_Shininess);

	//AMBIENT
	//*******
	m_pAmbientColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_ColorAmbient));
	m_pAmbientIntensityVariable->SetFloat(m_AmbientIntensity);

	//NORMAL MAPPING
	//**************
	m_pFlipGreenChannelVariable->SetBool(m_bFlipGreenChannel);
	m_pUseNormalMappingVariable->SetBool(m_bNormalMapping);
	if (m_bNormalMapping) m_pNormalMappingSRVvariable->SetResource(m_pNormalMappingTexture->GetShaderResourceView());

	//ENVIRONMENT MAPPING
	//*******************
	m_pUseEnvironmentMappingVariable->SetBool(m_bEnvironmentMapping);
	if (m_bEnvironmentMapping) m_pEnvironmentSRVvariable->SetResource(m_pEnvironmentCube->GetShaderResourceView());
	m_pReflectionStrengthVariable->SetFloat(m_ReflectionStrength);
	m_pRefractionStrengthVariable->SetFloat(m_RefractionStrength);
	m_pRefractionIndexVariable->SetFloat(m_RefractionIndex);

	//OPACITY
	//***************
	m_pOpacityVariable->SetFloat(m_Opacity);
	m_pUseOpacityMapVariable->SetBool(m_bOpacityMap);
	if (m_bOpacityMap) m_pOpacitySRVvariable->SetResource(m_pOpacityMap->GetShaderResourceView());

	//SPECULAR MODELS
	//***************
	m_pUseBlinnVariable->SetBool(m_bSpecularBlinn);
	m_pUsePhongVariable->SetBool(m_bSpecularPhong);

	//FRESNEL FALLOFF
	//***************
	m_pUseFresnelFalloffVariable->SetBool(m_bFresnelFaloff);
	m_pFresnelColorVariable->SetFloatVector(reinterpret_cast<float*>(&m_ColorFresnel));
	m_pFresnelPowerVariable->SetFloat(m_FresnelPower);
	m_pFresnelMultiplierVariable->SetFloat(m_FresnelMultiplier);
	m_pFresnelHardnessVariable->SetFloat(m_FresnelHardness);
}