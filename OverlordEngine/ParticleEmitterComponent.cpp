#include "stdafx.h"
#include "ParticleEmitterComponent.h"
 #include <utility>
#include "EffectHelper.h"
#include "ContentManager.h"
#include "TextureDataLoader.h"
#include "Particle.h"
#include "TransformComponent.h"

ParticleEmitterComponent::ParticleEmitterComponent(std::wstring  assetFile, int particleCount):
	m_pVertexBuffer(nullptr),
	m_pEffect(nullptr),
	m_pParticleTexture(nullptr),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_Settings(ParticleEmitterSettings()),
	m_ParticleCount(particleCount),
	m_ActiveParticles(0),
	m_LastParticleInit(0.0f),
	m_AssetFile(std::move(assetFile))
{
	for (int i = 0; i < m_ParticleCount; i++)
	{
		m_Particles.push_back(new Particle{ m_Settings });
	}
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	for (Particle* particle : m_Particles)
	{
		delete particle;
	}		

	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pInputLayout);
}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
	LoadEffect(gameContext);
	CreateVertexBuffer(gameContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ParticleRenderer.fx");
	m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);

	if (!m_pWvpVariable)
	{
		m_pWvpVariable = m_pEffect->GetVariableBySemantic("WorldViewProjection")->AsMatrix();
	}
		
	if (!m_pViewInverseVariable)
	{
		m_pViewInverseVariable = m_pEffect->GetVariableBySemantic("ViewInverse")->AsMatrix();
	}
		
	if (!m_pParticleTexture)
	{
		m_pTextureVariable = m_pEffect->GetVariableByName("gParticleTexture")->AsShaderResource();
	}


	std::vector<ILDescription> inputLayoutDesc{};
	UINT inputLayoutID{};
	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pDefaultTechnique, &m_pInputLayout, inputLayoutDesc, m_pInputLayoutSize, inputLayoutID);
}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
	SafeRelease(m_pVertexBuffer);

	D3D11_BUFFER_DESC buffer = {};
	buffer.ByteWidth = m_ParticleCount * sizeof(ParticleVertex);
	buffer.MiscFlags = 0;
	buffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer.Usage = D3D11_USAGE_DYNAMIC;
	buffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	Logger::LogHResult(gameContext.pDevice->CreateBuffer(&buffer, nullptr, &m_pVertexBuffer), L"Failed to create buffer!");
}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
	float particleInterval = ((m_Settings.MaxEnergy - m_Settings.MinEnergy) / m_ParticleCount);
	m_LastParticleInit += gameContext.pGameTime->GetElapsed();
	m_ActiveParticles = 0;

	//BUFFER MAPPING CODE [PARTIAL :)]
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ParticleVertex* pBuffer = (ParticleVertex*) mappedResource.pData;


	for (Particle* particles : m_Particles)
	{
		particles->Update(gameContext);
		if (particles->IsActive())
		{
			pBuffer[m_ActiveParticles] = particles->GetVertexInfo();
			m_ActiveParticles++;
		}
		else if (m_LastParticleInit >= particleInterval)
		{
			particles->Init(GetTransform()->GetPosition());
			pBuffer[m_ActiveParticles] = particles->GetVertexInfo();
			m_ActiveParticles++;
			m_LastParticleInit = 0.0f;
		}
	}
	gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::Draw(const GameContext& )
{}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{
	DirectX::XMMATRIX viewProj = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
	m_pWvpVariable->SetMatrix((float*)(&viewProj));

	DirectX::XMMATRIX viewInverse = XMLoadFloat4x4(&gameContext.pCamera->GetViewInverse());
	m_pViewInverseVariable->SetMatrix((float*)(&viewInverse));

	m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());

	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	UINT offset = 0;
	UINT stride = sizeof(ParticleVertex);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC desc{};
	m_pDefaultTechnique->GetDesc(&desc);

	for (UINT i = 0; i < desc.Passes; ++i)
	{
		m_pDefaultTechnique->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(m_ActiveParticles * 3, 0);
	}
}
