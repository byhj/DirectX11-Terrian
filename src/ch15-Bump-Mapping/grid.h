#ifndef Grid_H
#define Grid_H

#include <d3d11.h>

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


	void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix);
	void Shutdown();


private:
	void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
	void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	void init_texture(ID3D11Device *pD3D11Device);

	void loadHeightMap(const char *filename);
	void calcNormal(d3d::Geometry::MeshData &mesh);
	void CalcBump(d3d::Geometry::MeshData &mesh);

	d3d::MatrixBuffer cbMatrix;

	struct Vertex {
		// Position
		XMFLOAT3 Pos;
		// Normal
		XMFLOAT3 Normal;
		// TexCoords
		XMFLOAT2 Tex;

		XMFLOAT3 Tangent;

		XMFLOAT3 BiTangent;
	};

	d3d::Shader GridShader;
	std::vector<XMFLOAT3> m_Hightmap;
	std::vector<Vertex> m_VertexData;

	struct LightBuffer
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 lightDir;
		float    pad;
	};
	LightBuffer cbLight;
	ID3D11Buffer        *m_pGridVB      = nullptr;
	ID3D11Buffer        *m_pGridIB      = nullptr;
	ID3D11Buffer        *m_pMVPBuffer   = nullptr;
	ID3D11Buffer        *m_pLightBuffer   = nullptr;
	ID3D11InputLayout   *m_pInputLayout = nullptr;

	ID3D11ShaderResourceView *m_pTextureSRV;
	ID3D11SamplerState       *m_pTexSamplerState;
	ID3D11ShaderResourceView *m_pNormalTexSRV;

	int m_VertexCount  = 0;
	int m_IndexCount   = 0;
	int m_TerrainWidth = 0;
	int m_TerrainHeight= 0;
};

}
#endif