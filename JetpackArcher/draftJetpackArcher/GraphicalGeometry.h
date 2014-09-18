#pragma once

#include "d3dUtil.h"

class GraphicalGeometry
{
protected:
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	UINT mIndexCount;

public:
	GraphicalGeometry(void)  : mVB(0), mIB(0), mIndexCount(0)
	{
	}

	virtual ~GraphicalGeometry(void)
	{
		if(mVB)
		{
			mVB->Release();
		}

		if(mIB)
		{
			mIB->Release();
		}
	}

	ID3D11Buffer* GetVB()
	{
		return mVB;
	}

	ID3D11Buffer* GetIB()
	{
		return mIB;
	}

	UINT GetIndexCount() const
	{
		return mIndexCount;
	}

protected:

	virtual void InitIB(ID3D11Device* device) = 0;
};

