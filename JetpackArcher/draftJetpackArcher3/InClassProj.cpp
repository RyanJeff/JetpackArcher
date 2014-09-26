/*##############################################################################################
										 - Jetpack Archer -
Controls - LEFT ARROW to move left
		 - RIGHT ARROW to move right
		 - UP ARROW to jump
		 - SPACE to shoot arrows
		 - LEFT SHIFT to use jetpack
###############################################################################################*/

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "BoundingBoxes.h"
#include "Vertex.h"
#include "GraphicalObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Effect.h"
#include "FontRasterizer.h"
#include "Sprite.h"
#include "xnacollision.h"
#include "fmod.hpp"

struct JetpackParticle
{
	XMFLOAT3 pos;
	XMFLOAT3 vel;
	XMFLOAT2 size;
};

const int MAX_PARTICLES = 100000;

class InClassProj : public D3DApp
{
	enum CollisionSide
	{
		top,
		right,
		bot,
		left
	};

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

	CollisionSide RectRectCollision(BoundingBoxes::BoundingBox r1, BoundingBoxes::BoundingBox r2, Sprite* sprite);
	void InClassProj::SpriteRectCollision(Sprite* sprite, BoundingBoxes::BoundingBox bb);
	bool EnemyProjCollision(Sprite* sprite, Projectile* arrow);

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

	Sprite* mBG;

	Player* mPlayer;
	Enemy* mEnemy1;
	Enemy* mEnemy2;
	Enemy* mEnemy3;
	std::vector<Enemy*> enemies;

	BoundingBoxes::BoundingBox playerBB;

	BoundingBoxes::BoundingBox bb1;
	BoundingBoxes::BoundingBox bb2;
	BoundingBoxes::BoundingBox bb3;
	BoundingBoxes::BoundingBox bb4;
	BoundingBoxes::BoundingBox bb5;
	BoundingBoxes::BoundingBox bb6;
	BoundingBoxes::BoundingBox bb7;
	BoundingBoxes::BoundingBox bb8;
	BoundingBoxes::BoundingBox bb9;
	BoundingBoxes::BoundingBox bb10;
	BoundingBoxes::BoundingBox bb11;
	BoundingBoxes::BoundingBox bb12;
	BoundingBoxes::BoundingBox bb13;
	BoundingBoxes::BoundingBox bb14;
	BoundingBoxes::BoundingBox bb15;
	BoundingBoxes::BoundingBox bb16;
	BoundingBoxes::BoundingBox bb17;
	BoundingBoxes::BoundingBox bb18;
	BoundingBoxes::BoundingBox bb19;
	BoundingBoxes::BoundingBox bb20;
	BoundingBoxes::BoundingBox bb21;
	BoundingBoxes::BoundingBox bb22;
	BoundingBoxes::BoundingBox bb23;
	BoundingBoxes::BoundingBox bb24;
	BoundingBoxes::BoundingBox bb25;
	BoundingBoxes::BoundingBox bb26;
	BoundingBoxes::BoundingBox bb27;
	BoundingBoxes::BoundingBox bb28;
	BoundingBoxes::BoundingBox bb29;
	BoundingBoxes::BoundingBox bb30;
	BoundingBoxes::BoundingBox bb31;
	std::vector<BoundingBoxes::BoundingBox> boxes;

	std::vector<Projectile*> mProjectiles;
	Sprite::Frame* projectileFrame = new Sprite::Frame();

	std::vector<JetpackParticle> mParticles;

	ID3D11Buffer* mParticleVB;
	ID3D11ShaderResourceView* mParticleTexture;

	ID3D11BlendState* mAdditiveBS;
	ID3D11BlendState* mTransparentBS;
	ID3D11DepthStencilState* mNoDepthDS;
	ID3D11DepthStencilState* mFontDS;

	bool mMouseReleased;
	POINT mLastMousePos;

public:
	std::vector<Sprite::Frame*> projFrame;
	float cooldownTimer = 0.0f;
	bool canShoot = true;
	bool isFacingRight = true;
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
D3DApp(hInstance), mLitTexEffect(0), mMouseReleased(true), m2DCam(0), mPlayer(0), mBG(0), mEnemy1(0), mEnemy2(0), mEnemy3(0)
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

