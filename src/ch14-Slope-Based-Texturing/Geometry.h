#ifndef Geometry_H
#define Geometry_H

#include <d3d11.h>
#include <xnamath.h>

#include "d3d/d3dDebug.h"
#include "d3d/d3dShader.h"
#include "d3d/d3dGeometry.h"
#include "D3DX11.h"

class Geometry
{
public:
	Geometry()
	{
		m_pGridVB       = NULL;
		m_pGridIB       = NULL;
		m_pMVPBuffer    = NULL;
		m_pInputLayout  = NULL;
		m_VertexCount   = 0;
		m_IndexCount    = 0;
	}
	~Geometry() {}

	void Render(ID3D11DeviceContext *pD3D11DeviceContext, XMMATRIX &model,  
		XMMATRIX &view, XMMATRIX &proj)
	{
		//Update the the mvp matrix
		cbMatrix.model = XMMatrixTranspose(model);	
		cbMatrix.view  = XMMatrixTranspose(view);	
		cbMatrix.proj  = XMMatrixTranspose(proj);
		pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
		pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

		pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &m_pLightBuffer);
		pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pTextureSRV);
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
	void CalcNormal(D3DGeometry::MeshData &mesh);
	int GetTriangle()
	{
		return m_IndexCount / 3;
	}

private:
	struct MatrixBuffer
	{
		XMMATRIX  model;
		XMMATRIX  view;
		XMMATRIX  proj;

	};
	MatrixBuffer cbMatrix;

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

	D3DShader GeometryShader;
	ID3D11Buffer             *m_pGridVB;
	ID3D11Buffer             *m_pGridIB;
	ID3D11Buffer             *m_pMVPBuffer;
	ID3D11Buffer             *m_pLightBuffer;
	ID3D11InputLayout        *m_pInputLayout;
	ID3D11ShaderResourceView *m_pTextureSRV;
	ID3D11SamplerState       *m_pTexSamplerState;

	int m_VertexCount;
	int m_IndexCount;

	int m_TerrainWidth;
	int m_TerrainHeight;
};

#endif