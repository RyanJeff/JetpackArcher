#pragma once

#include "d3dUtil.h"
#include "BaseCamera.h"

class FPSCamera : public BaseCamera
{
public:
	FPSCamera(void) : BaseCamera()
	{}

	FPSCamera(FXMVECTOR pos, FXMVECTOR look, FXMVECTOR up) :
	BaseCamera(pos, look, up) 
	{}

	virtual ~FPSCamera(void);

	void Pitch(float angle);
	void YawLocal(float angle);
	void YawGlobal(float angle);
	void Roll(float angle);

	void MoveLook(float amt);
	void MoveStrafe(float amt);
};

