
#include "minimap.h"


namespace byhj
{


bool MiniMap::Init(ID3D11Device* pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hwnd, 
	                int screenWidth, int screenHeight, float terrainWidth, float terrainHeight)

{


	return true;
}


void MiniMap::Shutdown()
{

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

}