#ifndef _MINIMAPCLASS_H_
#define _MINIMAPCLASS_H_


#include <DirectXMath.h>
#include <d3d11.h>
#include "Bitmap.h"

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

	bool Init(ID3D11Device* pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext,  HWND hwnd, 
		       int screenWidth, int screenHeight, float terrainWidth,  float terrainHeight);

	bool Render(ID3D11DeviceContext *pD3D11DeviceContext, const XMMATRIX &Model,  
		        const XMMATRIX &View, const XMMATRIX &Proj);

	void Update(float posX, float posZ);

	void Shutdown();

	void SetBack(int sw, int sh, int posX, int posY, int width, int height, WCHAR *texFile);
	void SetBorder(int sw, int sh, int posX, int posY, int width, int height, WCHAR *texFile);
	void SetLocation(int sw, int sh, int posX, int posY, int width, int height, WCHAR *texFile);
	
private:
	
	XMFLOAT4X4 m_viewMatrix;

	Bitmap *m_pBack     = nullptr;
	Bitmap *m_pBorder   = nullptr;
	Bitmap *m_pLocation = nullptr;
};

}
#endif