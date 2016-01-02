#ifndef QUADTREE_H
#define QUADTREE_H


#include "d3d/d3dFrustum.h"
#include "grid.h"

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

namespace byhj
{

const int MAX_TRIANGLES = 10000;

class QuadTree
{
private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texture;
	};

	struct Node
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		Node* nodes[4];
	};

public:
	 QuadTree() = default;
	 QuadTree(const QuadTree &qtree) = default;
	~QuadTree() = default;

	bool Init(Grid*, ID3D11Device* pD3D11Device);
	void Shutdown();
	void Render(ID3D11DeviceContext* pD3D11DeviceContext);

	int GetDrawCount();

private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(Node*, float, float, float, ID3D11Device*);
	int  CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ReleaseNode(Node*);
	void RenderNode(Node*, ID3D11DeviceContext*);

private:

	int m_TriangleCount, m_DrawCount;
	Vertex* m_pVertexList = nullptr;
	Node* m_pParentNode = nullptr;
};


}
#endif