	for (int i = 0; i < enemies.size(); ++i)
	{
		if (enemies[i])
			delete enemies[i];
	}

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
	result = sys->createSound("Sounds/thunder.ogg", FMOD_DEFAULT, 0, &sound1);  //fire arrow sfx (placeholder)
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

	//projectile image
	ID3D11ShaderResourceView* projImage;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/arrow.png", 0, 0, &projImage, 0);
	//projectile frame
	projectileFrame->imageWidth = 22;
	projectileFrame->imageHeight = 9;
	projectileFrame->x = 0;
	projectileFrame->y = 0;
	projectileFrame->image = projImage;
	projFrame.push_back(projectileFrame);

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

	//Player spritesheet image
	Sprite::Frame* newFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* image;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/player.png", 0, 0, &image, 0);  //test (has walk right only)
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

	//Enemy1 spritesheet image
	Sprite::Frame* enemyFrame1 = new Sprite::Frame();
	ID3D11ShaderResourceView* enemyImage1;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/enemyStrip1.png", 0, 0, &enemyImage1, 0);
	//Enemy1 frame1
	enemyFrame1->imageWidth = 96;
	enemyFrame1->imageHeight = 32;
	enemyFrame1->x = 0;
	enemyFrame1->y = 0;
	enemyFrame1->image = enemyImage1;
	std::vector<Sprite::Frame*> enemyFrames1;
	enemyFrames1.push_back(enemyFrame1);
	//Enemy1 frame2
	enemyFrame1 = new Sprite::Frame();
	enemyFrame1->imageWidth = 96;
	enemyFrame1->imageHeight = 32;
	enemyFrame1->x = 32;
	enemyFrame1->y = 0;
	enemyFrame1->image = enemyImage1;
	enemyFrames1.push_back(enemyFrame1);
	//Enemy1 frame3
	enemyFrame1 = new Sprite::Frame();
	enemyFrame1->imageWidth = 96;
	enemyFrame1->imageHeight = 32;
	enemyFrame1->x = 64;
	enemyFrame1->y = 0;
	enemyFrame1->image = enemyImage1;
	enemyFrames1.push_back(enemyFrame1);

	//Enemy2 spritesheet image
	Sprite::Frame* enemyFrame2 = new Sprite::Frame();
	ID3D11ShaderResourceView* enemyImage2;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/enemyStrip2.png", 0, 0, &enemyImage2, 0);
	//Enemy2 frame1
	enemyFrame2->imageWidth = 96;
	enemyFrame2->imageHeight = 32;
	enemyFrame2->x = 0;
	enemyFrame2->y = 0;
	enemyFrame2->image = enemyImage2;
	std::vector<Sprite::Frame*> enemyFrames2;
	enemyFrames2.push_back(enemyFrame2);
	//Enemy2 frame2
	enemyFrame2 = new Sprite::Frame();
	enemyFrame2->imageWidth = 96;
	enemyFrame2->imageHeight = 32;
	enemyFrame2->x = 32;
	enemyFrame2->y = 0;
	enemyFrame2->image = enemyImage2;
	enemyFrames2.push_back(enemyFrame2);
	//Enemy2 frame3
	enemyFrame2 = new Sprite::Frame();
	enemyFrame2->imageWidth = 96;
	enemyFrame2->imageHeight = 32;
	enemyFrame2->x = 64;
	enemyFrame2->y = 0;
	enemyFrame2->image = enemyImage2;
	enemyFrames2.push_back(enemyFrame2);

