#ifndef D3DBITMAP_H
#define D3DBITMAP_H

#include <d3dx11.h>
#include <xnamath.h>


#include "d3d/Shader.h"

class D3DBitmap
{
public:
	D3DBitmap()
	{
		m_pInputLayout        = NULL;
		m_pMVPBuffer          = NULL;
		m_pVertexBuffer       = NULL;
		m_pIndexBuffer        = NULL;
	}

	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const XMMATRIX &Model,  
		        const XMMATRIX &View, const XMMATRIX &Proj)
	{
		cbMatrix.model  = XMMatrixTranspose(Model);
		cbMatrix.view   = XMMatrixTranspose(View);
		cbMatrix.proj   = XMMatrixTranspose(Proj);
		pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
		pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

		unsigned int stride;
		unsigned int offset;
		stride = sizeof(Vertex); 
		offset = 0;

		pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pTextureSRV);  
		pD3D11DeviceContext->PSSetSamplers( 0, 1, &m_pTexSamplerState );

		D3DRTTShader.use(pD3D11DeviceContext);
		pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
	}

	void shutdown()
	{
		    ReleaseCOM(m_pRenderTargetView  )
			ReleaseCOM(m_pMVPBuffer         )
			ReleaseCOM(m_pVertexBuffer      )
			ReleaseCOM(m_pIndexBuffer       )
	}

	void init_window(int ScreenWidth , int ScreenHeight, int BitmapWidth, int BitmapHeight);
	bool init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
	bool init_shader (ID3D11Device *pD3D11Device, HWND hWnd);
	void init_texture(ID3D11Device *pD3D11Device);

private:

	struct d3d::MatrixBuffer
	{
		XMMATRIX  model;
		XMMATRIX  view;
		XMMATRIX  proj;

	};
	d3d::MatrixBuffer cbMatrix;

	struct  Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
	};

	ID3D11RenderTargetView   *m_pRenderTargetView;
	ID3D11Buffer             *m_pMVPBuffer;
	ID3D11Buffer             *m_pVertexBuffer;
	ID3D11Buffer             *m_pIndexBuffer;
	ID3D11SamplerState       *m_pTexSamplerState;
	ID3D11InputLayout        *m_pInputLayout;
	ID3D11ShaderResourceView *m_pTextureSRV;

	int m_VertexCount;
	int m_IndexCount;

    int m_posX  ;
	int m_posY  ; 
	int m_width ; 
	int m_height;

	D3DShader D3DRTTShader;
};




#endif