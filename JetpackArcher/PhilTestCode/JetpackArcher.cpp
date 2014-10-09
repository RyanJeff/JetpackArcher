#include "JetpackArcher.h"
#include "Game.h"
#include "Splash.h"
#include "MainMenu.h"
#include "Credits.h"
#include "GameOver.h"
#include "GameWon.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	JetpackArcher theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}

JetpackArcher::JetpackArcher(HINSTANCE hInstance) :
D3DApp(hInstance), mLitTexEffect(0), mMouseReleased(true), mCurrState(GAME_WON)
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

	mainMenu = 0;
	game = 0;
	credits = 0;
	splash = 0;
	gameOver = 0;
	gameWon = 0;
}

JetpackArcher::~JetpackArcher()
{
	Vertex::CleanLayouts();

	if(mLitTexEffect)
		delete mLitTexEffect;

	//if (splash)
	//	delete splash;

	//if (mainMenu)
	//	delete mainMenu;

	//if (credits)
	//	delete credits;

	//if (gameWon)
	//	delete gameWon;

	//if (gameOver)
	//	delete gameOver;

}

FMOD_RESULT result;
FMOD::System     *sys;
FMOD::Sound      *sound1, *sound2, *sound3;
FMOD::Channel    *channel = 0;
unsigned int      version;
void             *extradriverdata = 0;

void JetpackArcher::BuildSceneLights()
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

bool JetpackArcher::Init()
{
	if (!D3DApp::Init())
		return false;

	//FMOD sounds
	result = FMOD::System_Create(&sys);
	result = sys->getVersion(&version);
	if (version < FMOD_VERSION)
	{
		OutputDebugString(L"FMOD lib version doesn't match header version");
	}
	result = sys->init(32, FMOD_INIT_NORMAL, extradriverdata);
	result = sys->createSound("Sounds/choo.ogg", FMOD_DEFAULT, 0, &sound1);  //fire arrow sfx (placeholder?)
	result = sound1->setMode(FMOD_LOOP_OFF);

	mLitTexEffect = new LitTexEffect();
	mLitTexEffect->LoadEffect(L"FX/lighting.fx", md3dDevice);

	m2DCam = new BaseCamera();
	Vertex::InitLitTexLayout(md3dDevice, mLitTexEffect->GetTech());

	BuildSceneLights();
	BuildBlendStates();
	BuildDSStates();

	splash = new Splash();
	splash->Init(md3dDevice, mClientWidth, mClientHeight);

	//mainMenu = new MainMenu();
	//mainMenu->Init(md3dDevice, mClientWidth, mClientHeight);

	//credits = new Credits();
	//credits->Init(md3dDevice, mClientWidth, mClientHeight);

	//game = new Game();
	//game->Init(md3dDevice);

	//gameWon = new GameWon();
	//gameWon->Init(md3dDevice, mClientWidth, mClientHeight);

	//gameOver = new GameOver();
	//gameOver->Init(md3dDevice, mClientWidth, mClientHeight);

	//result = sys->playSound(sound1, 0, false, &channel);

	return true;
}

void JetpackArcher::OnResize()
{
	D3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);

	P = XMMatrixOrthographicOffCenterLH(0.0f, mClientWidth, 0.0f, mClientHeight, 0.0001f, 1000.0f);
	XMStoreFloat4x4(&m2DProj, P);
}
void JetpackArcher::UpdateScene(float dt)
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
	UpdateKeyboardInput(dt);

	//md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::White));
	//md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//md3dImmediateContext->IASetInputLayout(Vertex::GetNormalTexVertLayout());
	//md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	switch (GetState())
	{
	case SPLASH:
		if (!splash)
		{
			Destroy();
			splash = new Splash();
			splash->Init(md3dDevice, mClientWidth, mClientHeight);
		}
		splashTimer += dt;
		if (splashTimer >= 2.0f)
		{
			mainMenu = new MainMenu();
			mainMenu->Init(md3dDevice, mClientWidth, mClientHeight);
			SetState(MAIN_MENU);
			break;
		}
		splash->UpdateScene(dt);
		break;
	case MAIN_MENU:
		if (!mainMenu)
		{
			Destroy();
			mainMenu = new MainMenu();
			mainMenu->Init(md3dDevice, mClientWidth, mClientHeight);
		}
		mainMenu->UpdateScene(dt);
		break;
	case CREDITS:
		if (!credits)
		{
			Destroy();
			credits = new Credits();
			credits->Init(md3dDevice, mClientWidth, mClientHeight);
		}
		credits->UpdateScene(dt);
		break;
	case GAME:
		if (!game)
		{
			Destroy();
			game = new Game();
			game->Init(md3dDevice);
		}
		game->UpdateScene(md3dImmediateContext, md3dDevice, dt, this);
		break;
	case GAME_WON:
		if (!gameWon)
		{
			Destroy();
			gameWon = new GameWon();
			gameWon->Init(md3dDevice, mClientWidth, mClientHeight);
		}
		gameWon->UpdateScene(dt);
		break;
	case GAME_OVER:
		if (!gameOver)
		{
			Destroy();
			gameOver = new GameOver();
			gameOver->Init(md3dDevice, mClientWidth, mClientHeight);
		}
		gameOver->UpdateScene(dt);
		break;
	}

	//update sounds
	sys->update();
}

