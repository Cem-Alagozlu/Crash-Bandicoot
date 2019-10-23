#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(std::vector<DirectX::XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_TickCount(0),
m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}

void ModelAnimator::SetAnimation(UINT clipNumber,bool isLooping)
{

	m_ClipSet = false;
	if (clipNumber < m_pMeshFilter->m_AnimationClips.size())
	{
		m_CurrentClip = m_pMeshFilter->m_AnimationClips[clipNumber];
		SetAnimation(m_CurrentClip,isLooping);
	}
	else
	{
		Reset();
		Logger::LogError(L"Error, clipNumber > Animation clips");
	}

}

void ModelAnimator::SetAnimation(std::wstring clipName, bool isLooping)
{
	m_ClipSet = false;
	for (unsigned int i = 0; i < m_pMeshFilter->m_AnimationClips.size() ; i++)
	{
		if (m_pMeshFilter->m_AnimationClips[i].Name == clipName)
		{
			m_CurrentClip = m_pMeshFilter->m_AnimationClips[i];
			SetAnimation(m_CurrentClip,isLooping);
			return;
		}
	}
	Reset();
	Logger::LogError(L"Couldn't find the correct clipname!\n");
}

void ModelAnimator::SetAnimation(AnimationClip clip, bool isLooping)
{
	m_ClipSet = true;
	m_CurrentClip = clip;
	m_IsLooping = isLooping;
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	if (pause)
	{
		m_IsPlaying = false;
	}
	m_TickCount = 0;
	m_AnimationSpeed = 1.0f;

	if (m_ClipSet)
	{
		auto boneTransform = m_CurrentClip.Keys[0].BoneTransforms;
		m_Transforms.assign(boneTransform.begin(), boneTransform.end());
	
	}
	else
	{
		DirectX::XMFLOAT4X4 identityMatrix{};
		DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());
		m_Transforms.assign(m_CurrentClip.Keys[0].BoneTransforms.size(), identityMatrix);
	}
}


void ModelAnimator::Update(const GameContext& gameContext)
{
	float elapsedSec = gameContext.pGameTime->GetElapsed();

	if (m_IsPlaying && m_ClipSet)
	{
		auto passedTicks = elapsedSec * m_CurrentClip.TicksPerSecond * m_AnimationSpeed;
		passedTicks = fmod(passedTicks, m_CurrentClip.Duration);
		//2.
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0)
			{
				m_TickCount = m_CurrentClip.Duration;
				if (!m_IsLooping)
				{
					Pause();
				}
			}
		}
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.Duration)
			{
				m_TickCount -= m_CurrentClip.Duration;

				if (!m_IsLooping)
				{
					Pause();
				}
			}
		}

		AnimationKey keyA{}, keyB{}, keyToFind{};
		keyToFind.Tick = m_TickCount;

		auto keyCheck = [](AnimationKey& key01, AnimationKey key02) {return key01.Tick < key02.Tick; };
		auto iterator = std::lower_bound(m_CurrentClip.Keys.begin(), m_CurrentClip.Keys.end(), keyToFind, keyCheck);
		keyB = *iterator;
		keyA = *std::prev(iterator);


		float blend{ (m_TickCount - keyA.Tick) / (keyB.Tick - keyA.Tick) };
		m_Transforms.clear();

		for (size_t i = 0; i < keyA.BoneTransforms.size(); i++)
		{
			auto transformA = keyA.BoneTransforms[i];
			auto transformB = keyB.BoneTransforms[i];

			DirectX::XMVECTOR translationA,rotationA,scaleA;
			DirectX::XMVECTOR translationB, rotationB, scaleB;
			DirectX::XMMatrixDecompose(&scaleA, &rotationA, &translationA, DirectX::XMLoadFloat4x4(&transformA));
			DirectX::XMMatrixDecompose(&scaleB, &rotationB, &translationB, DirectX::XMLoadFloat4x4(&transformB));

			translationA = DirectX::XMVectorLerp(translationA, translationB, blend);
			scaleA = DirectX::XMVectorLerp(scaleA, scaleB, blend);
			rotationA = DirectX::XMQuaternionSlerp(rotationA, rotationB, blend);

			DirectX::XMMATRIX transform = DirectX::XMMatrixAffineTransformation(scaleA, {}, rotationA, translationA);
			DirectX::XMStoreFloat4x4(&transformA, transform);

			m_Transforms.emplace_back(transformA);
		}
	}
}
