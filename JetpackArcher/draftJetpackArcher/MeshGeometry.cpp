#include "MeshGeometry.h"


MeshGeometry::MeshGeometry(LitTexEffect* effect) : 
	mEffect(effect)
{
}


MeshGeometry::~MeshGeometry(void)
{
	for(int i = 0; i < mSubsetTable.size(); ++i)
	{
		ReleaseCOM(mSubsetTable[i].srv);
	}
}

void MeshGeometry::SetVertices(ID3D11Device* device,
							   const Vertex::NormalTexVertex* verts,
							   UINT count)
{
	ReleaseCOM(mVB);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::NormalTexVertex) * count;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = verts;

	HR(device->CreateBuffer(&vbd, &vinitData, &mVB));
}

void MeshGeometry::SetIndices(ID3D11Device* device, const UINT* indices,
							  UINT count)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * count;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = indices;

	mIndexCount = count;

	HR(device->CreateBuffer(&ibd, &iInitData, &mIB));
}

void MeshGeometry::SetSubsetTable(std::vector<Subset>& subsetTable)
{
	mSubsetTable = subsetTable;
}

void MeshGeometry::Draw(ID3D11DeviceContext* context, UINT subsetID)
{
	UINT offset = 0;
	UINT stride = sizeof(Vertex::NormalTexVertex);

	context->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	context->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(mSubsetTable[subsetID].faceCount * 3,
						 mSubsetTable[subsetID].faceStartIndex * 3,
						 0);
}

void MeshGeometry::Draw(ID3D11DeviceContext* context, CXMMATRIX world,
						CXMMATRIX ITWorld, CXMMATRIX vp)
{
	mEffect->SetPerObjectParams(world, ITWorld, world * vp, mSubsetTable[0].srv);
	for(int i = 0; i <  mSubsetTable.size(); ++i)
	{
		mEffect->SetDiffuseMap(mSubsetTable[i].srv);
		mEffect->Draw(context, mVB, mIB, mSubsetTable[i].faceStartIndex * 3,
			mSubsetTable[i].faceCount * 3);
	}
}

