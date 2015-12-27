#ifndef QUADTREE_H
#define QUADTREE_H


#include "d3d/d3dFrustum.h"



namespace byhj
{

const int MAX_TRIANGLES = 10000;

class QuadTree
{
private:
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 TexCoord;
	};

	struct Node
	{
		float posX, posZ, width;
		int   triangleCount;
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		Node* nodes[4];
	};

public:
	 QuadTree();
	 QuadTree(const QuadTree &qtree);
	~QuadTree();

	bool Init(TerrainClass*, ID3D11Device* pD3D11Device);
	void Shutdown();
	void Render(FrustumClass*, ID3D11DeviceContext* pD3D11DeviceContext, TerrainShaderClass*);

	int GetDrawCount();

private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(Node*, float, float, float, ID3D11Device*);
	int  CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ReleaseNode(Node*);
	void RenderNode(Node*, FrustumClass*, ID3D11DeviceContext*, TerrainShaderClass*);

private:

	int m_triangleCount, m_drawCount;
	Vertex* m_vertexList;
	Node* m_parentNode;
};


}
#endif