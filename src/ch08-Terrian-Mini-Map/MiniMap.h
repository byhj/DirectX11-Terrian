#ifndef _MINIMAPCLASS_H_
#define _MINIMAPCLASS_H_


#include <DirectXMath.h>
#include <d3d11.h>

using namespace DirectX;

namespace byhj
{

class MiniMap
{
public:
	MiniMap() = default;
	MiniMap(const MiniMap&) = default;
	~MiniMap() = default;

public:

	bool Init(ID3D11Device* pD3D11Device, ID3D11DeviceContext pD3D11DeviceContext,  int screenWidth, int screenHeight, 
		       float terrainWidth,  float terrainHeight, XMMATRIX viewMatrix);


	bool Render(ID3D11DeviceContext *pD3D11DeviceContext, const XMMATRIX &Model,  
		        const XMMATRIX &View, const XMMATRIX &Proj);

	void Update(float positionX, float positionZ);

	void Shutdown();

private:
	int m_mapLocationX, m_mapLocationY, m_pointLocationX, m_pointLocationY;
	float m_mapSizeX, m_mapSizeY, m_terrainWidth, m_terrainHeight;
	
	XMFLOAT4X4 m_viewMatrix;

	d3d::Bitmap *m_pMiniMap  = nullptr;
    d3d::Bitmap *m_pBorder   = nullptr;
	d3d::Bitmap *m_pLocation = nullptr;
};

}
#endif