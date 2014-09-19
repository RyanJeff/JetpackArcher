#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "Vertex.h"
#include "GraphicalObject.h"
#include "Player.h"
#include "Projectile.h"
#include "Effect.h"
#include "FontRasterizer.h"
#include "Sprite.h"
#include "xnacollision.h"
#include "fmod.hpp"


struct BoundingBox
{
	XMFLOAT2 pos;
	float width;
	float height;
	BoundingBox() : pos(0.0f, 0.0f), width(0.0f), height(0.0f) {}
};

struct TestParticle
{
	XMFLOAT3 pos;
	XMFLOAT3 vel;
	XMFLOAT2 size;
};

const int MAX_PARTICLES = 100000;

class InClassProj : public D3DApp
{
public:
	InClassProj(HINSTANCE hInstance);
	~InClassProj();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void InitBoundingBoxes();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildVertexLayout();
	void BuildSceneLights();
	void BuildParticleVB();
	void BuildBlendStates();
	void BuildDSStates();

	void UpdateParticleVB();
	void UpdateKeyboardInput(float dt);
	void UpdateCollision();

	void DrawParticles();

private:
	LitTexEffect* mLitTexEffect;
	ParticleEffect* mParticleEffect;

	BaseCamera* m2DCam;

	FontRasterizer* mFont;

	XMFLOAT4 mAmbientColour;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 m2DProj;

	PointLightOptimized mPointLight;
	SpotLightOptimized mSpotLight;

	Sprite* mPlayer;

	Sprite* mBG;

	//bounding boxes for enviroment collision
	BoundingBox bb1;
	BoundingBox bb2;
	BoundingBox bb3;
	BoundingBox bb4;
	BoundingBox bb5;
	BoundingBox bb6;
	BoundingBox bb7;
	BoundingBox bb8;
	BoundingBox bb9;
	BoundingBox bb10;
	BoundingBox bb11;
	BoundingBox bb12;
	BoundingBox bb13;
	BoundingBox bb14;
	BoundingBox bb15;
	BoundingBox bb16;
	BoundingBox bb17;
	BoundingBox bb18;
	BoundingBox bb19;
	BoundingBox bb20;
	BoundingBox bb21;
	BoundingBox bb22;
	BoundingBox bb23;
	BoundingBox bb24;
	BoundingBox bb25;
	BoundingBox bb26;
	BoundingBox bb27;
	BoundingBox bb28;
	BoundingBox bb29;
	BoundingBox bb30;
	BoundingBox bb31;

	

	std::vector<BoundingBox> boxes;

	std::vector<Projectile*> mProjectiles;

	std::vector<TestParticle> mParticles;

	ID3D11Buffer* mParticleVB;
	ID3D11ShaderResourceView* mParticleTexture;

	ID3D11BlendState* mAdditiveBS;
	ID3D11BlendState* mTransparentBS;
	ID3D11DepthStencilState* mNoDepthDS;
	ID3D11DepthStencilState* mFontDS;

	bool mMouseReleased;
	POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	InClassProj theApp(hInstance);

	if( !theApp.Init() )
		return 0;

	return theApp.Run();
}

InClassProj::InClassProj(HINSTANCE hInstance) : 
	D3DApp(hInstance), mLitTexEffect(0), mMouseReleased(true), m2DCam(0), mPlayer(0), mBG(0)
{
	XMVECTOR pos = XMVectorSet(1.0f, 1.0f, 5.0f, 0.0f);
	XMVECTOR look = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	mMainWndCaption = L"Jetpack Archer";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
	XMStoreFloat4x4(&m2DProj, I);

	srand((UINT)time(NULL));
}

InClassProj::~InClassProj()
{
	Vertex::CleanLayouts();

	if(mLitTexEffect)
		delete mLitTexEffect;

	if (m2DCam)
		delete m2DCam;

	if (mPlayer)
		delete mPlayer;

	if (mBG)
		delete mBG;

	if (mParticleEffect)
		delete mParticleEffect;

	if (mParticleVB)
		ReleaseCOM(mParticleVB);

	if (mParticleTexture)
		ReleaseCOM(mParticleTexture);

	if (mAdditiveBS)
		ReleaseCOM(mAdditiveBS);

	if (mTransparentBS)
		ReleaseCOM(mTransparentBS);

	if (mNoDepthDS)
		ReleaseCOM(mNoDepthDS);
}

