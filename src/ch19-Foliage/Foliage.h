#ifndef Foliage_H
#define Foliage_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include  "d3d/Utility.h"
#include  "d3d/Shader.h"
using namespace DirectX;

namespace byhj
{
	class Foliage
	{
	 public:
		 Foliage() = default;
		 Foliage(const Foliage &) = default;
		 ~Foliage() = default;

		void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
		void Update(ID3D11DeviceContext *pD3D11DeviceContext);
		void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &mvpMatrix);
		void Shutdown();

	private:
		void gen_pos();
		void init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
		void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);
		void init_texture(ID3D11Device *pD3D11Device);

		struct Vertex
		{
			XMFLOAT3 Pos;
			XMFLOAT2 Tex;
		};

		struct InstanceType
		{
			XMFLOAT4X4 World;
			XMFLOAT3 Color;
		};

		struct FoliageType
		{
			float x, z;
			float r, g, b;
		};

		ID3D11Buffer *m_pFoliageVB = nullptr;
		ID3D11Buffer *m_pFoliageIB = nullptr;
		ID3D11Buffer *m_pMVPBuffer = nullptr;
		ID3D11Buffer *m_pInstanceBuffer = nullptr;

		ID3D11InputLayout   *m_pInputLayout = nullptr;
		ID3D11ShaderResourceView *m_pTextureSRV = nullptr;
		ID3D11SamplerState       *m_pTexSamplerState = nullptr;

		d3d::Shader FoliageShader;
		d3d::MatrixBuffer cbMatrix;

		int m_VertexCount = 0;
		int m_IndexCount = 0;
		float m_WindRotation = 0;
		int m_WindDirection = 1;
		int m_FoliageCount = 100;

		std::vector<FoliageType> m_FoliageType;
		std::vector<InstanceType> m_InstanceType;


	};
}
#endif