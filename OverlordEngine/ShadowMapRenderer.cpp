#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "ContentManager.h"
#include "ShadowMapMaterial.h"
#include "RenderTarget.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "OverlordGame.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	SafeDelete(m_pShadowRT);
	SafeDelete(m_pShadowMat);
}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
	if (m_IsInitialized)
		return;

	m_pShadowMat = new ShadowMapMaterial{};
	m_pShadowMat->Initialize(gameContext);

	m_pShadowRT = new RenderTarget{gameContext.pDevice};
	RENDERTARGET_DESC renderDesc{};
	renderDesc.EnableColorBuffer = false;
	renderDesc.EnableColorSRV = false;
	renderDesc.EnableDepthBuffer = true;
	renderDesc.EnableDepthSRV = true;
	renderDesc.Width = OverlordGame::GetGameSettings().Window.Width;
	renderDesc.Height = OverlordGame::GetGameSettings().Window.Height;
	m_pShadowRT->Create(renderDesc);

	m_IsInitialized = true;
}

void ShadowMapRenderer::SetLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
{
	m_LightPosition = position;
	m_LightDirection = direction;
	auto viewMatrix  = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&position), DirectX::XMLoadFloat3(&direction), { 0,1,0 });
	auto projectionMatrix = DirectX::XMMatrixOrthographicLH(300.0f * OverlordGame::GetGameSettings().Window.AspectRatio, 300.0f, 0.01f, 300.0f);

	DirectX::XMStoreFloat4x4(&m_LightVP, viewMatrix * projectionMatrix);
}

void ShadowMapRenderer::Begin(const GameContext& gameContext) const
{
	float color[4]{1,0,0,1};
	ID3D11ShaderResourceView *const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);
	m_pShadowRT->Clear(gameContext, color);
}

void ShadowMapRenderer::End(const GameContext& /*gameContext*/) const
{
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	m_pShadowMat->SetWorld(world);
	m_pShadowMat->SetBones(reinterpret_cast<const float*>(bones.data()),bones.size());
	m_pShadowMat->SetLightVP(m_LightVP);

	ShadowMapMaterial::ShadowGenType typeTech{};
	if (bones.size() == 0 )
	{
		typeTech = ShadowMapMaterial::ShadowGenType::Static;
	}
	else if (bones.size() > 0)
	{
		typeTech = ShadowMapMaterial::ShadowGenType::Skinned;
	}

	gameContext.pDeviceContext->IASetInputLayout(m_pShadowMat->m_pInputLayouts[typeTech]);
	UINT offset = 0;
	auto vertexBufferData = pMeshFilter->GetVertexBufferData(gameContext, m_pShadowMat->m_InputLayoutIds[typeTech]);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride, &offset);
	gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	auto tech = m_pShadowMat->m_pShadowTechs[typeTech];
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
	}
}


void ShadowMapRenderer::UpdateMeshFilter(const GameContext& gameContext, MeshFilter* pMeshFilter)
{
	ShadowMapMaterial::ShadowGenType genType = (ShadowMapMaterial::ShadowGenType)((int)pMeshFilter->m_BoneCount > 0);
	pMeshFilter->BuildVertexBuffer(gameContext, m_pShadowMat->m_InputLayoutIds[genType],
		m_pShadowMat->m_InputLayoutSizes[genType], 
		m_pShadowMat->m_InputLayoutDescriptions[genType]);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	return m_pShadowRT->GetDepthShaderResourceView();
}
