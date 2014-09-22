#pragma once
#include "GraphicalGeometry.h"
#include "Effect.h"

class Sprite : public GraphicalGeometry
{
public:
	struct Frame
	{
		ID3D11ShaderResourceView* image;
		uint16_t x;   //frame x location in pixels
		uint16_t y;   //frame y location in pixels

		uint16_t imageWidth;
		uint16_t imageHeight;
	};

public:
	Sprite(FXMVECTOR pos2D, FXMVECTOR scale2D, uint16_t frameWidth, uint16_t frameHeight, float depth, 
		const std::vector<Frame*>& frames, float frameRate, ID3D11Device* device);

	Sprite::Sprite();

	virtual ~Sprite();

	XMVECTOR GetPos() const
	{
		return XMLoadFloat2(&mPos);
	}

	void SetPos(FXMVECTOR pos)
	{
		XMStoreFloat2(&mPos, pos);
	}

	XMVECTOR GetScale() const
	{
		return XMLoadFloat2(&mScale);
	}

	void SetScale(FXMVECTOR scale)
	{
		XMStoreFloat2(&mScale, scale);
	}

	float GetDepth() const
	{
		return mDepth;
	}

	void SetDepth(float depth)
	{
		mDepth = depth;
	}

	float GetFrameRate() const
	{
		return mFrameRate;
	}

	void SetFrameRate(float frameRate)
	{
		mFrameRate = frameRate;
	}

	void Play(bool loop)
	{
		mLoop = loop;
		mPlaying = true;
	}

	void Pause()
	{
		mPlaying = false;
	}

	void Stop()
	{
		mPlaying = false;
		mFrameIndex = 0;
	}

	void SetFrame(uint16_t frame)
	{
		mFrameIndex = frame;
	}
	
	float GetAngle() const
	{
		return mAngle;
	}

	//note: angle is in radians
	void SetAngle(float angle)
	{
		mAngle = angle;
	}

	//note: angle is in radians
	void Rotate(float angle)
	{
		mAngle += angle;
	}

	float GetSpeed() const
	{
		return mSpeed;
	}

	virtual void HitGround();
	virtual void LeaveGround()
	{
		mGrounded = false;
	}

	virtual void AddForce(FXMVECTOR force);

	void Draw(CXMMATRIX vp, ID3D11DeviceContext* context, LitTexEffect* litTexEffect);

	virtual void Update(float dt);

protected:
	XMFLOAT2 mPos;
	XMFLOAT2 mScale;
	uint16_t mFrameWidth;
	uint16_t mFrameHeight;
	float mDepth;
	std::vector<Frame*> mFrames;
	float mFrameRate;
	bool mLoop;
	bool mPlaying;
	uint16_t mFrameIndex;
	float mAngle;   //in radians
	float mCurrFrameTime;

	virtual void InitVB(ID3D11Device* device);
	virtual void InitIB(ID3D11Device* device);

	XMMATRIX GetWorld();

	float mHealth;
	float mSpeed;
	XMFLOAT3 mVelocity;
	bool mGrounded;
};