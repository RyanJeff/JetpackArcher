#include "BasicModel.h"
#include "OBJLoader.h"

BasicModel::BasicModel(ID3D11Device* device, LitTexEffect* effect, std::string filename,
					   bool isRHS, bool isVFlipped)

{
	mModelMesh = new MeshGeometry(effect);

	std::vector<ID3D11ShaderResourceView*> srvs;
	if(OBJLoader::LoadOBJ(device, filename, mVertices, mIndices, mSubsets,
						isRHS, isVFlipped))
	{
		mModelMesh->SetVertices(device, &mVertices[0], mVertices.size());
		mModelMesh->SetIndices(device, &mIndices[0], mIndices.size());
		mModelMesh->SetSubsetTable(mSubsets);
	}
}


BasicModel::~BasicModel(void)
{
	if(mModelMesh)
	{
		delete mModelMesh;
	}
}