void InClassProj::BuildSceneLights()
{
	/* test code, test a point light out */
	mPointLight.pos = XMFLOAT3(50.0f, 50.0f, 50.0f);
	mPointLight.lightColour = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	mPointLight.range = 1000.0f;
	mPointLight.att = XMFLOAT3(0.0f, 0.02f, 0.0f);
	mPointLight.pad = 0.0f;

	mSpotLight.pos = XMFLOAT3(m2DCam->GetPos().m128_f32[0], m2DCam->GetPos().m128_f32[1], m2DCam->GetPos().m128_f32[2]);

	mSpotLight.lightColour = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	mSpotLight.range = 100.0f;
	mSpotLight.att = XMFLOAT3(0.0f, 0.25f, 0.0f);
	XMVECTOR temp = XMVectorSet(-mSpotLight.pos.x, -mSpotLight.pos.y, -mSpotLight.pos.z, 0.0f);
	temp = XMVector3Normalize(temp);
	XMStoreFloat3(&mSpotLight.direction, temp);
	mSpotLight.spot = 128.0f;

	mAmbientColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

FMOD_RESULT result;
FMOD::System     *sys;
FMOD::Sound      *sound1, *sound2, *sound3;
FMOD::Channel    *channel = 0;
unsigned int      version;
void             *extradriverdata = 0;


bool InClassProj::Init()
{
	if(!D3DApp::Init())
		return false;

	//FMOD sounds
	result = FMOD::System_Create(&sys);
	result = sys->getVersion(&version);
	if (version < FMOD_VERSION)
	{
		OutputDebugString(L"FMOD lib version doesn't match header version");
	}
	result = sys->init(32, FMOD_INIT_NORMAL, extradriverdata);
	result = sys->createSound("Sounds/thunder.ogg", FMOD_DEFAULT, 0, &sound1);  //fire arrow sfx
	result = sound1->setMode(FMOD_LOOP_OFF);

 	mLitTexEffect = new LitTexEffect();
	mLitTexEffect->LoadEffect(L"FX/lighting.fx", md3dDevice);

	mParticleEffect = new ParticleEffect();
	mParticleEffect->LoadEffect(L"FX/ParticleEffect.fx", md3dDevice);

	Vertex::InitLitTexLayout(md3dDevice, mLitTexEffect->GetTech());

	//camera
	m2DCam = new BaseCamera(XMVectorSet(0.0f, 0.0f, -0.5f, 0.0f), XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	m2DCam->Update();

	BuildSceneLights();

	Vertex::InitParticleVertLayout(md3dDevice, mParticleEffect->GetTech());

	BuildParticleVB();

	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/TestAdditive.png", 0, 0, &mParticleTexture, 0);

	BuildBlendStates();
	BuildDSStates();

	//font
	ID3D11ShaderResourceView* font;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/font.png", 0, 0, &font, 0);
	mFont = new FontRasterizer(m2DCam, XMLoadFloat4x4(&m2DProj), mLitTexEffect, 10, 10, font, md3dDevice);

	//bg image
	Sprite::Frame* BGFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* BGimage;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/TFAbgTiled.png", 0, 0, &BGimage, 0);
	//bg frame
	BGFrame->imageWidth = 1024;
	BGFrame->imageHeight = 768;
	BGFrame->x = 0;
	BGFrame->y = 0;
	BGFrame->image = BGimage;
	std::vector<Sprite::Frame*> bgFrame;
	bgFrame.push_back(BGFrame);

	//spritesheet
	Sprite::Frame* newFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* image;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/player.png", 0, 0, &image, 0);  //test
	//player frame1
	newFrame->imageWidth = 96;
	newFrame->imageHeight = 96;
	newFrame->x = 0;
	newFrame->y = 0;
	newFrame->image = image;
	std::vector<Sprite::Frame*> frames;
	frames.push_back(newFrame);
	//player frame2
	newFrame = new Sprite::Frame();
	newFrame->imageWidth = 96;
	newFrame->imageHeight = 96;
	newFrame->x = 32;
	newFrame->y = 0;
	newFrame->image = image;
	frames.push_back(newFrame);
	//player frame3
	newFrame = new Sprite::Frame();
	newFrame->imageWidth = 96;
	newFrame->imageHeight = 96;
	newFrame->x = 64;
	newFrame->y = 0;
	newFrame->image = image;
	frames.push_back(newFrame);

	mBG = new Sprite(XMVectorSet(mClientWidth / 2.0f, mClientHeight / 2.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), 
					 1024.0f, 768.0f, 1.0f, bgFrame, 0.25f, md3dDevice);

	mPlayer = new Sprite(XMVectorSet(mClientWidth / 2.0f, mClientHeight / 2.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
						 32, 32, 0.1f, frames, 0.25f, md3dDevice);

	mPlayer->Play(true);

	//result = sys->playSound(sound1, 0, false, &channel);

	return true;
}

void InClassProj::InitBoundingBoxes()
{
	bb1.pos = XMFLOAT2(0.0f, 0.0f);
	bb1.height = 32.0f;
	bb1.width = 128.0f;
	boxes.push_back(bb1);

	bb2.pos = XMFLOAT2(128.0f, 0.0f);
	bb2.height = 768.0f;
	bb2.width = 32.0f;
	boxes.push_back(bb2);

	bb3.pos = XMFLOAT2(896.0f, 0.0f);
	bb3.height = 128.0f;
	bb3.width = 64.0f;
	boxes.push_back(bb3);

	bb4.pos = XMFLOAT2(960.0f, 64.0f);
	bb4.height = 64.0f;
	bb4.width = 32.0f;
	boxes.push_back(bb4);

	bb5.pos = XMFLOAT2(0.0f, 128.0f);
	bb5.height = 32.0f;
	bb5.width = 128.0f;
	boxes.push_back(bb5);

	bb6.pos = XMFLOAT2(0.0f, 96.0f);
	bb6.height = 32.0f;
	bb6.width = 128.0f;
	boxes.push_back(bb6);

	bb7.pos = XMFLOAT2(992.0f, 96.0f);
	bb7.height = 32.0f;
	bb7.width = 128.0f;
	boxes.push_back(bb7);

	bb8.pos = XMFLOAT2(224.0f, 128.0f);
	bb8.height = 64.0f;
	bb8.width = 18.0f;
	boxes.push_back(bb8);

	bb9.pos = XMFLOAT2(736.0f, 128.0f);
	bb9.height = 64.0f;
	bb9.width = 18.0f;
	boxes.push_back(bb9);

	bb10.pos = XMFLOAT2(416.0f, 160.0f);
	bb10.height = 192.0f;
	bb10.width = 18.0f;
	boxes.push_back(bb10);

	bb11.pos = XMFLOAT2(448.0f, 177.0f);
	bb11.height = 128.0f;
	bb11.width = 47.0f;
	boxes.push_back(bb11);

	bb12.pos = XMFLOAT2(0.0f, 224.0f);
	bb12.height = 192.0f;
	bb12.width = 18.0f;
	boxes.push_back(bb12);

	bb13.pos = XMFLOAT2(0.0f, 241.0f);
	bb13.height = 128.0f;
	bb13.width = 47.0f;
	boxes.push_back(bb13);

	bb14.pos = XMFLOAT2(832.0f, 224.0f);
	bb14.height = 192.0f;
	bb14.width = 18.0f;
	boxes.push_back(bb14);

	bb15.pos = XMFLOAT2(896.0f, 140.0f);
	bb15.height = 128.0f;
	bb15.width = 47.0f;
	boxes.push_back(bb15);

	bb16.pos = XMFLOAT2(0.0f, 288.0f);
	bb16.height = 32.0f;
	bb16.width = 160.0f;
	boxes.push_back(bb16);

	bb17.pos = XMFLOAT2(256.0f, 320.0f);
	bb17.height = 86.0f;
	bb17.width = 96.0f;
	boxes.push_back(bb17);

	bb18.pos = XMFLOAT2(672.0f, 320.0f);
	bb18.height = 86.0f;
	bb18.width = 96.0f;
	boxes.push_back(bb18);

	bb19.pos = XMFLOAT2(992.0f, 288.0f);
	bb19.height = 32.0f;
	bb19.width = 116.0f;
	boxes.push_back(bb19);

	bb20.pos = XMFLOAT2(0.0f, 448.0f);
	bb20.height = 96.0f;
	bb20.width = 96.0f;
	boxes.push_back(bb20);

	bb21.pos = XMFLOAT2(480.0f, 448.0f);
	bb21.height = 64.0f;
	bb21.width = 64.0f;
	boxes.push_back(bb21);

	bb22.pos = XMFLOAT2(928.0f, 448.0f);
	bb22.height = 96.0f;
	bb22.width = 96.0f;
	boxes.push_back(bb22);

	bb23.pos = XMFLOAT2(0.0f, 544.0f);
	bb23.height = 128.0f;
	bb23.width = 64.0f;
	boxes.push_back(bb23);

	bb24.pos = XMFLOAT2(448.0f, 512.0f);
	bb24.height = 128.0f;
	bb24.width = 52.0f;
	boxes.push_back(bb24);

	bb25.pos = XMFLOAT2(896.0f, 544.0f);
	bb25.height = 128.0f;
	bb25.width = 64.0f;
	boxes.push_back(bb25);

	bb26.pos = XMFLOAT2(480.0f, 563.0f);
	bb26.height = 64.0f;
	bb26.width = 45.0f;
	boxes.push_back(bb26);

	bb27.pos = XMFLOAT2(0.0f, 608.0f);
	bb27.height = 320.0f;
	bb27.width = 18.0f;
	boxes.push_back(bb27);

	bb28.pos = XMFLOAT2(0.0f, 626.0f);
	bb28.height = 288.0f;
	bb28.width = 110.0f;
	boxes.push_back(bb28);

	bb29.pos = XMFLOAT2(704.0f, 608.0f);
	bb29.height = 320.0f;
	bb29.width = 18.0f;
	boxes.push_back(bb29);

	bb30.pos = XMFLOAT2(736.0f,626.0f);
	bb30.height = 288.0f;
	bb30.width = 110.0f;
	boxes.push_back(bb30);

	bb31.pos = XMFLOAT2(0.0f, 736.0f);
	bb31.height = 1024.0f;
	bb31.width = 32.0f;
	boxes.push_back(bb31);
}

void InClassProj::BuildBlendStates()
{
	D3D11_BLEND_DESC bsDesc = { 0 };
	bsDesc.AlphaToCoverageEnable = false;
	bsDesc.IndependentBlendEnable = false;

	bsDesc.RenderTarget[0].BlendEnable = true;
	bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bsDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(md3dDevice->CreateBlendState(&bsDesc, &mAdditiveBS));

	bsDesc.RenderTarget[0].BlendEnable = true;
	bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	HR(md3dDevice->CreateBlendState(&bsDesc, &mTransparentBS));
}

void InClassProj::BuildDSStates()
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// We are not rendering backfacing polygons, so these settings do not matter.
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(md3dDevice->CreateDepthStencilState(&dsDesc, &mNoDepthDS));

	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// We are not rendering backfacing polygons, so these settings do not matter.
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(md3dDevice->CreateDepthStencilState(&dsDesc, &mFontDS));
}

void InClassProj::BuildParticleVB()
{
	std::vector<Vertex::ParticleVertex> vertices(MAX_PARTICLES);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex::ParticleVertex) * MAX_PARTICLES;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mParticleVB));
}

