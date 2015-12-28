#ifndef SkyPlane_H
#define SkyPlane_H

#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include "d3d/Shader.h"
#include "d3d/Utility.h"
#include "d3d/Geometry.h"

using namespace DirectX;

namespace byhj
{

class SkyPlane
{
public:
	SkyPlane() = default;
	SkyPlane(const SkyPlane &);
	~SkyPlane() = default;
     
	struct SkyPlaneType
	{
		float x, y, z;
		float u, v;
	};
	struct Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
	};

	void Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);
	void Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix);
	void Shutdown();


	int GetIndexCount() const;

	ID3D11ShaderResourceView * GetCloundTexture1();
	ID3D11ShaderResourceView * GetCloundTexture2();

	float GetBrightness();
	float GetTranslation(int);

private:

	void init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
	void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);
	void init_texture(ID3D11Device *pD3D11Device);

	SkyPlaneType *m_pSkyPlaneType;
	ID3D11Buffer *m_pSkyPlaneVB;
	ID3D11Buffer *m_pSkyPlaneIB;

	ID3D11Buffer        *m_pMVPBuffer   = nullptr;
	ID3D11Buffer        *m_pSkyPlaneBuffer   = nullptr;
	ID3D11InputLayout   *m_pInputLayout = nullptr;

	ID3D11ShaderResourceView *m_pTextureSRV;
	ID3D11SamplerState       *m_pTexSamplerState;

	d3d::MatrixBuffer cbMatrix;
	d3d::Shader SkyPlaneShader;

	int m_VertexCount, m_IndexCount;
	float m_Brightness;
	float m_TranslationSpeed[4];
	float m_TextureTranslation[4];
};

}

#endif