	//Enemy3 spritesheet image
	Sprite::Frame* enemyFrame3 = new Sprite::Frame();
	ID3D11ShaderResourceView* enemyImage3;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/enemyStrip3.png", 0, 0, &enemyImage3, 0);
	//Enemy3 frame1
	enemyFrame3->imageWidth = 96;
	enemyFrame3->imageHeight = 32;
	enemyFrame3->x = 0;
	enemyFrame3->y = 0;
	enemyFrame3->image = enemyImage3;
	std::vector<Sprite::Frame*> enemyFrames3;
	enemyFrames3.push_back(enemyFrame3);
	//Enemy3 frame2
	enemyFrame3 = new Sprite::Frame();
	enemyFrame3->imageWidth = 96;
	enemyFrame3->imageHeight = 32;
	enemyFrame3->x = 32;
	enemyFrame3->y = 0;
	enemyFrame3->image = enemyImage3;
	enemyFrames3.push_back(enemyFrame3);
	//Enemy3 frame3
	enemyFrame3 = new Sprite::Frame();
	enemyFrame3->imageWidth = 96;
	enemyFrame3->imageHeight = 32;
	enemyFrame3->x = 64;
	enemyFrame3->y = 0;
	enemyFrame3->image = enemyImage3;
	enemyFrames3.push_back(enemyFrame3);

