#ifndef Water_H
#define Water_H

#include <d3d11.h>

#include <vector>
#include "d3d/Shader.h"
#include "d3d/Utility.h"

namespace byhj
{


	class Water
	{
	public:
		Water() {}
		~Water() {}


		void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
		void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix);
		void Shutdown();


	private:
		void init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
		void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);
		void init_texture(ID3D11Device *pD3D11Device);

		void loadHeightMap(const char *filename);
		void calcNormal(d3d::Geometry::MeshData &mesh);
		void CalcBump(d3d::Geometry::MeshData &mesh);

		d3d::MatrixBuffer cbMatrix;

		struct Vertex {
			XMFLOAT3 Pos;
			XMFLOAT2 Tex;
		};

		d3d::Shader WaterShader;

		struct LightBuffer
		{
			XMFLOAT4 ambient;
			XMFLOAT4 diffuse;
			XMFLOAT3 lightDir;
			float    pad;
		};
		LightBuffer cbLight;
		ID3D11Buffer        *m_pWaterVB      = nullptr;
		ID3D11Buffer        *m_pWaterIB      = nullptr;
		ID3D11Buffer        *m_pMVPBuffer   = nullptr;
		ID3D11Buffer        *m_pLightBuffer   = nullptr;
		ID3D11InputLayout   *m_pInputLayout = nullptr;

		ID3D11ShaderResourceView *m_pTextureSRV;
		ID3D11SamplerState       *m_pTexSamplerState;

		int m_VertexCount  = 0;
		int m_IndexCount   = 0;
	};

}
#endif