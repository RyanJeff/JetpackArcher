#include "GameOver.h"
#include "JetpackArcher.h"

GameOver::GameOver() : mBG(0)
{
}


GameOver::~GameOver()
{
	if (mBG)
	{
		//mBG = 0;
		delete mBG;
	}
}

void GameOver::Init(ID3D11Device* device, UINT16 clientW, UINT16 clientH)
{
	Sprite::Frame* BGFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* BGimage;
	D3DX11CreateShaderResourceViewFromFile(device, L"Textures/GameOverBackground.png", 0, 0, &BGimage, 0);
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

	InitBoundingBoxes();

}

void GameOver::DrawScene(CXMMATRIX vp, ID3D11DeviceContext* context, LitTexEffect* texEffect)
{
	mBG->Draw(vp, context, texEffect);
}

void GameOver::UpdateScene(float dt)
{
	mBG->Update(dt);
}

void GameOver::InitBoundingBoxes()
{
	//Try Again? bounding box
	tryAgainBB.pos = XMFLOAT2(384.0f, (300.0f));
	tryAgainBB.height = 57.0f;
	tryAgainBB.width = 288.0f;

	//Quit bounding box
	quitBB.pos = XMFLOAT2(460.0f, (376.0f));
	quitBB.height = 58.0f;
	quitBB.width = 460.0f;
}

void GameOver::CheckClick(POINT mousePos, JetpackArcher* instance)
{
	//check if over Try Again? button
	if (mousePos.x >= tryAgainBB.pos.x && mousePos.x <= (tryAgainBB.pos.x + tryAgainBB.width))
	{
		if (mousePos.y >= (tryAgainBB.pos.y) && mousePos.y <= (tryAgainBB.pos.y + tryAgainBB.height))
		{
			instance->SetState(2); //2 is enum of GAME
		}
	}

	//check if over Quit button
	if (mousePos.x >= quitBB.pos.x && mousePos.x <= (quitBB.pos.x + quitBB.width))
	{
		if (mousePos.y >= (quitBB.pos.y) && mousePos.y <= (quitBB.pos.y + quitBB.height))
		{
			exit(0); //Exit Game
		}
	}
}