void JetpackArcher::DrawScene()
{
	m2DCam->Update();
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::White));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

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

	switch (GetState())
	{
	case SPLASH:
		splash->DrawScene(vp, md3dImmediateContext, mLitTexEffect);
		break;
	case CREDITS:
		credits->DrawScene(vp, md3dImmediateContext, mLitTexEffect);
		break;
	case MAIN_MENU:
		mainMenu->DrawScene(vp, md3dImmediateContext, mLitTexEffect);
		break;
	case GAME:
		game->DrawScene(md3dImmediateContext, vp, mSwapChain, mRenderTargetView, mDepthStencilView, mPointLight, mSpotLight, mAmbientColour);
		break;
	case GAME_OVER:
		gameOver->DrawScene(vp, md3dImmediateContext, mLitTexEffect);
		break;
	case GAME_WON:
		gameWon->DrawScene(vp, md3dImmediateContext, mLitTexEffect);
		break;
	}

	HR(mSwapChain->Present(1, 0));
}

void JetpackArcher::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);

	if (mainMenu)
	{
		mainMenu->CheckClick(mLastMousePos, this);
	}
	else if (credits)
	{
		credits->CheckClick(mLastMousePos, this);
	}
	else if (gameOver)
	{
		gameOver->CheckClick(mLastMousePos, this);
	}
	else if (gameWon)
	{
		gameWon->CheckClick(mLastMousePos, this);
	}
}

void JetpackArcher::OnMouseUp(WPARAM btnState, int x, int y)
{
	mMouseReleased = false;
	ReleaseCapture();
}

void JetpackArcher::OnMouseMove(WPARAM btnState, int x, int y)
{
	// Make each pixel correspond to a quarter of a degree.
	float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
	float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void JetpackArcher::UpdateKeyboardInput(float dt)
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		//arrow sfx
		bool isPlaying = false;
		channel->isPlaying(&isPlaying);
		if (!isPlaying)
		{
			result = sys->playSound(sound1, 0, false, &channel);
		}
	}
}

void JetpackArcher::BuildBlendStates()
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

void JetpackArcher::BuildDSStates()
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

void JetpackArcher::SetState(States state)
{
	mCurrState = state;
}

JetpackArcher::States JetpackArcher::GetState()
{
	if (mCurrState == GAME_OVER)
	{
		if (!gameOver)
		{
			Destroy();
			gameOver = new GameOver();
			gameOver->Init(md3dDevice, mClientWidth, mClientHeight);
		}	
	}
	else if (mCurrState == GAME_WON)
	{
		if (!gameWon)
		{
			Destroy();
			gameWon = new GameWon();
			gameWon->Init(md3dDevice, mClientWidth, mClientHeight);
		}		
	}

	return mCurrState;
}

void JetpackArcher::Destroy()
{
	if (mainMenu)
	{
		delete mainMenu;
		mainMenu = 0;
	}
	if (game)
	{
		delete game;
		game = 0;
	}
	if (credits)
	{
		delete credits;
		credits = 0;
	}
	if (gameOver)
	{
		delete gameOver;
		gameOver = 0;
	}
	if (gameWon)
	{
		delete gameWon;
		gameWon = 0;
	}
	if (splash)
	{
		delete splash;
		splash = 0;
	}
}