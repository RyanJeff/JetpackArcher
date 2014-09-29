#include "Sprite.h"
#include "Vertex.h"

Sprite::Sprite(FXMVECTOR pos2D, FXMVECTOR scale2D, uint16_t frameWidth, uint16_t frameHeight, float depth, const std::vector<Frame*>& frames, 
	float frameRate, ID3D11Device* device) :
		mFrameWidth(frameWidth),
		mFrameHeight(frameHeight),
		mDepth(depth),
		mFrames(frames),
		mFrameRate(frameRate),
		mFrameIndex(0),
		mAngle(0),
		mCurrFrameTime(0.0f),
		mVelocity(0.0f, 0.0f, 0.0f),
		mGrounded(false),
		mWidth(20.0f),
		mHeight(32.0f),
		mHealth(5.0f),
		mDamage(1.0f)
{
	XMStoreFloat2(&mPos, pos2D);
	XMStoreFloat2(&mScale, scale2D);

	InitVB(device);
	InitIB(device);
}

Sprite::~Sprite()
{
}

void Sprite::HitGround()
{
	mGrounded = true;
	mVelocity.y = 0.0f;
}

void Sprite::InitVB(ID3D11Device* device)
{
	XMFLOAT3 vbPositions[4];
	//top left
	vbPositions[0] = XMFLOAT3(-mFrameWidth / 2.0f, mFrameHeight / 2.0f, 0.0f);
	//top right
	vbPositions[1] = XMFLOAT3(mFrameWidth / 2.0f, mFrameHeight / 2.0f, 0.0f);
	//bottom right
	vbPositions[2] = XMFLOAT3(mFrameWidth / 2.0f, -mFrameHeight / 2.0f, 0.0f);
	//bottom left
	vbPositions[3] = XMFLOAT3(-mFrameWidth / 2.0f, -mFrameHeight / 2.0f, 0.0f);

	XMFLOAT3 normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

	std::vector<Vertex::NormalTexVertex> vertices(mFrames.size() * 4);

	for (int i = 0; i < mFrames.size(); ++i)
	{
		int vertexIndex = i * 4;
		vertices[vertexIndex].pos = vbPositions[0];
		vertices[vertexIndex + 1].pos = vbPositions[1];
		vertices[vertexIndex + 2].pos = vbPositions[2];
		vertices[vertexIndex + 3].pos = vbPositions[3];

		vertices[vertexIndex].normal = normal;
		vertices[vertexIndex + 1].normal = normal;
		vertices[vertexIndex + 2].normal = normal;
		vertices[vertexIndex + 3].normal = normal;

		Frame* currFrame = mFrames[i];
		float leftU = (float)currFrame->x / (float)currFrame->imageWidth;
		float topV = (float)currFrame->y / (float)currFrame->imageHeight;
		float rightU = leftU + ((float)mFrameWidth / (float)currFrame->imageWidth);
		float bottomV =  topV + ((float)mFrameHeight / (float)currFrame->imageHeight);

		vertices[vertexIndex].tex = XMFLOAT2(leftU, topV);
		vertices[vertexIndex + 1].tex = XMFLOAT2(rightU, topV);
		vertices[vertexIndex + 2].tex = XMFLOAT2(rightU, bottomV);
		vertices[vertexIndex + 3].tex = XMFLOAT2(leftU, bottomV);
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::NormalTexVertex) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbd, &vinitData, &mVB));
}

void Sprite::InitIB(ID3D11Device* device)
{
	std::vector<UINT> indices(mFrames.size() * 6);

	for (int i = 0; i < mFrames.size(); ++i)
	{
		UINT vertexIndex = i * 4;
		UINT indexIndex = i * 6;
		indices[indexIndex] = vertexIndex;
		indices[indexIndex + 1] = vertexIndex + 1;
		indices[indexIndex + 2] = vertexIndex + 2;
		indices[indexIndex + 3] = vertexIndex;
		indices[indexIndex + 4] = vertexIndex + 2;
		indices[indexIndex + 5] = vertexIndex + 3;
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(device->CreateBuffer(&ibd, &iinitData, &mIB));
}

XMMATRIX Sprite::GetWorld()
{
	XMMATRIX scale = XMMatrixScaling(mScale.x, mScale.y, 1.0f);
	XMMATRIX rot = XMMatrixRotationZ(mAngle);
	XMMATRIX trans = XMMatrixTranslationFromVector(XMVectorSet(mPos.x, mPos.y, mDepth, 1.0f));

	XMMATRIX world = scale * rot * trans;

	return world;
}

void Sprite::Update(float dt)
{
	if (mPlaying)
	{
		mCurrFrameTime += dt;
		if (mCurrFrameTime > mFrameRate)
		{
			mFrameIndex++;
			mCurrFrameTime -= mFrameRate;

			if (mFrameIndex >= mFrames.size())
			{
				if (mLoop)
				{
					mFrameIndex = 0;
				}
				else
				{
					mFrameIndex--;
					mPlaying = false;
				}
			}
		}
	}

	XMVECTOR pos = XMLoadFloat2(&mPos);
	XMVECTOR vel = XMLoadFloat3(&mVelocity);
	//if (mGrounded)
	//{
	//	vel = vel * 0.8f;
	//}
	//else
	//{
	//	vel = vel * 0.97f;
	//}
	pos = pos + (vel * dt);
	XMStoreFloat2(&mPos, pos);
	XMStoreFloat3(&mVelocity, vel);
}

void Sprite::Draw(CXMMATRIX vp, ID3D11DeviceContext* context, LitTexEffect* litTexEffect)
{
	UINT offset = 0;
	UINT stride = sizeof(Vertex::NormalTexVertex);

	XMMATRIX world = GetWorld();
	XMMATRIX wvp = vp;
	wvp = world * wvp;
	XMMATRIX invWorld = MathHelper::InverseTranspose(world);
	context->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	context->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	litTexEffect->SetPerObjectParams(world, invWorld, wvp, mFrames[mFrameIndex]->image);
	litTexEffect->Draw(context, mVB, mIB, mFrameIndex * 6, 6);
}

void Sprite::AddForce(FXMVECTOR force)
{
	XMVECTOR vel = XMLoadFloat3(&mVelocity);
	vel = vel + force;
	XMStoreFloat3(&mVelocity, vel);
}

void Sprite::ApplyDamage(Sprite* spriteHit)
{
	spriteHit->SetHealth(spriteHit->GetHealth() - mDamage);
}
