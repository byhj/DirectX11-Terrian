#ifndef Terrain_H
#define Terrain_H

#include <d3d11.h>
#include <vector>


#include "d3d/Shader.h"
#include "d3d/Utility.h"
#include "d3d/Geometry.h"
#include "d3d/TextureMgr.h"

namespace byhj
{


	class Terrain
	{
	public:
		Terrain()  = default;
		~Terrain() = default;

		void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
		void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix);
		void Shutdown();

	private:

		void init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
		void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);
		void init_texture(ID3D11Device *pD3D11Device);

		void load_heightMap(const char *filename);
		void calc_normal(d3d::Geometry::MeshData &mesh);
	    void calc_bump(d3d::Geometry::MeshData &mesh);

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

		struct LightBuffer
		{
			XMFLOAT4 ambient;
			XMFLOAT4 diffuse;
			XMFLOAT3 lightDir;
			float    pad;
		};
		LightBuffer cbLight;

		d3d::MatrixBuffer cbMatrix;
		d3d::Shader TerrainShader;
		std::vector<XMFLOAT3> m_HightmapData;
		std::vector<Vertex> m_VertexData;

		ID3D11Buffer        *m_pLightBuffer          = nullptr;
		ID3D11Buffer        *m_pTerrainVB            = nullptr;
		ID3D11Buffer        *m_pTerrainIB            = nullptr;
		ID3D11Buffer        *m_pMVPBuffer            = nullptr;
		ID3D11InputLayout   *m_pInputLayout          = nullptr;

		ID3D11ShaderResourceView *m_pTextureSRV      = nullptr;
		ID3D11ShaderResourceView *m_pNormalTexSRV    = nullptr;
		ID3D11SamplerState       *m_pTexSamplerState = nullptr;

		int m_VertexCount  = 0;
		int m_IndexCount   = 0;
		int m_TerrainWidth = 0;
		int m_TerrainHeight= 0;
	};

}
#endif


