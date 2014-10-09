#include "MainMenu.h"
#include "Vertex.h"
#include "JetpackArcher.h"

MainMenu::MainMenu() : mBG(0)
{
}

MainMenu::~MainMenu()
{
	if (mBG)
	{
		delete mBG;
		mBG = 0;
	}
}

void MainMenu::Init(ID3D11Device* device, UINT16 clientW, UINT16 clientH)
{
	Sprite::Frame* BGFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* BGimage;
	D3DX11CreateShaderResourceViewFromFile(device, L"Textures/MenuScreen.png", 0, 0, &BGimage, 0);
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

	//start bounding box
	playBB.pos = XMFLOAT2(335.0f, 420.0f);
	playBB.height = 90.0f;
	playBB.width = 320.0f;

	//credits bounding box
	creditsBB.pos = XMFLOAT2(320.0f, 516.0f);
	creditsBB.height = 90.0f;
	creditsBB.width = 384.0f;

	//quit bounding box
	quitBB.pos = XMFLOAT2(430.0f, 668.0f);
	quitBB.height = 70.0f;
	quitBB.width = 168.0f;
}

void MainMenu::DrawScene(CXMMATRIX vp, ID3D11DeviceContext* context, LitTexEffect* texEffect)
{
	mBG->Draw(vp, context, texEffect);
}

void MainMenu::UpdateScene(float dt)
{
	mBG->Update(dt);
}

void MainMenu::CheckClick(POINT mousePos, JetpackArcher* instance)
{

	//if start button clicked
	if (mousePos.x > playBB.pos.x && mousePos.x < playBB.pos.x + playBB.width &&
		mousePos.y > playBB.pos.y && mousePos.y < playBB.pos.y + playBB.height)
	{
		instance->SetState(JetpackArcher::States::GAME);
	}
	//if credits button clicked
	else if (mousePos.x > creditsBB.pos.x && mousePos.x < creditsBB.pos.x + creditsBB.width &&
		mousePos.y > creditsBB.pos.y && mousePos.y < creditsBB.pos.y + creditsBB.height)
	{
		instance->SetState(JetpackArcher::States::CREDITS);
	}
	//if quit button clicked
	else if (mousePos.x > quitBB.pos.x && mousePos.x < quitBB.pos.x + quitBB.width &&
		mousePos.y > quitBB.pos.y && mousePos.y < quitBB.pos.y + quitBB.height)
	{
		exit(0);
	}
}
