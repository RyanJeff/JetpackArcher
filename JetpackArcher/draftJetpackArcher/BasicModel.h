#pragma once

#include "d3dUtil.h"
#include "MeshGeometry.h"
#include "Vertex.h"

class BasicModel
{
public:
	BasicModel() : 
	  mSubsetCount(0),
	  mModelMesh(0)
	  {}
	BasicModel(ID3D11Device* device, LitTexEffect* effect, std::string filename, 
			  bool isRHS, bool isVFlipped);
	virtual ~BasicModel(void);

	MeshGeometry* GetModelMesh() const
	{
		return mModelMesh;
	}


protected:
	UINT mSubsetCount;
	
	std::vector<Vertex::NormalTexVertex> mVertices;

	std::vector<UINT> mIndices;

	std::vector<MeshGeometry::Subset> mSubsets;

	MeshGeometry* mModelMesh;
};

