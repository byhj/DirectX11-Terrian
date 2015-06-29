#include "Geometry.h"
#include "d3d/d3dUtil.h"

void Geometry::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	HRESULT hr;

	loadHeightMap("../../media/heightmap01.bmp");
	//NormalizeHeightMap();
	/////////////////////////////Vertex Buffer//////////////////////////////

	D3DGeometry geom;
	D3DGeometry::MeshData gridMesh;
	geom.CreateGrid(160.0, 160.0, m_TerrainWidth, m_TerrainHeight, gridMesh);

	m_VertexCount = gridMesh.VertexData.size();
	for (int i = 0; i != m_VertexCount; ++i)
	    gridMesh.VertexData[i].Pos.y = m_Hightmap[i].y / 10.0f;

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
	mvpDesc.ByteWidth      = sizeof(MatrixBuffer);
	mvpDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpDesc.CPUAccessFlags = 0;
	mvpDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&mvpDesc, NULL, &m_pMVPBuffer);
	DebugHR(hr);
}

void Geometry::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	//Shader interface information
	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[3];
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


void Geometry::NormalizeHeightMap()
{
	for(int j = 0; j < m_TerrainHeight; j++)
		for(int i = 0; i < m_TerrainWidth;  i++)
			m_Hightmap[(m_TerrainHeight * j) + i].y /= 15.0f;
}