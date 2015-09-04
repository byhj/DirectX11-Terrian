
#include "minimap.h"

MiniMap::MiniMap()
{
	m_pMiniMap  = 0;
	m_pBorder   = 0;
	m_pLocation = 0;
}


MiniMap::MiniMap(const MiniMap& other)
{
}


MiniMap::~MiniMap()
{
}


bool MiniMap::Init(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight, 
						float terrainWidth,  float terrainHeight, XMMATRIX viewMatrix)
{
	bool result;

	m_mapLocationX = 150;
	m_mapLocationY = 75;
	m_mapSizeX = 150.0f;
	m_mapSizeY = 150.0f;

	m_viewMatrix = viewMatrix;

	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;

	m_pMiniMap = new D3DBitmap;
	result = m_pMiniMap->Init(device, screenWidth, screenHeight, L"../Engine/data/colorm01.dds", 150, 150);

	m_pBorder = new D3DBitmap;
	result = m_pBorder->Init(device, screenWidth, screenHeight, L"../Engine/data/border01.dds", 154, 154);

	m_pLocation = new D3DBitmap;
	result = m_pLocation->Init(device, screenWidth, screenHeight, L"../Engine/data/point01.dds", 3, 3);


	return true;
}


void MiniMap::Shutdown()
{
	// Release the point bitmap object.
	if(m_pLocation)
	{
		m_pLocation->shutdown();
		delete m_pLocation;
		m_pLocation = 0;
	}

	// Release the border bitmap object.
	if(m_pBorder)
	{
		m_pBorder->shutdown();
		delete m_pBorder;
		m_pBorder = 0;
	}

	// Release the mini-map bitmap object.
	if(m_pMiniMap)
	{
		m_pMiniMap>shutdown();
		delete m_pMiniMap;
		m_pMiniMap = 0;
	}

	return;
}


bool MiniMap::Render(ID3D11DeviceContext *pD3D11DeviceContext, const XMMATRIX &Model,  
						  const XMMATRIX &View, const XMMATRIX &Proj)
{
    m_pMiniMap->Render(pD3D11DeviceContext, Model, View, Proj);
	m_pBorder->Render(pD3D11DeviceContext, Model, View, Proj);
	m_pLocation->Render(pD3D11DeviceContext, Model, View, Proj);

	return true;
}

void MiniMap::PositionUpdate(float positionX, float positionZ)
{
	float percentX, percentY;
	// Ensure the point does not leave the minimap borders even if the camera goes past the terrain borders.
	if(positionX < 0)
	{
		positionX = 0;
	}

	if(positionZ < 0)
	{
		positionZ = 0;
	}

	if(positionX > m_terrainWidth)
	{
		positionX = m_terrainWidth;
	}

	if(positionZ > m_terrainHeight)
	{
		positionZ = m_terrainHeight;
	}

	// Calculate the position of the camera on the minimap in terms of percentage.
	percentX = positionX / m_terrainWidth;
	percentY = 1.0f - (positionZ / m_terrainHeight);

	// Determine the pixel location of the point on the mini-map.
	m_pointLocationX = m_mapLocationX + (int)(percentX * m_mapSizeX);
	m_pointLocationY = m_mapLocationY + (int)(percentY * m_mapSizeY);

	// Subtract one from the location to center the point on the mini-map according to the 3x3 point pixel image size.
	m_pointLocationX = m_pointLocationX - 1;
	m_pointLocationY = m_pointLocationY - 1;

	return;
}
