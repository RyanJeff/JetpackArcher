#pragma once

#include "d3dUtil.h"

class BaseCamera
{
protected:
	XMFLOAT3 mPos;
	XMFLOAT3 mLook;
	XMFLOAT3 mUp;
	XMFLOAT3 mRight;

	XMFLOAT4X4 mView;

	bool mViewUpdated;

public:
	BaseCamera(void);
	BaseCamera(FXMVECTOR pos, FXMVECTOR look, FXMVECTOR up);
	virtual ~BaseCamera(void);

	virtual void Update();

	FXMVECTOR GetPos() const;

	FXMVECTOR GetLook() const;

	FXMVECTOR GetUp() const;

	XMMATRIX GetView() const;

	void SetPos(FXMVECTOR pos);
	void SetFacing(FXMVECTOR look, FXMVECTOR up);
};

