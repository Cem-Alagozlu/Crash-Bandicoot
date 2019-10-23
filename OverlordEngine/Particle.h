#pragma once
#include "ParticleEmitterComponent.h"

struct GameContext;

class Particle
{
public:
	//Constructor, ParticleEmitterSettings (BY REFERENCE!)
	//the settings of the emitter, passing it by reference makes sure
	//that our particle has the latest settings of the emitter
	explicit Particle(const ParticleEmitterSettings& emitterSettings);
	~Particle() = default;

	Particle(const Particle& other) = delete;
	Particle(Particle&& other) noexcept = delete;
	Particle& operator=(const Particle& other) = delete;
	Particle& operator=(Particle&& other) noexcept = delete;

	//Update method
	void Update(const GameContext& context);
	//Init, this method will 'reset' our particle
	void Init(DirectX::XMFLOAT3 initPosition);

	//Method to retrieve the ParticleVertex information of our particle
	ParticleVertex GetVertexInfo() const { return m_VertexInfo; }
	//Method to retrieve the state of our particle
	bool IsActive() const { return m_IsActive; }

private:
	//ParticleVertex information, used by the emitter's vertexbuffer
	ParticleVertex m_VertexInfo;
	//The emitter's settings (by reference)
	const ParticleEmitterSettings& m_EmitterSettings;
	//An active particle is still alive,
	//An inactive particle can be reused (ReInitialized)
	bool m_IsActive;
	//Total energy before going to an inactive state (seconds)
	float m_TotalEnergy;
	//Energy passed since the particle became active
	float m_CurrentEnergy;
	//Percentual grow/scale during the particle's lifetime
	// <0 = shrink, >0 = grow
	float m_SizeGrow;
	//Initial size of the particle
	float m_InitSize;


};
