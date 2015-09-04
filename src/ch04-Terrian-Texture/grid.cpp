#include "grid.h"
#include "d3d/Geometry.h"

namespace byhj
{

void Grid::Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix)
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
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pGridVB, &stride, &offset);
	pD3D11DeviceContext->IASetIndexBuffer(m_pGridIB, DXGI_FORMAT_R32_UINT, 0);

	GridShader.use(pD3D11DeviceContext);
	pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);

}

void Grid::Shutdown()
{
	ReleaseCOM(m_pMVPBuffer)
	ReleaseCOM(m_pGridVB)
	ReleaseCOM(m_pGridIB)
	ReleaseCOM(m_pInputLayout)
}

void Grid::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	HRESULT hr;

	loadHeightMap("../../media/textures/heightmap01.bmp");
	d3d::Geometry geom;
	d3d::Geometry::MeshData gridMesh;
	geom.CreateGrid(160.0, 160.0, m_TerrainWidth, m_TerrainHeight, gridMesh);

	m_VertexCount = gridMesh.VertexData.size();
	for (int i = 0; i != m_VertexCount; ++i)
		gridMesh.VertexData[i].Pos.y = m_Hightmap[i].y / 10.0f;

	calcNormal(gridMesh);

	/////////////////////////////Vertex Buffer//////////////////////////////
	m_VertexCount = gridMesh.VertexData.size();
	D3D11_BUFFER_DESC gridVBDesc;
	gridVBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	gridVBDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	gridVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	gridVBDesc.CPUAccessFlags      = 0;
	gridVBDesc.MiscFlags           = 0;
	gridVBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA gridVBO;
	gridVBO.pSysMem = &gridMesh.VertexData[0];
	hr = pD3D11Device->CreateBuffer(&gridVBDesc, &gridVBO, &m_pGridVB);
	DebugHR(hr);

	/////////////////////////////Index Buffer//////////////////////////////

	m_IndexCount =  gridMesh.IndexData.size();
	D3D11_BUFFER_DESC gridIBDesc;
	gridIBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	gridIBDesc.ByteWidth           = sizeof(UINT) * m_IndexCount;
	gridIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	gridIBDesc.CPUAccessFlags      = 0;
	gridIBDesc.MiscFlags           = 0;
	gridIBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA girdIBO;
	girdIBO.pSysMem = &gridMesh.IndexData[0];
	hr = pD3D11Device->CreateBuffer(&gridIBDesc, &girdIBO, &m_pGridIB);
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
	lightDesc.ByteWidth      = sizeof(LightBuffer);
	lightDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	lightDesc.CPUAccessFlags = 0;
	lightDesc.MiscFlags      = 0;


	cbLight.ambient  = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	cbLight.diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbLight.lightDir = XMFLOAT3(-0.5f, -1.0f, 0.0f);
	cbLight.pad      = 0.0f;

	D3D11_SUBRESOURCE_DATA lightVBO;
	lightVBO.pSysMem = &cbLight;
	hr = pD3D11Device->CreateBuffer(&lightDesc, &lightVBO, &m_pLightBuffer);
	DebugHR(hr);
}


void Grid::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
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

	GridShader.init(pD3D11Device, hWnd);
	GridShader.attachVS(L"grid.vsh", vInputLayoutDesc);
	GridShader.attachPS(L"grid.psh");
	GridShader.end();
}

void Grid::loadHeightMap(const char *filename)
{
	BITMAPFILEHEADER  bitmapFileHeader;
	BITMAPINFOHEADER  bitmapInfoHeader;
	int imageSize;
	unsigned char *bitmapImage;
	unsigned char height;

	FILE *fp;
	int result =  fopen_s(&fp, filename, "rb");

	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	m_TerrainWidth  = bitmapInfoHeader.biWidth;
	m_TerrainHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	imageSize = m_TerrainWidth * m_TerrainHeight * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	// Move to the beginning of the bitmap data.
	fseek(fp, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	fread(bitmapImage, 1, imageSize, fp);
	fclose(fp);

	///////////////////////////////////////////////////////////////
	int k = 0, index;
	// Read the image data into the height map.
	for (int j = 0; j != m_TerrainHeight; ++j)
	{
		for (int i = 0; i != m_TerrainWidth; ++i)
		{
			height = bitmapImage[k];

			XMFLOAT3 Pos;
			Pos.x = (float)i;
			Pos.y = (float)height;
			Pos.z = (float)j;
			m_Hightmap.push_back(Pos);

			k += 3;
		}
	}

	// Release the bitmap image data.
	delete[] bitmapImage;
	bitmapImage = 0;
}

void Grid::calcNormal(d3d::Geometry::MeshData &mesh)
{
	for (int i = 0; i != mesh.IndexData.size(); i += 3)
	{
		int index1 = mesh.IndexData[i];
		int index2 = mesh.IndexData[i + 1];
		int index3 = mesh.IndexData[i + 2];
		XMFLOAT3 pos1 = mesh.VertexData[index1].Pos;
		XMFLOAT3 pos2 = mesh.VertexData[index2].Pos;
		XMFLOAT3 pos3 = mesh.VertexData[index3].Pos;

		XMVECTOR v1 = XMLoadFloat3(&pos1);
		XMVECTOR v2 = XMLoadFloat3(&pos2);
		XMVECTOR v3 = XMLoadFloat3(&pos3);

		XMVECTOR edge1 = v1 - v2;
		XMVECTOR edge2 = v2 - v3;

		XMVECTOR n1 = XMVector3Cross(edge1, edge2);
		XMVECTOR normal = XMVector3Normalize(n1);
		XMStoreFloat3(&mesh.VertexData[index1].Normal, normal);
		XMStoreFloat3(&mesh.VertexData[index2].Normal, normal);
		XMStoreFloat3(&mesh.VertexData[index3].Normal, normal);
	}

	for (int i = 0; i != mesh.VertexData.size(); ++i)
	{
		XMVECTOR n = XMLoadFloat3(&mesh.VertexData[i].Normal);
		XMVECTOR normal = XMVector3Normalize(n);
		XMStoreFloat3(&mesh.VertexData[i].Normal, normal);
	}

}

void Grid::init_texture(ID3D11Device *pD3D11Device)
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/dirt01.dds", NULL, NULL, &m_pTextureSRV, NULL);
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
