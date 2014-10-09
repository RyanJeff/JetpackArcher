#include "GameWon.h"
#include "JetpackArcher.h"

GameWon::GameWon() : mBG(0)
{
}


GameWon::~GameWon()
{
	if (mBG)
	{
		delete mBG;
		mBG = 0;
	}
}

void GameWon::Init(ID3D11Device* device, UINT16 clientW, UINT16 clientH)
{
	Sprite::Frame* BGFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* BGimage;
	D3DX11CreateShaderResourceViewFromFile(device, L"Textures/WinBackground.png", 0, 0, &BGimage, 0);
	//bg frame
	BGFrame->imageWidth = 1024;
	BGFrame->imageHeight = 768;
	BGFrame->x = 0;
	BGFrame->y = 0;
	BGFrame->image = BGimage;
	std::vector<Sprite::Frame*> bgFrame;
	bgFrame.push_back(BGFrame);

	mBG = new Sprite(XMVectorSet(clientW / 2.0f, clientH / 2.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		1024.0f, 768.0f, 1.0f, bgFrame, 0.25f, device, 0.0f);

	//Try Again? bounding box
	tryAgainBB.pos = XMFLOAT2(314.0f, 394.0f);
	tryAgainBB.height = 66.0f;
	tryAgainBB.width = 390.0f;

	//Quit bounding box
	quitBB.pos = XMFLOAT2(426.0f, 458.0f);
	quitBB.height = 66.0f;
	quitBB.width = 166.0f;
}

void GameWon::DrawScene(CXMMATRIX vp, ID3D11DeviceContext* context, LitTexEffect* texEffect)
{
	mBG->Draw(vp, context, texEffect);
}

void GameWon::UpdateScene(float dt)
{

	mBG->Update(dt);
}

void GameWon::CheckClick(POINT mousePos, JetpackArcher* instance)
{
	//if Try Again? button clicked
	if (mousePos.x > tryAgainBB.pos.x && mousePos.x < tryAgainBB.pos.x + tryAgainBB.width &&
		mousePos.y > tryAgainBB.pos.y && mousePos.y < tryAgainBB.pos.y + tryAgainBB.height)
	{
		instance->SetState(JetpackArcher::States::GAME);
	}
	//if Quit button clicked
	else if (mousePos.x > quitBB.pos.x && mousePos.x < quitBB.pos.x + quitBB.width &&
		mousePos.y > quitBB.pos.y && mousePos.y < quitBB.pos.y + quitBB.height)
	{
		exit(0);
	}
}