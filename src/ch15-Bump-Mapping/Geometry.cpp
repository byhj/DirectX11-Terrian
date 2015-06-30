#include "Geometry.h"
#include "d3d/d3dUtil.h"


void Geometry::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	HRESULT hr;

	loadHeightMap("../../media/textures/heightmap01.bmp");
	//NormalizeHeightMap();
	/////////////////////////////Vertex Buffer//////////////////////////////

	D3DGeometry geom;
	D3DGeometry::MeshData gridMesh;
	geom.CreateGrid(m_TerrainWidth, m_TerrainHeight, m_TerrainWidth, m_TerrainHeight, gridMesh);

	m_VertexCount = gridMesh.VertexData.size();
	for (int i = 0; i != m_VertexCount; ++i)
	    gridMesh.VertexData[i].Pos.y = m_Hightmap[i].y / 5.0f;
	CalcNormal(gridMesh);
	CalcBump(gridMesh);

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

	cbLight.ambient  = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	cbLight.diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbLight.lightDir = XMFLOAT3(-0.5f, -1.0f, 0.0f);
	cbLight.pad      = 0.0f;

	D3D11_SUBRESOURCE_DATA lightVBO;
	lightVBO.pSysMem = &cbLight;
	hr = pD3D11Device->CreateBuffer(&lightDesc, &lightVBO, &m_pLightBuffer);
	DebugHR(hr);
}

void Geometry::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	//Shader interface information
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

	GeometryShader.init(pD3D11Device, hWnd);
	GeometryShader.attachVS(L"grid.vsh", pInputLayoutDesc, numElements);
	GeometryShader.attachPS(L"grid.psh");
	GeometryShader.end();
}

void Geometry::loadHeightMap(const char *filename)
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
	for(int j = 0; j != m_TerrainHeight; ++j)
	{
		for(int i = 0; i != m_TerrainWidth; ++i)
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
	delete [] bitmapImage;
	bitmapImage = 0;
}


void Geometry::CalcNormal(D3DGeometry::MeshData &mesh)
{
	for (int i = 0; i != mesh.IndexData.size(); i += 3)
	{
		int index1 = mesh.IndexData[i];
		int index2 = mesh.IndexData[i+1];
		int index3 = mesh.IndexData[i+2];
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

void Geometry::CalcBump(D3DGeometry::MeshData &mesh)
{
	XMVECTOR v[3];
	XMVECTOR uv[3];
	int j = 0;
	for (int i = 3; i != mesh.VertexData.size(); i += 3)
	{
		v[j++ % 3]  = XMLoadFloat3(&mesh.VertexData[i-2].Pos);
		uv[j++ % 3] = XMLoadFloat2(&mesh.VertexData[i-2].Tex);

		v[j++ % 3]  = XMLoadFloat3(&mesh.VertexData[i-1].Pos);
		uv[j++ % 3] = XMLoadFloat2(&mesh.VertexData[i-1].Tex);

		v[j++ % 3]  = XMLoadFloat3(&mesh.VertexData[i].Pos);
		uv[j++ % 3] = XMLoadFloat2(&mesh.VertexData[i].Tex);

		// Edges of the triangle : postion delta
		XMVECTOR deltaPos1 = v[1]  - v[0];
		XMVECTOR deltaPos2 = v[2]  - v[0];
		XMVECTOR deltaUV1  = uv[1] - uv[0];
		XMVECTOR deltaUV2  = uv[2] - uv[0];

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		XMVECTOR tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y) * r;
		XMVECTOR bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x) * r;
        Vertex v1;
		v1.Pos = mesh.VertexData[i-2].Pos;
		v1.Pos = mesh.VertexData[i-2].Normal;
		v1.Tex = mesh.VertexData[i-2].Tex;

		XMStoreFloat3(&v1.Tangent, tangent);
		XMStoreFloat3(&v1.BiTangent, bitangent);
		m_VertexData.push_back(v1);
		v1.Pos = mesh.VertexData[i-1].Pos;
		v1.Pos = mesh.VertexData[i-1].Normal;
		v1.Tex = mesh.VertexData[i-1].Tex;
		XMStoreFloat3(&v1.Tangent, tangent);
		XMStoreFloat3(&v1.BiTangent, bitangent);
		m_VertexData.push_back(v1);

		v1.Pos = mesh.VertexData[i].Pos;
		v1.Pos = mesh.VertexData[i].Normal;
		v1.Tex = mesh.VertexData[i].Tex;
		XMStoreFloat3(&v1.Tangent, tangent);
		XMStoreFloat3(&v1.BiTangent, bitangent);
		m_VertexData.push_back(v1);
	}
}

void Geometry::init_texture(ID3D11Device *pD3D11Device)
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/dirt01.dds", NULL,NULL, &m_pTextureSRV, NULL);
	DebugHR(hr);

	hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, L"../../media/textures/bump.dds", NULL,NULL, &m_pNormalTexSRV, NULL);
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