void InClassProj::UpdateParticleVB()
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(md3dImmediateContext->Map(mParticleVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	Vertex::ParticleVertex* v = reinterpret_cast<Vertex::ParticleVertex*> (mappedData.pData);

	for (UINT i = 0; i < mParticles.size(); ++i)
	{
		v[i].pos = mParticles[i].pos;
		v[i].size = mParticles[i].size;
	}

	md3dImmediateContext->Unmap(mParticleVB, 0);
}

void InClassProj::OnResize()
{
	D3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);

	P = XMMatrixOrthographicOffCenterLH(0.0f, mClientWidth, 0.0f, mClientHeight, 0.0001f, 1000.0f);
	XMStoreFloat4x4(&m2DProj, P);
}

void InClassProj::UpdateCollision()
{

}

//float timer = 0.0f;
void InClassProj::UpdateScene(float dt)
{
	UpdateKeyboardInput(dt);
	
	m2DCam->Update();

	for (int i = 0; i < mProjectiles.size(); ++i)
	{
		mProjectiles[i]->Update(dt);

		if (mProjectiles[i]->GetDistanceTravelled() > mProjectiles[i]->MAX_DISTANCE)
		{
			delete mProjectiles[i];
			mProjectiles.erase(mProjectiles.begin() + i);
			i--;
		}
	}

	for (int i = 0; i < mParticles.size(); ++i)
	{
		XMVECTOR vel = XMLoadFloat3(&mParticles[i].vel);
		XMVECTOR pos = XMLoadFloat3(&mParticles[i].pos);
		pos = pos + vel;
		XMStoreFloat3(&mParticles[i].pos, pos);
	}

	UpdateParticleVB();
	UpdateCollision();

	//mBG->Update(dt);
	mPlayer->Update(dt);

	sys->update();
}

