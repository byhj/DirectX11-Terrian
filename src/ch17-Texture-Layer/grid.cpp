#include "grid.h"
#include "d3d/d3dGeometry.h"

namespace byhj
{

void Grid::Render(ID3D11DeviceContext *pD3D11DeviceContext, const MatrixBuffer &matrix)
{
	//Update the the mvp matrix
	cbMatrix.model = matrix.model;
	cbMatrix.view  = matrix.view;
	cbMatrix.proj  = matrix.proj;
	pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0);
	pD3D11DeviceContext->VSSetConstantBuffers(0, 1, &m_pMVPBuffer);
	pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &m_pLightBuffer);

	pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pTextureSRV);
	pD3D11DeviceContext->PSSetShaderResources(1, 1, &m_pColor1TexSRV);
	pD3D11DeviceContext->PSSetShaderResources(2, 1, &m_pColor2TexSRV);
	pD3D11DeviceContext->PSSetShaderResources(3, 1, &m_pStoneTexSRV);
	pD3D11DeviceContext->PSSetShaderResources(4, 1, &m_pAlphaTexSRV);
	pD3D11DeviceContext->PSSetShaderResources(5, 1, &m_pNormalTex1SRV);
	pD3D11DeviceContext->PSSetShaderResources(6, 1, &m_pNormalTex2SRV);

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

	loadHeightMap("../../media/textures/hm.bmp");
	D3DGeometry geom;
	D3DGeometry::MeshData gridMesh;
	geom.CreateGrid(160.0, 160.0, m_TerrainWidth, m_TerrainHeight, gridMesh);

	m_VertexCount = gridMesh.VertexData.size();

	for (int i = 0; i != m_VertexCount; ++i)
		gridMesh.VertexData[i].Pos.y = m_Hightmap[i].y / 15.0f;

	calcNormal(gridMesh);
	CalcBump(gridMesh);

	/////////////////////////////Vertex Buffer//////////////////////////////

	D3D11_BUFFER_DESC gridVBDesc;
	gridVBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	gridVBDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	gridVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	gridVBDesc.CPUAccessFlags      = 0;
	gridVBDesc.MiscFlags           = 0;
	gridVBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA gridVBO;
	gridVBO.pSysMem = &m_VertexData[0];
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
	mvpDesc.ByteWidth      = sizeof(MatrixBuffer);
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


	cbLight.ambient  = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	cbLight.diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbLight.lightDir = XMFLOAT3(0.5f, 1.0f, 0.0f);
	cbLight.pad      = 0.0f;

	D3D11_SUBRESOURCE_DATA lightVBO;
	lightVBO.pSysMem = &cbLight;
	hr = pD3D11Device->CreateBuffer(&lightDesc, &lightVBO, &m_pLightBuffer);
	DebugHR(hr);
}


