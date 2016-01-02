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

void SkyPlane::Update()
{
	m_SkyPlaneBuffer.Translation += 0.0001f;
	if (m_SkyPlaneBuffer.Translation  > 1.0f)
	{
		m_SkyPlaneBuffer.Translation  -= 1.0f;
	}


}

void SkyPlane::Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix)
{
	//Update the the mvp matrix
	cbMatrix.model = matrix.model;
	cbMatrix.view  = matrix.view;
	cbMatrix.proj  = matrix.proj;
	pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0);
	pD3D11DeviceContext->VSSetConstantBuffers(0, 1, &m_pMVPBuffer);

	//update data
	pD3D11DeviceContext->UpdateSubresource(m_pSkyPlaneBuffer,  0, NULL, &m_SkyPlaneBuffer, 0, 0);
	pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &m_pSkyPlaneBuffer);

	pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pCloudTexSRV1);
	pD3D11DeviceContext->PSSetShaderResources(1, 1, &m_pCloudTexSRV2);
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
	float skyPlaneWidth = 10.0f;
	float skyPlaneTop = 0.5f;
	float skyPlaneBottom = 0.0f;

	int skyPlaneResolution = 10;
	int textureRepeat = 4;

	m_SkyPlaneBuffer.Brightness  = 0.5f;
	m_SkyPlaneBuffer.Scale = 0.3f;
	m_SkyPlaneBuffer.Translation = 0.0f;


	float quadSize, radius, constant, textureDelta;
	int i, j, index;
	float positionX, positionY, positionZ, tu, tv;


	// Create the array to hold the sky plane coordinates.
	m_pSkyPlaneVertex = new SkyPlaneType[(skyPlaneResolution + 1) * (skyPlaneResolution + 1)];

	// Determine the size of each quad on the sky plane.
	quadSize = skyPlaneWidth / (float)skyPlaneResolution;

	// Calculate the radius of the sky plane based on the width.
	radius = skyPlaneWidth / 2.0f;

	// Calculate the height constant to increment by.
	constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

	// Calculate the texture coordinate increment value.
	textureDelta = (float)textureRepeat / (float)skyPlaneResolution;

	// Loop through the sky plane and build the coordinates based on the increment values given.
	for (j=0; j <= skyPlaneResolution; j++)
	{
		for (i=0; i <= skyPlaneResolution; i++)
		{
			// Calculate the vertex coordinates.
			positionX = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
			positionZ = (-0.5f * skyPlaneWidth) + ((float)j * quadSize);
			positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ * positionZ)));

			// Calculate the texture coordinates.
			tu = (float)i * textureDelta;
			tv = (float)j * textureDelta;

			// Calculate the index into the sky plane array to add this coordinate.
			index = j * (skyPlaneResolution + 1) + i;

			// Add the coordinates to the sky plane array.
			m_pSkyPlaneVertex[index].x = positionX;
			m_pSkyPlaneVertex[index].y = positionY;
			m_pSkyPlaneVertex[index].z = positionZ;
			m_pSkyPlaneVertex[index].u = tu;
			m_pSkyPlaneVertex[index].v = tv;
		}
	}




	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index1, index2, index3, index4;


	// Calculate the number of pVertexData in the sky plane mesh.
	m_VertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1) * 6;

	// Set the index count to the same as the vertex count.
	m_IndexCount = m_VertexCount;

	// Create the vertex array.
	std::vector<Vertex> pVertexData(m_VertexCount);


	// Create the index array.
	std::vector<UINT> pIndexData(m_IndexCount, 0);

	// Initialize the index into the vertex array.
	index = 0;

	// Load the vertex and index array with the sky plane array data.
	for (j=0; j < skyPlaneResolution; j++)
	{
		for (i=0; i < skyPlaneResolution; i++)
		{
			index1 = j * (skyPlaneResolution + 1) + i;
			index2 = j * (skyPlaneResolution + 1) + (i + 1);
			index3 = (j + 1) * (skyPlaneResolution + 1) + i;
			index4 = (j + 1) * (skyPlaneResolution + 1) + (i + 1);

			// Triangle 1 - Upper Left
			pVertexData[index].Pos = XMFLOAT3(m_pSkyPlaneVertex[index1].x, m_pSkyPlaneVertex[index1].y, m_pSkyPlaneVertex[index1].z);
			pVertexData[index].Tex = XMFLOAT2(m_pSkyPlaneVertex[index1].u, m_pSkyPlaneVertex[index1].v);
			pIndexData[index] = index;
			index++;

			// Triangle 1 - Upper Right
			pVertexData[index].Pos = XMFLOAT3(m_pSkyPlaneVertex[index2].x, m_pSkyPlaneVertex[index2].y, m_pSkyPlaneVertex[index2].z);
			pVertexData[index].Tex= XMFLOAT2(m_pSkyPlaneVertex[index2].u, m_pSkyPlaneVertex[index2].v);
			pIndexData[index] = index;
			index++;

			// Triangle 1 - Bottom Left
			pVertexData[index].Pos = XMFLOAT3(m_pSkyPlaneVertex[index3].x, m_pSkyPlaneVertex[index3].y, m_pSkyPlaneVertex[index3].z);
			pVertexData[index].Tex= XMFLOAT2(m_pSkyPlaneVertex[index3].u, m_pSkyPlaneVertex[index3].v);
			pIndexData[index] = index;
			index++;

			// Triangle 2 - Bottom Left
			pVertexData[index].Pos = XMFLOAT3(m_pSkyPlaneVertex[index3].x, m_pSkyPlaneVertex[index3].y, m_pSkyPlaneVertex[index3].z);
			pVertexData[index].Tex= XMFLOAT2(m_pSkyPlaneVertex[index3].u, m_pSkyPlaneVertex[index3].v);
			pIndexData[index] = index;
			index++;

			// Triangle 2 - Upper Right
			pVertexData[index].Pos = XMFLOAT3(m_pSkyPlaneVertex[index2].x, m_pSkyPlaneVertex[index2].y, m_pSkyPlaneVertex[index2].z);
			pVertexData[index].Tex= XMFLOAT2(m_pSkyPlaneVertex[index2].u, m_pSkyPlaneVertex[index2].v);
			pIndexData[index] = index;
			index++;

			// Triangle 2 - Bottom Right
			pVertexData[index].Pos = XMFLOAT3(m_pSkyPlaneVertex[index4].x, m_pSkyPlaneVertex[index4].y, m_pSkyPlaneVertex[index4].z);
			pVertexData[index].Tex= XMFLOAT2(m_pSkyPlaneVertex[index4].u, m_pSkyPlaneVertex[index4].v);
			pIndexData[index] = index;
			index++;
		}
	}


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
	SkyPlaneVBO.pSysMem = &pVertexData[0];
	hr = pD3D11Device->CreateBuffer(&SkyPlaneVBDesc, &SkyPlaneVBO, &m_pSkyPlaneVB);
	DebugHR(hr);

	/////////////////////////////Index Buffer//////////////////////////////

	D3D11_BUFFER_DESC SkyPlaneIBDesc;
	SkyPlaneIBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	SkyPlaneIBDesc.ByteWidth           = sizeof(UINT) * m_IndexCount;
	SkyPlaneIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	SkyPlaneIBDesc.CPUAccessFlags      = 0;
	SkyPlaneIBDesc.MiscFlags           = 0;
	SkyPlaneIBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA girdIBO;
	girdIBO.pSysMem = &pIndexData[0];
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

	D3D11_BUFFER_DESC skyPlaneDesc;
	ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));
	skyPlaneDesc.Usage          = D3D11_USAGE_DEFAULT;
	skyPlaneDesc.ByteWidth      = sizeof(SkyPlaneBuffer);
	skyPlaneDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	skyPlaneDesc.CPUAccessFlags = 0;
	skyPlaneDesc.MiscFlags      = 0;


	D3D11_SUBRESOURCE_DATA lightVBO;
	lightVBO.pSysMem = &m_SkyPlaneBuffer;
	hr = pD3D11Device->CreateBuffer(&skyPlaneDesc, &lightVBO, &m_pSkyPlaneBuffer);
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

	pInputLayoutDesc.SemanticName         = "TEXCOORD";
	pInputLayoutDesc.SemanticIndex        = 0;
	pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32_FLOAT;
	pInputLayoutDesc.InputSlot            = 0;
	pInputLayoutDesc.AlignedByteOffset    = 12;
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
	hr = CreateDDSTextureFromFile(pD3D11Device, L"../../media/textures/cloud001.dds", NULL, &m_pCloudTexSRV1, NULL);
	DebugHR(hr);
	hr = CreateDDSTextureFromFile(pD3D11Device, L"../../media/textures//perturb001.dds", NULL, &m_pCloudTexSRV2, NULL);
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