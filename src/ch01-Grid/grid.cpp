#include "grid.h"
#include "d3d/Geometry.h"

namespace byhj
{

void Grid::Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix)
{
	//Update the the mvp matrix
	cbMatrix.model = matrix.model;
	cbMatrix.view  = matrix.view;
	cbMatrix.proj  = matrix.proj;
	pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0);
	pD3D11DeviceContext->VSSetConstantBuffers(0, 1, &m_pMVPBuffer);

	// Set vertex buffer stride and offset
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(Vertex);
	offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pGridVB, &stride, &offset);
	pD3D11DeviceContext->IASetIndexBuffer(m_pGridIB, DXGI_FORMAT_R32_UINT, 0);

	GridShader.use(pD3D11DeviceContext);
	pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);

}

void Grid::Shutdown()
{
	ReleaseCOM(m_pMVPBuffer)
	ReleaseCOM(m_pGridVB)
	ReleaseCOM(m_pGridIB)
	ReleaseCOM(m_pInputLayout)
}

void Grid::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	HRESULT hr;

	/////////////////////////////Vertex Buffer//////////////////////////////

	d3d::Geometry geom;
	d3d::Geometry::MeshData gridMesh;
	geom.CreateGrid(160.0, 160.0, 50, 50, gridMesh);

	m_VertexCount = gridMesh.VertexData.size();

	D3D11_BUFFER_DESC gridVBDesc;
	gridVBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	gridVBDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	gridVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	gridVBDesc.CPUAccessFlags      = 0;
	gridVBDesc.MiscFlags           = 0;
	gridVBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA gridVBO;
	gridVBO.pSysMem = &gridMesh.VertexData[0];
	hr = pD3D11Device->CreateBuffer(&gridVBDesc, &gridVBO, &m_pGridVB);
	DebugHR(hr);

	/////////////////////////////Index Buffer//////////////////////////////

	m_IndexCount =  gridMesh.IndexData.size();
	D3D11_BUFFER_DESC gridIBDesc;
	gridIBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	gridIBDesc.ByteWidth           = sizeof(UINT) * m_IndexCount;
	gridIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	gridIBDesc.CPUAccessFlags      = 0;
	gridIBDesc.MiscFlags           = 0;
	gridIBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA girdIBO;
	girdIBO.pSysMem = &gridMesh.IndexData[0];
	hr = pD3D11Device->CreateBuffer(&gridIBDesc, &girdIBO, &m_pGridIB);
	DebugHR(hr);

	////////////////////////////////Const Buffer//////////////////////////////////////

	D3D11_BUFFER_DESC mvpDesc;	
	ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));
	mvpDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpDesc.ByteWidth      = sizeof(d3d::MatrixBuffer);
	mvpDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpDesc.CPUAccessFlags = 0;
	mvpDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&mvpDesc, NULL, &m_pMVPBuffer);
	DebugHR(hr);
}


void Grid::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	//Shader interface information
	std::vector<D3D11_INPUT_ELEMENT_DESC> vInputLayoutDesc;
	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc;
	pInputLayoutDesc.SemanticName         = "POSITION";
	pInputLayoutDesc.SemanticIndex        = 0;
	pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc.InputSlot            = 0;
	pInputLayoutDesc.AlignedByteOffset    = 0;
	pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(pInputLayoutDesc);

	pInputLayoutDesc.SemanticName         = "NORMAL";
	pInputLayoutDesc.SemanticIndex        = 0;
	pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc.InputSlot            = 0;
	pInputLayoutDesc.AlignedByteOffset    = 12;
	pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(pInputLayoutDesc);

	pInputLayoutDesc.SemanticName         = "TEXCOORD";
	pInputLayoutDesc.SemanticIndex        = 0;
	pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32_FLOAT;
	pInputLayoutDesc.InputSlot            = 0;
	pInputLayoutDesc.AlignedByteOffset    = 24;
	pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc.InstanceDataStepRate = 0;
	vInputLayoutDesc.push_back(pInputLayoutDesc);
	

	GridShader.init(pD3D11Device, hWnd);
	GridShader.attachVS(L"grid.vsh", vInputLayoutDesc);
	GridShader.attachPS(L"grid.psh");
	GridShader.end();
}


}
