#ifndef Skymap_H
#define Skymap_H

#include "d3d/Shader.h"
#include "d3d/Utility.h"

#include <vector>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <DirectXMath.h> 
using namespace DirectX;

namespace byhj
{


class Skymap
{
public:
	Skymap() {}

	void createSphere(ID3D11Device *pD3D11Device, int LatLines, int LongLines);
	void load_texture(ID3D11Device *pD3D11Device, WCHAR *texFile);
	void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);
	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &mvpMatrix);

private:

	struct Vertex	//Overloaded Vertex Structure
	{
		Vertex(){}
		Vertex(float x, float y, float z)
			: pos(x,y,z){}

		XMFLOAT3 pos;
	};


	d3d::MatrixBuffer cbMatrix;

	ComPtr<ID3D11Buffer> m_pIndexBuffer;
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pMVPBuffer;

	int m_VertexCount = 0;
	int m_IndexCount = 0;

	int NumSphereVertices;
	int NumSphereFaces;

	ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
	ComPtr<ID3D11SamplerState>       m_pTexSamplerState;
	d3d::Shader SkymapShader;
};



}

#endif