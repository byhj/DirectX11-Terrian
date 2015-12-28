#include "skyPlane.h"
#include "DirectXTK/DDSTextureLoader.h"

namespace byhj
{



void SkyPlane::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	init_buffer(pD3D11Device, pD3D11DeviceContext);
	init_shader(pD3D11Device, hWnd);
	init_texture(pD3D11Device);
}

void SkyPlane::Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix)
{
	//Update the the mvp matrix
	cbMatrix.model = matrix.model;
	cbMatrix.view  = matrix.view;
	cbMatrix.proj  = matrix.proj;
	pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0);
	pD3D11DeviceContext->VSSetConstantBuffers(0, 1, &m_pMVPBuffer);
	pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &m_pLightBuffer);
	pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pTextureSRV);
	pD3D11DeviceContext->PSSetSamplers(0, 1, &m_pTexSamplerState);

	// Set vertex buffer stride and offset
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(Vertex);
	offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pSkyPlaneVB, &stride, &offset);
	pD3D11DeviceContext->IASetIndexBuffer(m_pSkyPlaneIB, DXGI_FORMAT_R32_UINT, 0);

	SkyPlaneShader.use(pD3D11DeviceContext);
	pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);

}

void SkyPlane::Shutdown()
{
	ReleaseCOM(m_pMVPBuffer)
	ReleaseCOM(m_pSkyPlaneVB)
	ReleaseCOM(m_pSkyPlaneIB)
	ReleaseCOM(m_pInputLayout)
}

void SkyPlane::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	HRESULT hr;


	/////////////////////////////Vertex Buffer//////////////////////////////

	D3D11_BUFFER_DESC SkyPlaneVBDesc;
	SkyPlaneVBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	SkyPlaneVBDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	SkyPlaneVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	SkyPlaneVBDesc.CPUAccessFlags      = 0;
	SkyPlaneVBDesc.MiscFlags           = 0;
	SkyPlaneVBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA SkyPlaneVBO;
	SkyPlaneVBO.pSysMem = &m_VertexData[0];
	hr = pD3D11Device->CreateBuffer(&SkyPlaneVBDesc, &SkyPlaneVBO, &m_pSkyPlaneVB);
	DebugHR(hr);

	/////////////////////////////Index Buffer//////////////////////////////

	m_IndexCount =  SkyPlaneMesh.IndexData.size();
	D3D11_BUFFER_DESC SkyPlaneIBDesc;
	SkyPlaneIBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	SkyPlaneIBDesc.ByteWidth           = sizeof(UINT) * m_IndexCount;
	SkyPlaneIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	SkyPlaneIBDesc.CPUAccessFlags      = 0;
	SkyPlaneIBDesc.MiscFlags           = 0;
	SkyPlaneIBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA girdIBO;
	girdIBO.pSysMem = &SkyPlaneMesh.IndexData[0];
	hr = pD3D11Device->CreateBuffer(&SkyPlaneIBDesc, &girdIBO, &m_pSkyPlaneIB);
	DebugHR(hr);

	////////////////////////////////Const Buffer//////////////////////////////////////

	D3D11_BUFFER_DESC mvpDesc;
	ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));
	mvpDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpDesc.ByteWidth      = sizeof(d3d::MatrixBuffer);
	mvpDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpDesc.CPUAccessFlags = 0;
	mvpDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&mvpDesc, NULL, &m_pMVPBuffer);
	DebugHR(hr);

	D3D11_BUFFER_DESC lightDesc;
	ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));
	lightDesc.Usage          = D3D11_USAGE_DEFAULT;
	lightDesc.ByteWidth      = sizeof(SkyPlaneBuffer);
	lightDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	lightDesc.CPUAccessFlags = 0;
	lightDesc.MiscFlags      = 0;


	cbLight.ambient  = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);

	D3D11_SUBRESOURCE_DATA lightVBO;
	lightVBO.pSysMem = &cbLight;
	hr = pD3D11Device->CreateBuffer(&lightDesc, &lightVBO, &m_pSkyPlaneBuffer);
	DebugHR(hr);
}


void SkyPlane::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	//Shader interface information
	std::vector<D3D11_INPUT_ELEMENT_DESC> vInputLayoutDesc;
	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc;

	pInputLayoutDesc.SemanticName         = "POSITION";
	pInputLayoutDesc.SemanticIndex        = 0;
	pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc.InputSlot            = 0;
	pInputLayoutDesc.AlignedByteOffset    = 0;
	pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(pInputLayoutDesc);

	pInputLayoutDesc.SemanticName         = "NORMAL";
	pInputLayoutDesc.SemanticIndex        = 0;
	pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc.InputSlot            = 0;
	pInputLayoutDesc.AlignedByteOffset    = 12;
	pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(pInputLayoutDesc);

	pInputLayoutDesc.SemanticName         = "TEXCOORD";
	pInputLayoutDesc.SemanticIndex        = 0;
	pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32_FLOAT;
	pInputLayoutDesc.InputSlot            = 0;
	pInputLayoutDesc.AlignedByteOffset    = 24;
	pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(pInputLayoutDesc);

	pInputLayoutDesc.SemanticName         = "COLOR";
	pInputLayoutDesc.SemanticIndex        = 0;
	pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc.InputSlot            = 0;
	pInputLayoutDesc.AlignedByteOffset    = 32;
	pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(pInputLayoutDesc);


	SkyPlaneShader.init(pD3D11Device, vInputLayoutDesc);
	SkyPlaneShader.attachVS(L"SkyPlane.vsh", "VS", "vs_5_0");
	SkyPlaneShader.attachPS(L"SkyPlane.psh", "PS", "ps_5_0");
	SkyPlaneShader.end();
}

void SkyPlane::init_texture(ID3D11Device *pD3D11Device)
{
	HRESULT hr;
	hr = CreateDDSTextureFromFile(pD3D11Device, L"../../media/textures/dirt01.dds", NULL, &m_pTextureSRV, NULL);
	DebugHR(hr);

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	hr = pD3D11Device->CreateSamplerState(&samplerDesc, &m_pTexSamplerState);
	DebugHR(hr);
}



}