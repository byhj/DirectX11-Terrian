#ifndef _MINIMAPCLASS_H_
#define _MINIMAPCLASS_H_


#include "d3d/d3dBitmap.h"
#include <DirectXMath.h>

using namespace DirectX;

namespace byhj
{

class MiniMap
{
public:
	MiniMap();
	MiniMap(const MiniMap&);
	~MiniMap();

public:

	bool Init(ID3D11Device* device, HWND hwnd, int screenWidth, int screenHeight, 
		       float terrainWidth,  float terrainHeight, XMMATRIX viewMatrix);


	bool Render(ID3D11DeviceContext *pD3D11DeviceContext, const XMMATRIX &Model,  
		        const XMMATRIX &View, const XMMATRIX &Proj);

	void PositionUpdate(float positionX, float positionZ);

	void Shutdown();
private:
	int m_mapLocationX, m_mapLocationY, m_pointLocationX, m_pointLocationY;
	float m_mapSizeX, m_mapSizeY, m_terrainWidth, m_terrainHeight;
	XMMATRIX m_viewMatrix;

	D3DBitmap *m_pMiniMap, *m_pBorder, *m_pLocation;
};

}
#endif