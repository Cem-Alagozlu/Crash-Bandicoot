#include "stdafx.h"
#include "FollowCam.h"
#include "Components.h"

FollowCam::FollowCam(GameObject * crashBandicoot)
{
	m_pCrashBandicoot = crashBandicoot;
}

FollowCam::FollowCam()
{
}

void FollowCam::SetCamRot(DirectX::XMFLOAT3 rot)
{
	m_Rot = rot;
}

DirectX::XMFLOAT3 FollowCam::GetCamRot()
{
	return m_Rot;
}

void FollowCam::ResetCamRot()
{
	m_Rot = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f };
}

void FollowCam::Initialize(const GameContext& )
{
	auto camObj = new GameObject();
	camObj->AddComponent(new CameraComponent{});
	camObj->GetTransform()->Translate(0.0f, 20, -30.0f);
	camObj->GetTransform()->Rotate(30, 0, 0);
	AddChild(camObj);
	camObj->GetComponent<CameraComponent>()->SetActive();

	static_cast<CrashBandicoot*>(m_pCrashBandicoot)->SetCamera(this);
}

void FollowCam::Update(const GameContext& gameContext)
{
	DirectX::XMVECTOR myVec{};
	DirectX::XMFLOAT3 subVec{};

	
	GetTransform()->Rotate(m_Rot);

	myVec = DirectX::XMVector3Length(DirectX::XMVectorSubtract(
		DirectX::XMLoadFloat3(&m_pCrashBandicoot->GetTransform()->GetPosition()),
		DirectX::XMLoadFloat3(&GetTransform()->GetPosition())));

	DirectX::XMStoreFloat3(&subVec, myVec);
	if (subVec.x > 4.5f)
	{
		GetTransform()->Translate(DirectX::XMVectorLerp(
			DirectX::XMLoadFloat3(&GetTransform()->GetPosition()),
			DirectX::XMLoadFloat3(&m_pCrashBandicoot->GetTransform()->GetPosition()),
			gameContext.pGameTime->GetElapsed() * 5.f));
	}

}