void InClassProj::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::White));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(Vertex::GetNormalTexVertLayout());
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMVECTOR ambient = XMLoadFloat4(&mAmbientColour);

	XMVECTOR eyePos = XMVectorSet(m2DCam->GetPos().m128_f32[0], m2DCam->GetPos().m128_f32[1], m2DCam->GetPos().m128_f32[2], 0.0f);

	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX view = m2DCam->GetView();

	mLitTexEffect->SetPerFrameParams(ambient, eyePos, mPointLight, mSpotLight);

	XMMATRIX vp = view * proj;

	for (int i = 0; i < mProjectiles.size(); ++i)
	{
		mProjectiles[i]->Draw(md3dImmediateContext, vp);
	}

	vp = XMMatrixIdentity();
	proj = XMLoadFloat4x4(&m2DProj);
	view = m2DCam->GetView();

	vp = vp * view * proj;

	DrawParticles();
	
	md3dImmediateContext->IASetInputLayout(Vertex::GetNormalTexVertLayout());
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	md3dImmediateContext->OMSetBlendState(mTransparentBS, blendFactor, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(mFontDS, 0);
	
	vp = XMMatrixIdentity();
	proj = XMLoadFloat4x4(&m2DProj);
	view = m2DCam->GetView();

	vp = vp * view * proj;

	mBG->Draw(vp, md3dImmediateContext, mLitTexEffect);

	mPlayer->Draw(vp, md3dImmediateContext, mLitTexEffect);

	//mFont->DrawFont(md3dImmediateContext, XMVectorSet(10.0f, 500.0f, 0.0f, 0.0f), 50, 75, 10, "Hi Brandon, you are a good student");

	md3dImmediateContext->OMSetDepthStencilState(0, 0);
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);

	HR(mSwapChain->Present(1, 0));
}

