#ifndef D3DCUBEMAP_H
#define D3DCUBEMAP_H

#include "d3d/d3dShader.h"
#include "d3d/d3dDebug.h"
#include "d3d/d3dUtility.h"

#include <windows.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <vector>

namespace byhj
{

class D3DSkymap
{
public:
	D3DSkymap() {}

	void createSphere(ID3D11Device *pD3D11Device, int LatLines, int LongLines);
	void load_texture(ID3D11Device *pD3D11Device, WCHAR *texFile);
	void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);
	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const byhj::MatrixBuffer &matrix);

private:

	struct Vertex	//Overloaded Vertex Structure
	{
		Vertex(){}
		Vertex(float x, float y, float z)
			: pos(x,y,z){}

		XMFLOAT3 pos;
	};

	struct MatrixBuffer
	{
		XMFLOAT4X4  model;
		XMFLOAT4X4  view;
		XMFLOAT4X4  proj;
	};
	MatrixBuffer m_Matrix;

	ID3D11Buffer *m_pIndexBuffer;
	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Buffer *m_pMVPBuffer;

	int m_VertexCount;
	int m_IndexCount;
	XMMATRIX sphereWorld;
	XMMATRIX Rotationx;
	XMMATRIX Rotationy;

	int NumSphereVertices;
	int NumSphereFaces;

	ID3D11ShaderResourceView *m_pShaderResourceView;
	ID3D11DepthStencilState  *m_pDSLessEqual;
	ID3D11RasterizerState    *m_pRSCullNone;
	ID3D11SamplerState       *m_pTexSamplerState;
	byhj::Shader SkymapShader;
};


}

#endif