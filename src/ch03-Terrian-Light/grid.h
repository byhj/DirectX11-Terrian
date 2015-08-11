#ifndef Grid_H
#define Grid_H

#include <d3d11.h>
#include <xnamath.h>
#include <vector>

#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"
#include "d3d/d3dUtility.h"
#include "d3d/d3dGeometry.h"

namespace byhj
{


class Grid
{
public:
	Grid() {}
	~Grid() {}

	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const MatrixBuffer &matrix);
	void Shutdown();

	void init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
	void init_shader (ID3D11Device *pD3D11Device, HWND hWnd);

private:
	void loadHeightMap(const char *filename);
	void calcNormal(D3DGeometry::MeshData &mesh);

	byhj::MatrixBuffer cbMatrix;

	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};

	byhj::Shader GridShader;
	std::vector<XMFLOAT3> m_Hightmap;
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

	int m_VertexCount  = 0;
	int m_IndexCount   = 0;
	int m_TerrainWidth = 0;
	int m_TerrainHeight= 0;
};

}
#endif