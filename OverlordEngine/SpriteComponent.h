#pragma once
#include "BaseComponent.h"

class TextureData;

class SpriteComponent : public BaseComponent
{
	TextureData* m_pTexture;
	std::wstring m_SpriteAsset;
	DirectX::XMFLOAT2 m_Pivot;
	DirectX::XMFLOAT4 m_Color;
public:
	SpriteComponent(const SpriteComponent& other) = delete;
	SpriteComponent(SpriteComponent&& other) noexcept = delete;
	SpriteComponent& operator=(const SpriteComponent& other) = delete;
	SpriteComponent& operator=(SpriteComponent&& other) noexcept = delete;
	SpriteComponent(std::wstring  spriteAsset, DirectX::XMFLOAT2 pivot = DirectX::XMFLOAT2(0, 0),
	                DirectX::XMFLOAT4 color = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
	virtual ~SpriteComponent() = default;

	DirectX::XMFLOAT2 GetPivot() const { return m_Pivot; }
	DirectX::XMFLOAT4 GetColor() const { return m_Color; }

	void SetPivot(DirectX::XMFLOAT2 pivot) { m_Pivot = pivot; }
	void SetColor(DirectX::XMFLOAT4 color) { m_Color = color; }
	void SetTexture(const std::wstring& spriteAsset);

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
};
