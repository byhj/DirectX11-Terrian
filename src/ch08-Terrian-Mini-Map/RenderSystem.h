#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "d3d/App.h"
#include "d3d/Utility.h"
#include "d3d/Font.h"
#include "d3d/Timer.h"
#include "d3d/Camera.h"

#include "Terrain.h"
#include "Bitmap.h"

namespace byhj
{

	class RenderSystem : public d3d::App
{
public:
	RenderSystem();
	~RenderSystem();

	void v_Init();
	void v_Update();
	void v_Render();
	void v_Shutdown();

private:
	void init_device();
	void init_camera();
	void init_object();

	void BeginScene();
	void EndScene();
    void EnableZbuffer();
	void DisableZbuffer();
	void DrawFps();
	void DrawInfo(); 

	byhj::Terrain m_Terrain;
	byhj::Bitmap  m_Back;
	byhj::Bitmap  m_Border;
	byhj::Bitmap  m_Location;

	d3d::Font m_Font;
	d3d::Timer m_Timer;
	d3d::Camera m_Camera;

	float fps = 0.0f;
	int m_videoCardMemory;
	std::wstring m_videoCardInfo;

	IDXGISwapChain           *m_pSwapChain                  = nullptr;
	ID3D11Device             *m_pD3D11Device                = nullptr;
	ID3D11DeviceContext      *m_pD3D11DeviceContext         = nullptr;
	ID3D11RenderTargetView   *m_pRenderTargetView           = nullptr;
	ID3D11DepthStencilView   *m_pDepthStencilView           = nullptr;
	ID3D11Texture2D          *m_pDepthStencilBuffer         = nullptr;
	ID3D11DepthStencilState  *m_pDepthStencilState          = nullptr;
	ID3D11DepthStencilState  *m_pDepthDisabledStencilState  = nullptr;
	ID3D11RasterizerState    *m_pRasterState                = nullptr;

	d3d::MatrixBuffer m_Matrix;
};
}
#endif