	//background
	mBG = new Sprite(XMVectorSet(mClientWidth / 2.0f, mClientHeight / 2.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), 
		1024.0f, 768.0f, 1.0f, bgFrame, 0.25f, md3dDevice);
	//player
	mPlayer = new Player(XMVectorSet(mClientWidth / 2.0f, mClientHeight / 2.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		32, 32, 0.1f, frames, 0.25f, md3dDevice);
	//enemy1
	mEnemy1 = new Enemy(XMVectorSet(800.0f, 500.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		32.0f, 32.0f, 0.1f, enemyFrames1, 0.25f, md3dDevice);
	enemies.push_back(mEnemy1);
	//enemy2
	mEnemy2 = new Enemy(XMVectorSet(600.0f, 700.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		32.0f, 32.0f, 0.1f, enemyFrames2, 0.25f, md3dDevice);
	enemies.push_back(mEnemy2);
	//enemy3
	mEnemy3 = new Enemy(XMVectorSet(800, 200.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		32.0f, 32.0f, 0.1f, enemyFrames3, 0.25f, md3dDevice);
	enemies.push_back(mEnemy3);

	mPlayer->Play(true);

	for (int i = 0; i < enemies.size(); ++i)
	{
		enemies[i]->Play(true);
	}

	//result = sys->playSound(sound1, 0, false, &channel);

	InitBoundingBoxes();

	return true;
}

void InClassProj::InitBoundingBoxes()
{
	//player bounding box
	playerBB.pos.x = mPlayer->GetPos().m128_f32[0];
	playerBB.pos.y = mPlayer->GetPos().m128_f32[1];
	playerBB.height = 12.0f;
	playerBB.width = 12.0f;

	//environment bounding boxes
	bb1.pos = XMFLOAT2(0.0f, 736.0f);
	bb1.height = 32.0f;
	bb1.width = 1024.0f;
	boxes.push_back(bb1);

	bb2.pos = XMFLOAT2(0.0f, 32.0f);
	bb2.height = 704.0f;
	bb2.width = 40.0f;
	boxes.push_back(bb2);

	bb3.pos = XMFLOAT2(0.0f, 0.0f);
	bb3.height = 48.0f;
	bb3.width = 1024.0f;
	boxes.push_back(bb3);

	bb4.pos = XMFLOAT2(995.0f, 32.0f);
	bb4.height = 704.0f;
	bb4.width = 32.0f;
	boxes.push_back(bb4);

	bb5.pos = XMFLOAT2(32.0f, 672.0f);
	bb5.height = 64.0f;
	bb5.width = 40.0f;
	boxes.push_back(bb5);

	bb6.pos = XMFLOAT2(64.0f, 704.0f);
	bb6.height = 32.0f;
	bb6.width = 72.0f;
	boxes.push_back(bb6);

	bb7.pos = XMFLOAT2(899.0f, 704.0f);
	bb7.height = 32.0f;
	bb7.width = 64.0f;
	boxes.push_back(bb7);

	bb8.pos = XMFLOAT2(963.0f, 672.0f);
	bb8.height = 64.0f;
	bb8.width = 32.0f;
	boxes.push_back(bb8);

	bb9.pos = XMFLOAT2(227.0f, 608.0f);
	bb9.height = 48.0f;
	bb9.width = 70.0f;
	boxes.push_back(bb9);

	bb10.pos = XMFLOAT2(419.0f, 576.0f);
	bb10.height = 48.0f;
	bb10.width = 198.0f;
	boxes.push_back(bb10);

	bb11.pos = XMFLOAT2(451.0f, 544.0f);
	bb11.height = 48.0f;
	bb11.width = 134.0f;
	boxes.push_back(bb11);

	bb12.pos = XMFLOAT2(739.0f, 608.0f);
	bb12.height = 48.0f;
	bb12.width = 70.0f;
	boxes.push_back(bb12);

	bb13.pos = XMFLOAT2(32.0f, 479.0f);
	bb13.height = 81.0f;
	bb13.width = 105.0f;
	boxes.push_back(bb13);

	bb14.pos = XMFLOAT2(128.0f, 512.0f);
	bb14.height = 48.0f;
	bb14.width = 73.0f;
	boxes.push_back(bb14);

	bb15.pos = XMFLOAT2(835.0f, 512.0f);
	bb15.height = 48.0f;
	bb15.width = 73.0f;
	boxes.push_back(bb15);

	bb16.pos = XMFLOAT2(899.0f, 479.0f);
	bb16.height = 81.0f;
	bb16.width = 105.0f;
	boxes.push_back(bb16);

	bb17.pos = XMFLOAT2(259.0f, 351.0f);
	bb17.height = 113.0f;
	bb17.width = 102.0f;
	boxes.push_back(bb17);

	bb18.pos = XMFLOAT2(675.0f, 351.0f);
	bb18.height = 113.0f;
	bb18.width = 102.0f;
	boxes.push_back(bb18);

	bb19.pos = XMFLOAT2(32.0f, 224.0f);
	bb19.height = 112.0f;
	bb19.width = 72.0f;
	boxes.push_back(bb19);

	bb20.pos = XMFLOAT2(487.0f, 256.0f);
	bb20.height = 78.0f;
	bb20.width = 64.0f;
	boxes.push_back(bb20);

	bb21.pos = XMFLOAT2(932.0f, 224.0f);
	bb21.height = 112.0f;
	bb21.width = 72.0f;
	boxes.push_back(bb21);

	bb22.pos = XMFLOAT2(32.0f, 160.0f);
	bb22.height = 79.0f;
	bb22.width = 104.0f;
	boxes.push_back(bb22);

	bb23.pos = XMFLOAT2(453.0f, 192.0f);
	bb23.height = 78.0f;
	bb23.width = 130.0f;
	boxes.push_back(bb23);

	bb24.pos = XMFLOAT2(900.0f, 160.0f);
	bb24.height = 80.0f;
	bb24.width = 104.0f;
	boxes.push_back(bb24);

	bb25.pos = XMFLOAT2(32.0f, 32.0f);
	bb25.height = 144.0f;
	bb25.width = 264.0f;
	boxes.push_back(bb25);

	bb26.pos = XMFLOAT2(288.0f, 128.0f);
	bb26.height = 48.0f;
	bb26.width = 41.0f;
	boxes.push_back(bb26);

	bb27.pos = XMFLOAT2(484.0f, 159.0f);
	bb27.height = 48.0f;
	bb27.width = 70.0f;
	boxes.push_back(bb27);

	bb28.pos = XMFLOAT2(707.0f, 128.0f);
	bb28.height = 48.0f;
	bb28.width = 41.0f;
	boxes.push_back(bb28);

	bb29.pos = XMFLOAT2(740.0f, 32.0f);
	bb29.height = 144.0f;
	bb29.width = 264.0f;
	boxes.push_back(bb29);

	bb30.pos = XMFLOAT2(227.0f, 350.0f);
	bb30.height = 45.0f;
	bb30.width = 40.0f;
	boxes.push_back(bb30);

	bb31.pos = XMFLOAT2(771.0f, 350.0f);
	bb31.height = 45.0f;
	bb31.width = 40.0f;
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

//between enemies and BBs (player and environment)
void InClassProj::SpriteRectCollision(Sprite* sprite, BoundingBoxes::BoundingBox bb)
{
	float r1CentreX = sprite->GetPos().m128_f32[0] + sprite->GetWidth() / 2;
	float r1CentreY = sprite->GetPos().m128_f32[1] + sprite->GetHeight() / 2;

	float r2CentreX = bb.pos.x + bb.width / 2;
	float r2CentreY = bb.pos.y + bb.height / 2;

	float diffX = r1CentreX - r2CentreX;
	float diffY = r1CentreY - r2CentreY;
	float halfWidths = (sprite->GetWidth() + bb.width) / 2;
	float halfHeights = (sprite->GetHeight() + bb.height) / 2;

	float overlapX = halfWidths - abs(diffX);
	float overlapY = halfHeights - abs(diffY);

	if (overlapX > 0 && overlapY > 0)
	{
		//check to see which axis to correct on (smallest overlap)
		if (overlapX >= overlapY)
		{
			//correct sprite on y
			if (r1CentreY < r2CentreY)
			{
				sprite->SetPos(XMVectorSet(sprite->GetPos().m128_f32[0], sprite->GetPos().m128_f32[1] - overlapY, 0.0f, 0.0f));
			}
			else
			{
				sprite->SetPos(XMVectorSet(sprite->GetPos().m128_f32[0], sprite->GetPos().m128_f32[1] + overlapY, 0.0f, 0.0f));
			}
		}
		else
		{
			//correct sprite on x
			if (r1CentreX < r2CentreX)
			{
				sprite->SetPos(XMVectorSet(sprite->GetPos().m128_f32[0] - overlapX, sprite->GetPos().m128_f32[1], 0.0f, 0.0f));
			}
			else
			{
				sprite->SetPos(XMVectorSet(sprite->GetPos().m128_f32[0] + overlapX, sprite->GetPos().m128_f32[1], 0.0f, 0.0f));
			}
		}
	}
}

//between playerBB and environmentBBs
//returns which side of sprite was hit: 0 = top, 1 = right, 2 = bot, 3 = left
InClassProj::CollisionSide InClassProj::RectRectCollision(BoundingBoxes::BoundingBox r1, BoundingBoxes::BoundingBox r2, Sprite* sprite)
{
	float r1CentreX = r1.pos.x + r1.width / 2;
	float r1CentreY = r1.pos.y + r1.height / 2;

	float r2CentreX = r2.pos.x + r2.width / 2;
	float r2CentreY = r2.pos.y + r2.height / 2;

	float diffX = r1CentreX - r2CentreX;
	float diffY = r1CentreY - r2CentreY;
	float halfWidths = (r1.width + r2.width) / 2;
	float halfHeights = (r1.height + r2.height) / 2;

	float overlapX = halfWidths - abs(diffX);
	float overlapY = halfHeights - abs(diffY);

	if (overlapX > 0 && overlapY > 0)
	{
		//check to see which axis to correct on (smallest overlap) and which side of player was hit
		if (overlapX >= overlapY)
		{
			//correct player on y
			if (r1CentreY < r2CentreY)
			{
				sprite->SetPos(XMVectorSet(sprite->GetPos().m128_f32[0], sprite->GetPos().m128_f32[1] - overlapY, 0.0f, 0.0f));
				return CollisionSide::top;
			}
			else
			{
				sprite->SetPos(XMVectorSet(sprite->GetPos().m128_f32[0], sprite->GetPos().m128_f32[1] + overlapY, 0.0f, 0.0f));
				return CollisionSide::bot;
			}
		}
		else
		{
			//correct player on x
			if (r1CentreX < r2CentreX)
			{
				sprite->SetPos(XMVectorSet(sprite->GetPos().m128_f32[0] - overlapX, sprite->GetPos().m128_f32[1], 0.0f, 0.0f));
				return CollisionSide::right;
			}
			else
			{
				sprite->SetPos(XMVectorSet(sprite->GetPos().m128_f32[0] + overlapX, sprite->GetPos().m128_f32[1], 0.0f, 0.0f));
				return CollisionSide::left;
			}
		}
	}
}

//between enemies and arrow projectiles
bool InClassProj::EnemyProjCollision(Sprite* sprite, Projectile* arrow)
{
	float r1CentreX = sprite->GetPos().m128_f32[0] + sprite->GetWidth() / 2;
	float r1CentreY = sprite->GetPos().m128_f32[1] + sprite->GetHeight() / 2;

	float p1CentreX = arrow->GetPos().m128_f32[0] + arrow->GetProjWidth() / 2;
	float p1CentreY = arrow->GetPos().m128_f32[1] + arrow->GetProjHeight() / 2;

	float diffX = r1CentreX - p1CentreX;
	float diffY = r1CentreY - p1CentreY;
	float halfWidths = (sprite->GetWidth() + arrow->GetProjWidth()) / 2;
	float halfHeights = (sprite->GetHeight() + arrow->GetProjHeight()) / 2;

	float overlapX = halfWidths - abs(diffX);
	float overlapY = halfHeights - abs(diffY);

	if (overlapX > 0 && overlapY > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void InClassProj::UpdateScene(float dt)
{
	playerBB.pos.x = mPlayer->GetPos().m128_f32[0];
	playerBB.pos.y = mPlayer->GetPos().m128_f32[1];

	UpdateKeyboardInput(dt);
	
	m2DCam->Update();

	//update particles
	for (int i = 0; i < mParticles.size(); ++i)
	{
		XMVECTOR vel = XMLoadFloat3(&mParticles[i].vel);
		XMVECTOR pos = XMLoadFloat3(&mParticles[i].pos);
		pos = pos + vel;
		XMStoreFloat3(&mParticles[i].pos, pos);
	}
	UpdateParticleVB();

	//update player
	mPlayer->Update(dt);
	mPlayer->AddForce(XMVectorSet(0.0f, -9.81f, 0.0f, 0.0f));   //adds gravity

	//update enemies
	for (int i = 0; i < enemies.size(); ++i)
	{
		enemies[i]->Update(dt);
		enemies[i]->Chase(enemies, mPlayer, dt);

		//collision checks between enemies and playerBB		
		SpriteRectCollision(enemies[i], playerBB);

		//collision checks between enemies and environmentBBs
		for (int j = 0; j < boxes.size(); ++j)
		{
			SpriteRectCollision(enemies[i], boxes[j]);
		}
	}

	//collision between playerBB and environmentBBs
	for (int i = 0; i < boxes.size(); ++i)
	{
		RectRectCollision(playerBB, boxes[i], mPlayer);
	}

	//updating mGrounded bool for player jump
	for (int i = 0; i < boxes.size(); ++i)
	{
		if (RectRectCollision(playerBB, boxes[i], mPlayer) == CollisionSide::bot)
		{
			mPlayer->HitGround();
		}
	}

	//update projectiles
	for (int i = 0; i < mProjectiles.size(); ++i)
	{
 		mProjectiles[i]->Update(dt);
		for (int j = 0; j < enemies.size(); ++j)
		{
			if (mProjectiles[i]->GetDistanceTravelled() > mProjectiles[i]->MAX_DISTANCE/* ||
				mProjectiles[i]->GetDistanceTravelled() > mProjectiles[i]->MIN_DISTANCE*/)
			{
				delete mProjectiles[i];
				mProjectiles.erase(mProjectiles.begin() + i);
				i--;
				break;
			}
			//collision checks between enemies and projectiles	
			if (EnemyProjCollision(enemies[j], mProjectiles[i]))
			{
				delete mProjectiles[i];
				mProjectiles.erase(mProjectiles.begin() + i);
				i--;
				delete enemies[j];
				enemies.erase(enemies.begin() + j);
				j--;
 				break;
			}
		}
	}
		
	//update sounds
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

	vp = XMMatrixIdentity();
	proj = XMLoadFloat4x4(&m2DProj);
	view = m2DCam->GetView();

	vp = vp * view * proj;

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

	ID3D11RasterizerState* rs;
	D3D11_RASTERIZER_DESC rsd;
	rsd.CullMode = D3D11_CULL_NONE;
	rsd.AntialiasedLineEnable = false;
	rsd.DepthBias = 0.0f;
	rsd.DepthBiasClamp = 0.0f;
	rsd.DepthClipEnable = true;
	rsd.FillMode = D3D11_FILL_SOLID;
	rsd.FrontCounterClockwise = true;
	rsd.MultisampleEnable = true;
	rsd.ScissorEnable = false;
	rsd.SlopeScaledDepthBias = 0.0f;
	md3dDevice->CreateRasterizerState(&rsd, &rs);
	md3dImmediateContext->RSSetState(rs);

	//draw player
	if (isFacingRight)
	{
		mPlayer->SetScale(XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f));
	}
	else if (!isFacingRight)
	{
		mPlayer->SetScale(XMVectorSet(-1.0f, 1.0f, 0.0f, 0.0f));
	}
	mPlayer->Draw(vp, md3dImmediateContext, mLitTexEffect);
	md3dImmediateContext->RSSetState(0);
	
	//draw enemies
	for (int i = 0; i < enemies.size(); ++i)
	{
		enemies[i]->Draw(vp, md3dImmediateContext, mLitTexEffect);
	}

	//draw arrow projectiles
	for (int i = 0; i < mProjectiles.size(); ++i)
	{
		mProjectiles[i]->Draw(vp, md3dImmediateContext, mLitTexEffect);
	}

	DrawParticles();
	md3dImmediateContext->OMSetDepthStencilState(0, 0);
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);

	//mFont->DrawFont(md3dImmediateContext, XMVectorSet(10.0f, 500.0f, 0.0f, 0.0f), 50, 75, 10, "Hi Brandon, you are a good student");

	HR(mSwapChain->Present(1, 0));
}

void InClassProj::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	if ((btnState & MK_RBUTTON) != 0)
	{
	}
	if ((btnState & MK_LBUTTON) != 0)
	{
		JetpackParticle newParticle;
		XMStoreFloat3(&newParticle.pos, mPlayer->GetPos() + XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		//XMStoreFloat3(&newParticle.vel, mTestPlayer->GetLook() * 0.1f);
		XMStoreFloat3(&newParticle.vel, XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) * 0.1f);
		newParticle.size.x = 1.0f;
		newParticle.size.y = 1.0f;
		mParticles.push_back(newParticle);
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
	float move = 0.0f;
	move = dt * 100;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		if (isFacingRight)
		{
			isFacingRight = false;
		}
		mPlayer->SetPos(XMVectorSet(mPlayer->GetPos().m128_f32[0] - move, mPlayer->GetPos().m128_f32[1], 0.0f, 0.0f));
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		if (!isFacingRight)
		{
			isFacingRight = true;
		}
		mPlayer->SetPos(XMVectorSet(mPlayer->GetPos().m128_f32[0] + move, mPlayer->GetPos().m128_f32[1], 0.0f, 0.0f));
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (!GetAsyncKeyState(VK_LSHIFT))
		{
			//jump
			mPlayer->Jump();
		}
	}
	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
	{
		//jetpack
		mPlayer->UseJetpack(dt);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		//shoot arrows
		if (canShoot)
		{
			if (isFacingRight)
			{
				Projectile* arrowProjectile = new Projectile(XMVectorSet(mPlayer->GetPos().m128_f32[0], mPlayer->GetPos().m128_f32[1] - 8, 0.0f, 0.0f),
					XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f), 22, 9, 0.5f, projFrame, 0.25f, md3dDevice, XMVectorSet(250.0f, 0.0f, 0.0f, 0.0f));
				mProjectiles.push_back(arrowProjectile);
				canShoot = false;
			}
			if (!isFacingRight)
			{
				Projectile* arrowProjectile = new Projectile(XMVectorSet(mPlayer->GetPos().m128_f32[0], mPlayer->GetPos().m128_f32[1] - 8, 0.0f, 0.0f),
					XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f), 22, 9, 0.5f, projFrame, 0.25f, md3dDevice, XMVectorSet(-250.0f, 0.0f, 0.0f, 0.0f));
				mProjectiles.push_back(arrowProjectile);
				canShoot = false;
			}
		}
		else
		{
			cooldownTimer++;
			if (cooldownTimer >= 15)
			{
				canShoot = true;
				cooldownTimer = 0.0f;
			}
		}

		//arrow sfx
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