#include "Game.h"
#include "JetpackArcher.h"

Game::Game() : mLitTexEffect(0), m2DCam(0), mPlayer(0), mBG(0), mEnemy1(0), mEnemy2(0), mEnemy3(0),
	mEnemy4(0), mEnemy5(0), mEnemy6(0), mGreenHBar(0), mRedHBar(0), mGreenHBarP(0), mRedHBarP(0), 
	mRedFuel(0), mGreenFuel(0), EOLobj(0)
{
	XMVECTOR pos = XMVectorSet(1.0f, 1.0f, 5.0f, 0.0f);
	XMVECTOR look = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
	XMStoreFloat4x4(&m2DProj, I);
}

Game::~Game()
{
	Vertex::CleanLayouts();

	if (mLitTexEffect)
		delete mLitTexEffect;

	if (m2DCam)
		delete m2DCam;

	if (mPlayer)
		delete mPlayer;

	for (int i = 0; i < enemies.size(); ++i)
	{
		if (enemies[i])
			delete enemies[i];
		if (redBarVec[i])
			delete redBarVec[i];
		if (greenBarVec[i])
			delete greenBarVec[i];
	}

	if (mGreenHBar)
		delete mGreenHBar;

	if (mRedHBar)
		delete mRedHBar;

	if (mGreenHBarP)
		delete mGreenHBarP;

	if (mRedHBarP)
		delete mRedHBarP;

	if (mGreenFuel)
		delete mGreenFuel;

	if (mRedFuel)
		delete mRedFuel;

	if (EOLobj)
		delete EOLobj;

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

bool Game::Init(ID3D11Device* md3dDevice)
{
	mLitTexEffect = new LitTexEffect();
	mLitTexEffect->LoadEffect(L"FX/lighting.fx", md3dDevice);

	mParticleEffect = new ParticleEffect();
	mParticleEffect->LoadEffect(L"FX/ParticleEffect.fx", md3dDevice);

	//camera
	m2DCam = new BaseCamera(XMVectorSet(0.0f, 0.0f, -0.5f, 0.0f), XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	m2DCam->Update();

	Vertex::InitParticleVertLayout(md3dDevice, mParticleEffect->GetTech());

	BuildParticleVB(md3dDevice);

	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/TestAdditive.png", 0, 0, &mParticleTexture, 0);

	BuildBlendStates(md3dDevice);
	BuildDSStates(md3dDevice);

	//font
	ID3D11ShaderResourceView* font;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/fontW.png", 0, 0, &font, 0);
	mHealthFont = new FontRasterizer(m2DCam, XMLoadFloat4x4(&m2DProj), mLitTexEffect, 10, 10, font, md3dDevice);
	mFuelFont = new FontRasterizer(m2DCam, XMLoadFloat4x4(&m2DProj), mLitTexEffect, 10, 10, font, md3dDevice);
	mControlsFont = new FontRasterizer(m2DCam, XMLoadFloat4x4(&m2DProj), mLitTexEffect, 10, 10, font, md3dDevice);

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
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/player.png", 0, 0, &image, 0);
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

	//Enemy4 spritesheet image
	Sprite::Frame* enemyFrame4 = new Sprite::Frame();
	ID3D11ShaderResourceView* enemyImage4;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/enemy4.png", 0, 0, &enemyImage4, 0);
	//Enemy4 frame1
	enemyFrame4->imageWidth = 96;
	enemyFrame4->imageHeight = 32;
	enemyFrame4->x = 0;
	enemyFrame4->y = 0;
	enemyFrame4->image = enemyImage4;
	std::vector<Sprite::Frame*> enemyFrames4;
	enemyFrames4.push_back(enemyFrame4);
	//Enemy4 frame2
	enemyFrame4 = new Sprite::Frame();
	enemyFrame4->imageWidth = 96;
	enemyFrame4->imageHeight = 32;
	enemyFrame4->x = 32;
	enemyFrame4->y = 0;
	enemyFrame4->image = enemyImage4;
	enemyFrames4.push_back(enemyFrame4);
	//Enemy4 frame3
	enemyFrame4 = new Sprite::Frame();
	enemyFrame4->imageWidth = 96;
	enemyFrame4->imageHeight = 32;
	enemyFrame4->x = 64;
	enemyFrame4->y = 0;
	enemyFrame4->image = enemyImage4;
	enemyFrames4.push_back(enemyFrame4);

	//Enemy5 spritesheet image
	Sprite::Frame* enemyFrame5 = new Sprite::Frame();
	ID3D11ShaderResourceView* enemyImage5;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/enemy5.png", 0, 0, &enemyImage5, 0);
	//Enemy5 frame1
	enemyFrame5->imageWidth = 96;
	enemyFrame5->imageHeight = 32;
	enemyFrame5->x = 0;
	enemyFrame5->y = 0;
	enemyFrame5->image = enemyImage5;
	std::vector<Sprite::Frame*> enemyFrames5;
	enemyFrames5.push_back(enemyFrame5);
	//Enemy5 frame2
	enemyFrame5 = new Sprite::Frame();
	enemyFrame5->imageWidth = 96;
	enemyFrame5->imageHeight = 32;
	enemyFrame5->x = 32;
	enemyFrame5->y = 0;
	enemyFrame5->image = enemyImage5;
	enemyFrames5.push_back(enemyFrame5);
	//Enemy5 frame3
	enemyFrame5 = new Sprite::Frame();
	enemyFrame5->imageWidth = 96;
	enemyFrame5->imageHeight = 32;
	enemyFrame5->x = 64;
	enemyFrame5->y = 0;
	enemyFrame5->image = enemyImage5;
	enemyFrames5.push_back(enemyFrame5);

	//Enemy6 spritesheet image
	Sprite::Frame* enemyFrame6 = new Sprite::Frame();
	ID3D11ShaderResourceView* enemyImage6;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/enemy6.png", 0, 0, &enemyImage6, 0);
	//Enemy6 frame1
	enemyFrame6->imageWidth = 96;
	enemyFrame6->imageHeight = 32;
	enemyFrame6->x = 0;
	enemyFrame6->y = 0;
	enemyFrame6->image = enemyImage6;
	std::vector<Sprite::Frame*> enemyFrames6;
	enemyFrames6.push_back(enemyFrame6);
	//Enemy6 frame2
	enemyFrame6 = new Sprite::Frame();
	enemyFrame6->imageWidth = 96;
	enemyFrame6->imageHeight = 32;
	enemyFrame6->x = 32;
	enemyFrame6->y = 0;
	enemyFrame6->image = enemyImage6;
	enemyFrames6.push_back(enemyFrame6);
	//Enemy6 frame3
	enemyFrame6 = new Sprite::Frame();
	enemyFrame6->imageWidth = 96;
	enemyFrame6->imageHeight = 32;
	enemyFrame6->x = 64;
	enemyFrame6->y = 0;
	enemyFrame6->image = enemyImage6;
	enemyFrames6.push_back(enemyFrame6);

	//enemy green health bar image
	Sprite::Frame* greenHBFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* greenHBimage;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/greenHealth.png", 0, 0, &greenHBimage, 0);
	greenHBFrame->imageWidth = 32;
	greenHBFrame->imageHeight = 8;
	greenHBFrame->x = 0;
	greenHBFrame->y = 0;
	greenHBFrame->image = greenHBimage;
	hbFrameG.push_back(greenHBFrame);
	//enemy red health bar image
	Sprite::Frame* redHBFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* redHBimage;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/redHealth.png", 0, 0, &redHBimage, 0);
	redHBFrame->imageWidth = 32;
	redHBFrame->imageHeight = 8;
	redHBFrame->x = 0;
	redHBFrame->y = 0;
	redHBFrame->image = redHBimage;
	hbFrameR.push_back(redHBFrame);

	//green fuel bar image
	Sprite::Frame* greenFBFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* greenFBimage;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/greenFuel.png", 0, 0, &greenFBimage, 0);
	greenFBFrame->imageWidth = 96;
	greenFBFrame->imageHeight = 16;
	greenFBFrame->x = 0;
	greenFBFrame->y = 0;
	greenFBFrame->image = greenFBimage;
	fbFrameG.push_back(greenFBFrame);
	//red fuel bar image
	Sprite::Frame* redFBFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* redFBimage;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/redFuel.png", 0, 0, &redFBimage, 0);
	redFBFrame->imageWidth = 96;
	redFBFrame->imageHeight = 16;
	redFBFrame->x = 0;
	redFBFrame->y = 0;
	redFBFrame->image = redFBimage;
	fbFrameR.push_back(redFBFrame);

	//end of level object image
	Sprite::Frame* EOLobjFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* EOLobjImg;
	D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/EOLobj.png", 0, 0, &EOLobjImg, 0);
	//EOLobj frame1
	EOLobjFrame->imageWidth = 96;
	EOLobjFrame->imageHeight = 32;
	EOLobjFrame->x = 0;
	EOLobjFrame->y = 0;
	EOLobjFrame->image = EOLobjImg;
	EOLobjFrames.push_back(EOLobjFrame);
	//EOLobj frame2
	EOLobjFrame = new Sprite::Frame();
	EOLobjFrame->imageWidth = 96;
	EOLobjFrame->imageHeight = 32;
	EOLobjFrame->x = 32;
	EOLobjFrame->y = 0;
	EOLobjFrame->image = EOLobjImg;
	EOLobjFrames.push_back(EOLobjFrame);
	//EOLobj frame3
	EOLobjFrame = new Sprite::Frame();
	EOLobjFrame->imageWidth = 96;
	EOLobjFrame->imageHeight = 32;
	EOLobjFrame->x = 64;
	EOLobjFrame->y = 0;
	EOLobjFrame->image = EOLobjImg;
	EOLobjFrames.push_back(EOLobjFrame);

	//centre of screen:(512.0f, 384.0f)
	//background
	mBG = new Sprite(XMVectorSet(512.0f, 384.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		1024.0f, 768.0f, 1.0f, bgFrame, 0.25f, md3dDevice, 0.0f);
	//player
	mPlayer = new Player(XMVectorSet(512.0f, 384.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		32, 32, 0.1f, frames, 0.25f, md3dDevice, 3.0f);
	//enemy1
	mEnemy1 = new Enemy(XMVectorSet(800.0f, 500.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		32.0f, 32.0f, 0.1f, enemyFrames1, 0.25f, md3dDevice, 5.0f);
	enemies.push_back(mEnemy1);
	//enemy2
	mEnemy2 = new Enemy(XMVectorSet(600.0f, 700.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		32.0f, 32.0f, 0.1f, enemyFrames2, 0.25f, md3dDevice, 5.0f);
	enemies.push_back(mEnemy2);
	//enemy3
	mEnemy3 = new Enemy(XMVectorSet(800, 200.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		32.0f, 32.0f, 0.1f, enemyFrames3, 0.25f, md3dDevice, 5.0f);
	enemies.push_back(mEnemy3);
	//enemy4
	mEnemy4 = new Enemy(XMVectorSet(200.0f, 500.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		32.0f, 32.0f, 0.1f, enemyFrames4, 0.25f, md3dDevice, 5.0f);
	enemies.push_back(mEnemy4);
	//enemy5
	mEnemy5 = new Enemy(XMVectorSet(200.0f, 700.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		32.0f, 32.0f, 0.1f, enemyFrames5, 0.25f, md3dDevice, 5.0f);
	enemies.push_back(mEnemy5);
	//enemy6
	mEnemy6 = new Enemy(XMVectorSet(200.0f, 200.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		32.0f, 32.0f, 0.1f, enemyFrames6, 0.25f, md3dDevice, 5.0f);
	enemies.push_back(mEnemy6);

	//enemy health bars
	for (int i = 0; i < enemies.size(); ++i)
	{
		mGreenHBar = new Sprite(XMVectorSet(enemies[i]->GetPos().m128_f32[0], enemies[i]->GetPos().m128_f32[1] + 32, 0.0f, 0.0f),
			XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), 32.0f, 8.0f, 1.0f, hbFrameG, 0.25f, md3dDevice, 0.0f);
		greenBarVec.push_back(mGreenHBar);
		mRedHBar = new Sprite(XMVectorSet(redXPos, enemies[i]->GetPos().m128_f32[1] + 32, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
			 32.0f, 8.0f, 1.0f, hbFrameR, 0.25f, md3dDevice, 0.0f);
		redBarVec.push_back(mRedHBar);
	}
	//player health bar
	mGreenHBarP = new Sprite(XMVectorSet(60.0f, 736.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), 
		96.0f, 16.0f, 1.0f, hbFrameG, 0.25f, md3dDevice, 0.0f);
	mRedHBarP = new Sprite(XMVectorSet(redXPosP, 736.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), 
		96.0f, 16.0f, 1.0f, hbFrameR, 0.25f, md3dDevice, 0.0f);

	//fuel bar
	mGreenFuel = new Sprite(XMVectorSet(60.0f, 698.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		96.0f, 16.0f, 1.0f, fbFrameG, 0.25f, md3dDevice, 0.0f);
	mRedFuel = new Sprite(XMVectorSet(redXPosF, 698.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		96.0f, 16.0f, 1.0f, fbFrameR, 0.25f, md3dDevice, 0.0f);

	//end of level object
	EOLobj = new Sprite(XMVectorSet(512.0f, 720.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), //centre on x, 48 down trom top
		32.0f, 32.0f, 1.0f, EOLobjFrames, 0.25f, md3dDevice, 0.0f);

	mPlayer->Play(true);

	for (int i = 0; i < enemies.size(); ++i)
	{
		enemies[i]->Play(true);
	}

	InitBoundingBoxes();

	return true;
}

void Game::InitBoundingBoxes()
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

void Game::BuildBlendStates(ID3D11Device* md3dDevice)
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

void Game::BuildDSStates(ID3D11Device* md3dDevice)
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

void Game::BuildParticleVB(ID3D11Device* md3dDevice)
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

void Game::UpdateParticleVB(ID3D11DeviceContext* md3dImmediateContext)
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

//between enemies and BBs (player and environment)
void Game::SpriteRectCollision(Sprite* sprite, BoundingBoxes::BoundingBox bb)
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
Game::CollisionSide Game::RectRectCollision(BoundingBoxes::BoundingBox r1, BoundingBoxes::BoundingBox r2, Sprite* sprite)
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
bool Game::EnemyProjCollision(Sprite* sprite, Projectile* arrow)
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

//for player health decrementing
bool Game::PlayerEnemyCollision(Sprite* player, Sprite* enemy)
{
	float r1CentreX = player->GetPos().m128_f32[0] + player->GetWidth() / 2;
	float r1CentreY = player->GetPos().m128_f32[1] + player->GetHeight() / 2;

	float p1CentreX = enemy->GetPos().m128_f32[0] + enemy->GetWidth() / 2;
	float p1CentreY = enemy->GetPos().m128_f32[1] + enemy->GetHeight() / 2;

	float diffX = r1CentreX - p1CentreX;
	float diffY = r1CentreY - p1CentreY;
	float halfWidths = (player->GetWidth() + enemy->GetWidth()) / 2;
	float halfHeights = (player->GetHeight() + enemy->GetHeight()) / 2;

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

void Game::UpdateScene(ID3D11DeviceContext* md3dImmediateContext, ID3D11Device* md3dDevice, float dt, JetpackArcher* instance)
{
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

	if (recoverTime > 0.0f)
	{
		recoverTime = recoverTime - dt;
	}
	else
	{
		recoverTime = 0.0f;
	}

	playerBB.pos.x = mPlayer->GetPos().m128_f32[0];
	playerBB.pos.y = mPlayer->GetPos().m128_f32[1];

	UpdateKeyboardInput(md3dDevice, dt);

	m2DCam->Update();

	//update particles
	for (int i = 0; i < mParticles.size(); ++i)
	{
		XMVECTOR vel = XMLoadFloat3(&mParticles[i].vel);
		XMVECTOR pos = XMLoadFloat3(&mParticles[i].pos);
		pos = pos + vel;
		XMStoreFloat3(&mParticles[i].pos, pos);
	}
	UpdateParticleVB(md3dImmediateContext);

	//update player
	mPlayer->Update(dt);
	mPlayer->AddForce(XMVectorSet(0.0f, -9.8f, 0.0f, 0.0f));   //adds gravity
	//update enemy damage done to player
	for (int i = 0; i < enemies.size(); ++i)
	{
		if (PlayerEnemyCollision(mPlayer, enemies[i]) && recoverTime == 0.0f)
		{
			enemies[i]->ApplyDamage(mPlayer);
			/*std::wstringstream ss;
			ss << mPlayer->GetHealth();
			OutputDebugString(ss.str().c_str());
			OutputDebugString(L"\n");*/
			recoverTime = 3.0f;   //player has 3 seconds before damage can be done to him again
		}
	}
	//set player health bar
	redXPosP = 60.0f;
	currHealthP = mPlayer->GetHealth();
	maxHealthP = 3.0f;
	ratioP = 1.0f - (currHealthP / maxHealthP);
	redXPosP += ((1.0f - ratioP) / 2.0f) * 96.0f; //96 is barSize
	redXScaleP = ratioP;
	mGreenHBarP->SetPos(XMVectorSet(60.0f, 736.0f, 0.0f, 0.0f));
	mRedHBarP->SetPos(XMVectorSet(redXPosP, 736.0f, 0.0f, 0.0f));
	mRedHBarP->SetScale(XMVectorSet(redXScaleP, 1.0f, 0.0f, 0.0f));
	//player dead / game over
	if (mPlayer->GetHealth() == 0)
	{
		instance->SetState(JetpackArcher::States::GAME_OVER);
	}
	//collision between player and end of level object / game won
	if (RectRectCollision(playerBB, EOLobjBB, mPlayer) == CollisionSide::top || 
		RectRectCollision(playerBB, EOLobjBB, mPlayer) == CollisionSide::left || 
		RectRectCollision(playerBB, EOLobjBB, mPlayer) == CollisionSide::right)
	{
		instance->SetState(JetpackArcher::States::GAME_WON);
	}

	//update enemies
	for (int i = 0; i < enemies.size(); ++i)
	{
		enemies[i]->Update(dt);
		enemies[i]->Chase(enemies, mPlayer, dt);

		//set health bars to above enemies' heads and red bar to adjust accordingly
		redXPos = enemies[i]->GetPos().m128_f32[0];
		currHealth = enemies[i]->GetHealth();
		maxHealth = 5.0f;
		ratio = 1.0f - (currHealth / maxHealth);
		redXPos += ((1.0f - ratio) / 2.0f) * 32.0f; //32 is barSize
		redXScale = ratio;
		greenBarVec[i]->SetPos(XMVectorSet(enemies[i]->GetPos().m128_f32[0], enemies[i]->GetPos().m128_f32[1] + 32, 0.0f, 0.0f));
		redBarVec[i]->SetPos(XMVectorSet(redXPos, enemies[i]->GetPos().m128_f32[1] + 32, 0.0f, 0.0f));
		redBarVec[i]->SetScale(XMVectorSet(redXScale, 1.0f, 0.0f, 0.0f));

		//collision checks between enemies and playerBB		
		SpriteRectCollision(enemies[i], playerBB);

		//collision checks between enemies and environmentBBs
		for (int j = 0; j < boxes.size(); ++j)
		{
			SpriteRectCollision(enemies[i], boxes[j]);
		}

		//delete enemy upon death
		if (enemies[i]->GetHealth() == 0)
		{
			delete enemies[i];
			enemies.erase(enemies.begin() + i);
			i--;
			break;
		}
	}
	//when all enemies eliminated:
	if (enemies.size() == 0)
	{
		EOLobjActive = true;
		EOLobjBB.pos = XMFLOAT2(EOLobj->GetPos().m128_f32[0], EOLobj->GetPos().m128_f32[1]);
		EOLobjBB.height = 32.0f;
		EOLobjBB.width = 32.0f;
	}

	//collision between playerBB and environmentBBs
	for (int i = 0; i < boxes.size(); ++i)
	{
		RectRectCollision(playerBB, boxes[i], mPlayer);

		//update mGrounded bool for player jump
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
			if (mProjectiles[i]->GetDistanceTravelled() > mProjectiles[i]->MAX_DISTANCE ||
				mProjectiles[i]->GetDistanceTravelled() < mProjectiles[i]->MIN_DISTANCE)
			{
				delete mProjectiles[i];
				mProjectiles.erase(mProjectiles.begin() + i);
				i--;
				break;
			}
			//collision checks between enemies and projectiles	
			if (EnemyProjCollision(enemies[j], mProjectiles[i]))
			{
				mProjectiles[i]->ApplyDamage(enemies[j]);
				/*std::wstringstream ss;
				ss << enemies[j]->GetHealth();
				OutputDebugString(ss.str().c_str());
				OutputDebugString(L"\n");*/
				delete mProjectiles[i];
				mProjectiles.erase(mProjectiles.begin() + i);
				i--;
				break;
			}
		}
	}

	mGreenFuel->Update(dt);
	mRedFuel->Update(dt);

	EOLobj->Update(dt);
}

void Game::DrawScene(ID3D11DeviceContext* md3dImmediateContext, CXMMATRIX vp, IDXGISwapChain* mSwapChain, ID3D11RenderTargetView* mRenderTargetView,
	ID3D11DepthStencilView* mDepthStencilView, PointLightOptimized mPointLight, SpotLightOptimized mSpotLight, XMFLOAT4 mAmbientColour)
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::White));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(Vertex::GetNormalTexVertLayout());
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMVECTOR ambient = XMLoadFloat4(&mAmbientColour);

	XMVECTOR eyePos = XMVectorSet(m2DCam->GetPos().m128_f32[0], m2DCam->GetPos().m128_f32[1], m2DCam->GetPos().m128_f32[2], 0.0f);

	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX view = m2DCam->GetView();

	mLitTexEffect->SetPerFrameParams(ambient, eyePos, mPointLight, mSpotLight);
	
	XMMATRIX tempVP = vp;

	md3dImmediateContext->IASetInputLayout(Vertex::GetNormalTexVertLayout());
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	md3dImmediateContext->OMSetBlendState(mTransparentBS, blendFactor, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(mFontDS, 0);

	mBG->Draw(vp, md3dImmediateContext, mLitTexEffect);


	//draw end of level object
	if (EOLobjActive)
	{
		EOLobj->Draw(vp, md3dImmediateContext, mLitTexEffect);
	}

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

	//draw player health bar
	mGreenHBarP->Draw(vp, md3dImmediateContext, mLitTexEffect);
	mRedHBarP->Draw(vp, md3dImmediateContext, mLitTexEffect);

	//draw fuel bar 
	mGreenFuel->Draw(vp, md3dImmediateContext, mLitTexEffect);
	mRedFuel->Draw(vp, md3dImmediateContext, mLitTexEffect);

	//draw enemies
	for (int i = 0; i < enemies.size(); ++i)
	{
		enemies[i]->Draw(vp, md3dImmediateContext, mLitTexEffect);

		//draw enemy health bars
		greenBarVec[i]->Draw(vp, md3dImmediateContext, mLitTexEffect);
		redBarVec[i]->Draw(vp, md3dImmediateContext, mLitTexEffect);
	}

	//draw arrow projectiles
	for (int i = 0; i < mProjectiles.size(); ++i)
	{
		mProjectiles[i]->Draw(vp, md3dImmediateContext, mLitTexEffect);
	}

	DrawParticles(md3dImmediateContext);

	//draw HUD font
	mHealthFont->DrawFont(md3dImmediateContext, XMVectorSet(60.0f, 732.0f, 0.0f, 0.0f), 96, 20, 10, "Health"/*, vp*/);
	mFuelFont->DrawFont(md3dImmediateContext, XMVectorSet(60.0f, 704.0f, 0.0f, 0.0f), 96, 20, 10, "Jetpack Fuel"/*, vp*/);
	//make a timer for this font
	mControlsFont->DrawFont(md3dImmediateContext, XMVectorSet(600.0f, 100.0f, 0.0f, 0.0f), 50, 50, 21,
		"Controls: UP to Jump, SPACE to Fire Arrow, Hold CTRL for Jetpack"/*, vp*/);

	md3dImmediateContext->OMSetDepthStencilState(0, 0);
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);

	
}

void Game::OnMouseDown(HWND mhMainWnd, WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	//left mouse button will eventually be used for menu selections not particles
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

void Game::OnMouseUp(WPARAM btnState, int x, int y)
{
	mMouseReleased = false;
	ReleaseCapture();
}

void Game::OnMouseMove(WPARAM btnState, int x, int y)
{
	// Make each pixel correspond to a quarter of a degree.
	float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
	float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void Game::UpdateKeyboardInput(ID3D11Device* md3dDevice, float dt)
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
		//can't jump while jetpack in use
		if (!GetAsyncKeyState(VK_LCONTROL) || !GetAsyncKeyState(VK_RCONTROL))
		{
			//jump
			mPlayer->Jump();
		}
	}
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000)
	{
		//activate jetpack
		fuelRecoverTime += dt;
		if (canUseJetpack)
		{
			if (fuelRecoverTime > 3.0f)
			{
				mFuel--;
				fuelRecoverTime = 0.0f;
			}
			//set fuel health bar
			redXPosF = 60.0f;
			currHealthF = mFuel;
			maxHealthF = 5.0f;
			ratioF = 1.0f - (currHealthF / maxHealthF);
			redXPosF += ((1.0f - ratioF) / 2.0f) * 96.0f; //96 is barSize
			redXScaleF = ratioF;
			mGreenFuel->SetPos(XMVectorSet(60.0f, 698.0f, 0.0f, 0.0f));
			mRedFuel->SetPos(XMVectorSet(redXPosF, 698.0f, 0.0f, 0.0f));
			mRedFuel->SetScale(XMVectorSet(redXScaleF, 1.0f, 0.0f, 0.0f));

			std::wstringstream ss;
			ss << mFuel;
			OutputDebugString(ss.str().c_str());
			OutputDebugString(L"\n");

			mPlayer->UseJetpack(dt);
		}
		if (mFuel == 0)
		{
			canUseJetpack = false;
		}
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		//shoot arrows
		if (canShoot)
		{
			if (isFacingRight)
			{
				Projectile* arrowProjectile = new Projectile(XMVectorSet(mPlayer->GetPos().m128_f32[0], mPlayer->GetPos().m128_f32[1] - 8, 0.0f, 0.0f),
					XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f), 22, 9, 0.5f, projFrame, 0.25f, md3dDevice, 0.0f, XMVectorSet(250.0f, 0.0f, 0.0f, 0.0f));
				mProjectiles.push_back(arrowProjectile);
				canShoot = false;
			}
			if (!isFacingRight)
			{
				Projectile* arrowProjectile = new Projectile(XMVectorSet(mPlayer->GetPos().m128_f32[0], mPlayer->GetPos().m128_f32[1] - 8, 0.0f, 0.0f),
					XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f), 22, 9, 0.5f, projFrame, 0.25f, md3dDevice, 0.0f, XMVectorSet(-250.0f, 0.0f, 0.0f, 0.0f));
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
	}
}

void Game::DrawParticles(ID3D11DeviceContext* md3dImmediateContext)
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