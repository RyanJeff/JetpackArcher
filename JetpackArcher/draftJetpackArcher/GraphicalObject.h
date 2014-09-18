#pragma once
#include "d3dUtil.h"
#include "BasicModel.h"


class GraphicalObject : public BasicModel
{

protected:
	XMFLOAT3 mPos;
	XMFLOAT3 mLook, mRight, mUp;
	XMFLOAT3 mScale;

	XMFLOAT4X4 mWorld;

	bool mWorldUpdated;

public:
	GraphicalObject();
	/*GraphicalObject(FXMVECTOR pos, FXMVECTOR look, FXMVECTOR up,
					ID3D11Device* device, LitTexEffect* effect, std::string filename,
					bool isRHS = false, bool isVFlipped = false);*/

	GraphicalObject(FXMVECTOR pos, FXMVECTOR look, FXMVECTOR up, BasicModel& model);

	virtual ~GraphicalObject(void);

	void Update();

	void Draw(ID3D11DeviceContext* context, CXMMATRIX vp);

	FXMVECTOR GetPos() const;

	FXMVECTOR GetLook() const;

	FXMVECTOR GetUp() const;

	FXMVECTOR GetRight() const;


	void SetPos(FXMVECTOR pos);
	void SetFacing(FXMVECTOR look, FXMVECTOR up);

	void Pitch(float angle);
	void YawLocal(float angle);
	void YawGlobal(float angle);
	void Roll(float angle);

	virtual void MoveLook(float amt);
	virtual void MoveStrafe(float amt);
};