void InClassProj::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	if ((btnState & MK_RBUTTON) != 0)
	{
		//Projectile* newProjectile = new Projectile(mTestPlayer->GetLook() * 200.0f, mTestPlayer->GetPos() + mTestPlayer->GetUp() * 2.0f,
			//mTestPlayer->GetLook(), mTestPlayer->GetUp(), 1.0f, *mBarnProjectile);
		//mProjectiles.push_back(newProjectile);
	}

	if ((btnState & MK_LBUTTON) != 0)
	{
		//TestParticle newParticle;
		//XMStoreFloat3(&newParticle.pos, mPlayer->GetPos() + XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		//XMStoreFloat3(&newParticle.vel, mPlayer->GetLook() * 0.1f);
		//newParticle.size.x = 2.0f;
		//newParticle.size.y = 2.0f;
		//mParticles.push_back(newParticle);
	}

	SetCapture(mhMainWnd);	 
}

void InClassProj::OnMouseUp(WPARAM btnState, int x, int y)
{
	mMouseReleased = false;

	ReleaseCapture();
}

void InClassProj::OnMouseMove(WPARAM btnState, int x, int y)
{
	// Make each pixel correspond to a quarter of a degree.
	float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
	float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void InClassProj::UpdateKeyboardInput(float dt)
{
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		//temp code:
		float x = 0.0f;
		x = dt * 150;
		mPlayer->SetPos(XMVectorSet(mPlayer->GetPos().m128_f32[0] - x, mPlayer->GetPos().m128_f32[1], mPlayer->GetPos().m128_f32[2], 0.0f));
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		//temp code:
		float x = 0.0f;
		x = dt * 150;
		mPlayer->SetPos(XMVectorSet(mPlayer->GetPos().m128_f32[0] + x, mPlayer->GetPos().m128_f32[1], mPlayer->GetPos().m128_f32[2], 0.0f));
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		//jump
		
	}
	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
	{
		//jetpack
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		//shoot arrows

		bool isPlaying = false;
		channel->isPlaying(&isPlaying);
		if (!isPlaying)
		{
			result = sys->playSound(sound1, 0, false, &channel);
		}
	}
}

void InClassProj::DrawParticles()
{
	XMVECTOR eyePos = m2DCam->GetPos();
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX view = m2DCam->GetView();

	XMMATRIX vp = view * proj;

	mParticleEffect->SetPerFrameParams(eyePos);
	mParticleEffect->SetPerObjectParams(vp, mParticleTexture);

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	md3dImmediateContext->IASetInputLayout(Vertex::GetParticleVertLayout());
	
	UINT stride = sizeof(Vertex::ParticleVertex);
	UINT offset = 0;

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mParticleVB, &stride, &offset);
	
	md3dImmediateContext->OMSetDepthStencilState(mNoDepthDS, 0);
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	md3dImmediateContext->OMSetBlendState(mAdditiveBS, blendFactor, 0xffffffff);

	D3DX11_TECHNIQUE_DESC techDesc; 
	mParticleEffect->GetTech()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mParticleEffect->GetTech()->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->Draw(mParticles.size(), 0);
	}

	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(NULL, 0);
}