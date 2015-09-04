#ifndef Grid_H
#define Grid_H

#include <d3d11.h>
#include <xnamath.h>
#include <vector>


#include "d3d/Shader.h"
#include "d3d/Utility.h"
#include "d3d/Geometry.h"

namespace byhj
{


class Grid
{
public:
	Grid() {}
	~Grid() {}

	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix);
	void Shutdown();

	void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
	void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	void init_texture(ID3D11Device *pD3D11Device);

private:
	void loadHeightMap(const char *filename);
	void loadColorMap(const char *filename);

	void calcNormal(d3d::Geometry::MeshData &mesh);

	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT3 Color;
	};


	struct LightBuffer
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 lightDir;
		float    pad;
	};

	d3d::MatrixBuffer cbMatrix;
	d3d::Shader GridShader;
	LightBuffer cbLight;
	std::vector<Vertex> m_VertexData;
	std::vector<XMFLOAT3> m_Hightmap;	
	std::vector<XMFLOAT3> m_Colormap;


	ID3D11Buffer        *m_pGridVB      = nullptr;
	ID3D11Buffer        *m_pGridIB      = nullptr;
	ID3D11Buffer        *m_pMVPBuffer   = nullptr;
	ID3D11Buffer        *m_pLightBuffer   = nullptr;
	ID3D11InputLayout   *m_pInputLayout = nullptr;

	ID3D11ShaderResourceView *m_pTextureSRV;
	ID3D11SamplerState       *m_pTexSamplerState;

	int m_VertexCount  = 0;
	int m_IndexCount   = 0;
	int m_TerrainWidth = 0;
	int m_TerrainHeight= 0;
};

}
#endif