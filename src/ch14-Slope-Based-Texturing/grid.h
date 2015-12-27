#ifndef Geometry_H
#define Geometry_H

#include <d3d11.h>


#include "d3d/Utility.h"
#include "d3d/Shader.h"
#include "d3d/Geometry.h"
#include "D3DX11.h"

namespace byhj
{


class Grid
{
public:
	Grid()
	{
		m_pGridVB       = NULL;
		m_pGridIB       = NULL;
		m_pMVPBuffer    = NULL;
		m_pInputLayout  = NULL;
		m_VertexCount   = 0;
		m_IndexCount    = 0;
	}
	~Grid() {}

	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const XMFLOAT4X4 &model,  
		const XMFLOAT4X4 &view, const XMFLOAT4X4 &proj)
	{
		//Update the the mvp matrix
		cbMatrix.model = model;
		cbMatrix.view = view;
		cbMatrix.proj = proj;
		pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
		pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

		pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &m_pLightBuffer);
		pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pGrassTexSRV);
		pD3D11DeviceContext->PSSetShaderResources(1, 1, &m_pSlopeTexSRV);
		pD3D11DeviceContext->PSSetShaderResources(2, 1, &m_pRockTexSRV);
		pD3D11DeviceContext->PSSetSamplers(0, 1, &m_pTexSamplerState);

		// Set vertex buffer stride and offset
		unsigned int stride;
		unsigned int offset;
		stride = sizeof(Vertex); 
		offset = 0;
		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pGridVB, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pGridIB, DXGI_FORMAT_R32_UINT, 0);

		GeometryShader.use(pD3D11DeviceContext);
		pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);

	}

	void shutdown()
	{
		ReleaseCOM(m_pMVPBuffer    )
		ReleaseCOM(m_pGridVB       )
		ReleaseCOM(m_pGridIB       )
		ReleaseCOM(m_pInputLayout  )
	}	

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
	ID3D11Buffer             *m_pGridVB;
	ID3D11Buffer             *m_pGridIB;
	ID3D11Buffer             *m_pMVPBuffer;
	ID3D11Buffer             *m_pLightBuffer;
	ID3D11InputLayout        *m_pInputLayout;
	ID3D11ShaderResourceView *m_pGrassTexSRV;
	ID3D11ShaderResourceView *m_pSlopeTexSRV;
	ID3D11ShaderResourceView *m_pRockTexSRV;
	ID3D11SamplerState       *m_pTexSamplerState;

	int m_VertexCount;
	int m_IndexCount;

	int m_TerrainWidth;
	int m_TerrainHeight;
};

}
#endif