void Grid::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[5];
	pInputLayoutDesc[0].SemanticName         = "POSITION";
	pInputLayoutDesc[0].SemanticIndex        = 0;
	pInputLayoutDesc[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[0].InputSlot            = 0;
	pInputLayoutDesc[0].AlignedByteOffset    = 0;
	pInputLayoutDesc[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[0].InstanceDataStepRate = 0;

	pInputLayoutDesc[1].SemanticName         = "NORMAL";
	pInputLayoutDesc[1].SemanticIndex        = 0;
	pInputLayoutDesc[1].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[1].InputSlot            = 0;
	pInputLayoutDesc[1].AlignedByteOffset    = 12;
	pInputLayoutDesc[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[1].InstanceDataStepRate = 0;

	pInputLayoutDesc[2].SemanticName         = "TEXCOORD";
	pInputLayoutDesc[2].SemanticIndex        = 0;
	pInputLayoutDesc[2].Format               = DXGI_FORMAT_R32G32_FLOAT;
	pInputLayoutDesc[2].InputSlot            = 0;
	pInputLayoutDesc[2].AlignedByteOffset    = 24;
	pInputLayoutDesc[2].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[2].InstanceDataStepRate = 0;

	pInputLayoutDesc[3].SemanticName         = "TANGENT";
	pInputLayoutDesc[3].SemanticIndex        = 0;
	pInputLayoutDesc[3].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[3].InputSlot            = 0;
	pInputLayoutDesc[3].AlignedByteOffset    = 32;
	pInputLayoutDesc[3].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[3].InstanceDataStepRate = 0;

	pInputLayoutDesc[4].SemanticName         = "BITANGENT";
	pInputLayoutDesc[4].SemanticIndex        = 0;
	pInputLayoutDesc[4].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[4].InputSlot            = 0;
	pInputLayoutDesc[4].AlignedByteOffset    = 44;
	pInputLayoutDesc[4].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[4].InstanceDataStepRate = 0;
	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	GridShader.init(pD3D11Device, hWnd);
	GridShader.attachVS(L"grid.vsh", pInputLayoutDesc, numElements);
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

void Grid::calcNormal(D3DGeometry::MeshData &mesh)
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

		XMVECTOR addn = XMLoadFloat3(&mesh.VertexData[index1].Normal);
		addn = addn + n1;
		XMStoreFloat3(&mesh.VertexData[index1].Normal, addn);

		addn = XMLoadFloat3(&mesh.VertexData[index1].Normal);
		addn = addn + n1;
		XMStoreFloat3(&mesh.VertexData[index2].Normal, addn);

		addn = XMLoadFloat3(&mesh.VertexData[index1].Normal);
		addn = addn + n1;
		XMStoreFloat3(&mesh.VertexData[index3].Normal, addn);
	}
	m_VertexData.resize(mesh.VertexData.size());

	for (int i = 0; i != mesh.VertexData.size(); ++i)
	{
		XMVECTOR n = XMLoadFloat3(&mesh.VertexData[i].Normal);
		XMVECTOR normal = XMVector3Normalize(n);
		XMStoreFloat3(&mesh.VertexData[i].Normal, normal);

		m_VertexData[i].Pos = mesh.VertexData[i].Pos;
		m_VertexData[i].Normal = mesh.VertexData[i].Normal;
		m_VertexData[i].Tex = mesh.VertexData[i].Tex;
		m_VertexData[i].Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_VertexData[i].BiTangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
}

void Grid::CalcBump(D3DGeometry::MeshData &mesh)
{
	XMVECTOR v[3];
	XMVECTOR uv[3];
	int j = 0;
	for (int i = 0; i != mesh.IndexData.size(); i += 3)
	{
		int index1 = mesh.IndexData[i];
		int index2 = mesh.IndexData[i + 1];
		int index3 = mesh.IndexData[i + 2];
		XMFLOAT3 pos1 = mesh.VertexData[index1].Pos;
		XMFLOAT3 pos2 = mesh.VertexData[index2].Pos;
		XMFLOAT3 pos3 = mesh.VertexData[index3].Pos;

		XMFLOAT2 tex1 = mesh.VertexData[index1].Tex;
		XMFLOAT2 tex2 = mesh.VertexData[index2].Tex;
		XMFLOAT2 tex3 = mesh.VertexData[index3].Tex;

		XMFLOAT3 normal1 = mesh.VertexData[index1].Normal;
		XMFLOAT3 normal2 = mesh.VertexData[index2].Normal;
		XMFLOAT3 normal3 = mesh.VertexData[index3].Normal;

		v[0]  = XMLoadFloat3(&pos1);
		uv[0] = XMLoadFloat2(&tex1);

		v[1]  = XMLoadFloat3(&pos2);
		uv[1] = XMLoadFloat2(&tex2);

		v[2]  = XMLoadFloat3(&pos3);
		uv[2] = XMLoadFloat2(&tex3);
		// Edges of the triangle : postion delta
		XMVECTOR deltaPos1 = v[1] - v[0];
		XMVECTOR deltaPos2 = v[2] - v[0];
		XMVECTOR deltaUV1  = uv[1] - uv[0];
		XMVECTOR deltaUV2  = uv[2] - uv[0];

		float u1x = XMVectorGetX(deltaUV1);
		float u1y = XMVectorGetY(deltaUV1);
		float u2x = XMVectorGetX(deltaUV2);
		float u2y = XMVectorGetY(deltaUV2);

		float r = 1.0f / (u1x * u2y - u1y * u2x);
		XMVECTOR tangent = (deltaPos1 * u2y - deltaPos2 * u1y) * r;
		XMVECTOR bitangent = (deltaPos2 * u1x - deltaPos1 * u2x) * r;

		Vertex v1;
		XMStoreFloat3(&v1.Tangent, tangent);
		XMStoreFloat3(&v1.BiTangent, bitangent);

		m_VertexData[index1].Tangent = v1.Tangent;
		m_VertexData[index1].BiTangent = v1.BiTangent;
		m_VertexData[index2].Tangent = v1.Tangent;
		m_VertexData[index2].BiTangent = v1.BiTangent;
		m_VertexData[index3].Tangent = v1.Tangent;
		m_VertexData[index3].BiTangent = v1.BiTangent;
	}

	for (int i = 0; i != m_VertexData.size(); ++i)
	{
		XMVECTOR tan = XMLoadFloat3(&m_VertexData[i].Tangent);
		XMVECTOR tangent = XMVector3Normalize(tan);
		XMStoreFloat3(&m_VertexData[i].Tangent, tangent);

		XMVECTOR bitan = XMLoadFloat3(&m_VertexData[i].BiTangent);
		XMVECTOR bitangent = XMVector3Normalize(bitan);
		XMStoreFloat3(&m_VertexData[i].BiTangent, bitangent);
	}
}
void Grid::init_texture(ID3D11Device *pD3D11Device)
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/dirt001.dds", NULL, NULL, &m_pTextureSRV, NULL);
	DebugHR(hr);
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/dirt004.dds", NULL, NULL, &m_pColor1TexSRV, NULL);
	DebugHR(hr);
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/dirt002.dds", NULL, NULL, &m_pColor2TexSRV, NULL);
	DebugHR(hr);
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/stone001.dds", NULL, NULL, &m_pStoneTexSRV, NULL);
	DebugHR(hr);
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/alpha001.dds", NULL, NULL, &m_pAlphaTexSRV, NULL);
	DebugHR(hr);
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/normal001.dds", NULL, NULL, &m_pNormalTex1SRV, NULL);
	DebugHR(hr);
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/normal002.dds", NULL, NULL, &m_pNormalTex2SRV, NULL);
	DebugHR(hr);

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
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
