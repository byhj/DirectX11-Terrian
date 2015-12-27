#ifndef Geometry_H
#define Geometry_H

#include <d3d11.h>


#include "d3d/Utility.h"
#include "d3d/Shader.h"
#include "d3d/Geometry.h"

namespace byhj
{


class Grid
{
public:
	Grid()  = default;
	~Grid() = default;

	void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const XMFLOAT4X4 &model, const XMFLOAT4X4 &view, const XMFLOAT4X4 &proj);
	void Shutdown();

private:

	void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
	void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	void init_texture(ID3D11Device *pD3D11Device);

	void loadHeightMap(const char *filename);
	void CalcNormal(d3d::Geometry::MeshData &mesh);
	int GetTriangle()
	{
		return m_IndexCount / 3;
	}

private:

	d3d::MatrixBuffer cbMatrix;

	struct LightBuffer
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 lightDir;
		float    pad;
	};
	LightBuffer cbLight;

	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};

	std::vector<XMFLOAT3> m_Hightmap;

	d3d::Shader GeometryShader;
	ID3D11Buffer             *m_pGridVB          = nullptr;
	ID3D11Buffer             *m_pGridIB          = nullptr;
	ID3D11Buffer             *m_pMVPBuffer       = nullptr;
	ID3D11Buffer             *m_pLightBuffer     = nullptr;
	ID3D11InputLayout        *m_pInputLayout     = nullptr;
	ID3D11ShaderResourceView *m_pGrassTexSRV     = nullptr;
	ID3D11ShaderResourceView *m_pSlopeTexSRV     = nullptr;
	ID3D11ShaderResourceView *m_pRockTexSRV      = nullptr;
	ID3D11SamplerState       *m_pTexSamplerState = nullptr;

	int m_VertexCount;
	int m_IndexCount;

	int m_TerrainWidth;
	int m_TerrainHeight;
};

}
#endif