#pragma once
#include "GraphicalGeometry.h"
#include "Vertex.h"
#include "Effect.h"

class MeshGeometry : public GraphicalGeometry
{
public:
	struct Subset
	{
		UINT id;
		UINT vertexStart;
		UINT vertexCount;
		UINT faceCount;
		UINT faceStartIndex;
		ID3D11ShaderResourceView* srv;
		std::wstring name;
	};

public:
	MeshGeometry(LitTexEffect* effect);
	~MeshGeometry(void);

	void SetVertices(ID3D11Device* device, const Vertex::NormalTexVertex* verts, UINT count);

	void SetIndices(ID3D11Device* device, const UINT* indices, UINT count);

	void SetSubsetTable(std::vector<Subset>& subsetTable);

	void Draw(ID3D11DeviceContext* dc, UINT subsetID);

	void Draw(ID3D11DeviceContext* context, CXMMATRIX world, CXMMATRIX ITWorld, CXMMATRIX vp);

protected:
	std::vector<Subset> mSubsetTable;

	LitTexEffect* mEffect;

	virtual void InitIB(ID3D11Device* device)
	{}
};