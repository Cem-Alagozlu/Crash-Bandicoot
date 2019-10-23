#include "stdafx.h"
#include "PostProcessingMaterial.h"
#include "RenderTarget.h"
#include "OverlordGame.h"
#include "ContentManager.h"

PostProcessingMaterial::PostProcessingMaterial(std::wstring effectFile, unsigned int renderIndex,
                                               std::wstring technique)
	: m_IsInitialized(false), 
	  m_pInputLayout(nullptr),
	  m_pInputLayoutSize(0),
	  m_effectFile(std::move(effectFile)),
	  m_InputLayoutID(0),
	  m_RenderIndex(renderIndex),
	  m_pRenderTarget(nullptr),
	  m_pVertexBuffer(nullptr),
	  m_pIndexBuffer(nullptr),
	  m_NumVertices(0),
	  m_NumIndices(0),
	  m_VertexBufferStride(0),
	  m_pEffect(nullptr),
	  m_pTechnique(nullptr),
	  m_TechniqueName(std::move(technique))
{
}

PostProcessingMaterial::~PostProcessingMaterial()
{

	delete m_pRenderTarget;
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
	SafeRelease(m_pInputLayout);
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if (!m_IsInitialized)
	{
		LoadEffect(gameContext, m_effectFile);
		CreateInputLayout(gameContext, m_pTechnique);
		CreateVertexBuffer(gameContext);

		m_pRenderTarget = new RenderTarget{ gameContext.pDevice };

		RENDERTARGET_DESC renderDesc{};
		int widthWindow = OverlordGame::GetGameSettings().Window.Width;
		int heightWindow = OverlordGame::GetGameSettings().Window.Height;

		renderDesc.Width = widthWindow;
		renderDesc.Height = heightWindow;
		renderDesc.EnableColorSRV = true;
		m_pRenderTarget->Create(renderDesc);	
	}
}

bool PostProcessingMaterial::LoadEffect(const GameContext& gameContext, const std::wstring& effectFile)
{
	m_pEffect = ContentManager::Load<ID3DX11Effect>(effectFile);
	UNREFERENCED_PARAMETER(gameContext);

	if(!m_TechniqueName.empty())
	{
		std::string tech = std::string(m_TechniqueName.begin(), m_TechniqueName.end());
		m_pTechnique = m_pEffect->GetTechniqueByName(tech.c_str());
	}
	else
	{
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	}
	LoadEffectVariables();

	return true;
}

void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRendertarget)
{
	FLOAT colorRGBA[4] = { 1,0,0,1 };
	m_pRenderTarget->Clear(gameContext, colorRGBA);

	UpdateEffectVariables(previousRendertarget);
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

	UINT offset{ 0 };
	m_VertexBufferStride = sizeof(VertexPosTex);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &offset);
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(4, 0);
	}

	gameContext.pDeviceContext->GenerateMips(m_pRenderTarget->GetShaderResourceView());
}

void PostProcessingMaterial::CreateInputLayout(const GameContext& gameContext, ID3DX11EffectTechnique* pTechnique)
{
	if (m_pInputLayout)
	{
		return;
	}

	if (!EffectHelper::BuildInputLayout(
		gameContext.pDevice,
		pTechnique,
		&m_pInputLayout,
		m_pInputLayoutDescriptions, 
		m_pInputLayoutSize, 
		m_InputLayoutID))
	{
		Logger::LogWarning(L"PostProcessingMaterial: failed to create input layout!");
	}

}

void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	m_NumVertices = 4;
	if (m_pVertexBuffer)
	{
		return;
	}

	//Create vertex array containing three elements in system memory
	std::vector<VertexPosTex> vertices;

	for (int i = 0; i < m_NumVertices; i++)
	{
		vertices.push_back(VertexPosTex{ DirectX::XMFLOAT3{0,0,0},DirectX::XMFLOAT2{0,0} });
	}

	vertices[0].Position = DirectX::XMFLOAT3{ -1,1,0 };
	vertices[0].UV = DirectX::XMFLOAT2{ 0,0 };

	vertices[1].Position = DirectX::XMFLOAT3{ 1,1,0 };
	vertices[1].UV = DirectX::XMFLOAT2{ 1,0 };

	vertices[2].Position = DirectX::XMFLOAT3{ -1,-1,0 };
	vertices[2].UV = DirectX::XMFLOAT2{ 0,1 };

	vertices[3].Position = DirectX::XMFLOAT3{ 1,-1,0 };
	vertices[3].UV = DirectX::XMFLOAT2{ 1,1 };

	//fill a buffer description to copy the vertexdata into graphics memory
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = m_NumVertices * sizeof(VertexPosTex);
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.StructureByteStride = 0;


	//create a ID3D10Buffer in graphics memory containing the vertex info
	D3D11_SUBRESOURCE_DATA subSrcData;
	subSrcData.pSysMem = vertices.data();
	gameContext.pDevice->CreateBuffer(&bufferDesc, &subSrcData, &m_pVertexBuffer);


	
}

void PostProcessingMaterial::CreateIndexBuffer(const GameContext& /*gameContext*/)
{
	m_NumIndices = 6;

	D3D11_BUFFER_DESC bufferDesc {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(unsigned int)*m_NumIndices;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
}
