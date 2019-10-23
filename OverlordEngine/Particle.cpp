#include "stdafx.h"
#include "Particle.h"
#include "MathHelper.h"

// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
using namespace DirectX;

Particle::Particle(const ParticleEmitterSettings& emitterSettings):
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_IsActive(false),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0)
{}

void Particle::Update(const GameContext& gameContext)
{
	if (!m_IsActive)
	{
		return;
	}

	m_CurrentEnergy -= gameContext.pGameTime->GetElapsed();
	if (m_CurrentEnergy <= 0.0f)
	{
		m_IsActive = false;
		return;
	}

	XMVECTOR vertexPos = DirectX::XMLoadFloat3(&m_VertexInfo.Position);
	XMVECTOR emitVel = DirectX::XMLoadFloat3(&m_EmitterSettings.Velocity);
	XMVECTOR calculateVal = vertexPos + emitVel * gameContext.pGameTime->GetElapsed();

	DirectX::XMStoreFloat3(&m_VertexInfo.Position, calculateVal);
	m_VertexInfo.Color = m_EmitterSettings.Color;
	
	float particleLifePercent = m_CurrentEnergy / m_TotalEnergy;
	m_VertexInfo.Color.w = particleLifePercent * 2;

	if (m_SizeGrow < 1.0f)
	{
		m_VertexInfo.Size = m_InitSize - (1.0f - particleLifePercent) * m_SizeGrow;
	}
	else if (m_SizeGrow > 1.0f)
	{
		m_VertexInfo.Size = m_InitSize + (1.0f - particleLifePercent) * m_SizeGrow;
	}
}

void Particle::Init(XMFLOAT3 initPosition)
{
	m_IsActive = true;

	m_TotalEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	m_CurrentEnergy = m_TotalEnergy;

	XMVECTOR unitVector = { 1,0,0 };
	XMMATRIX randRot = XMMatrixRotationRollPitchYaw(randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI));
	XMVECTOR randRotVec = XMVector3TransformNormal(unitVector, randRot);

	float distance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);




	XMVECTOR initPos = DirectX::XMLoadFloat3(&initPosition);
	XMVECTOR calculateVal = initPos + randRotVec * distance;
	DirectX::XMStoreFloat3(&m_VertexInfo.Position, calculateVal);


	m_VertexInfo.Size = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);
	m_InitSize = m_VertexInfo.Size;

	m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);
}
