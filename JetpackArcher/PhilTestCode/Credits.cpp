#include "Credits.h"
#include "Sprite.h"
#include "JetpackArcher.h"

Credits::Credits() : mBG(0)
{
}


Credits::~Credits()
{
	if (mBG)
	{
		delete mBG;
		mBG = 0;
	}
}

void Credits::Init(ID3D11Device* device, UINT16 clientW, UINT16 clientH)
{
	Sprite::Frame* BGFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* BGimage;
	D3DX11CreateShaderResourceViewFromFile(device, L"Textures/CreditScreen.png", 0, 0, &BGimage, 0);
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

	//back button bounding box
	backBB.pos = XMFLOAT2(416.0f, (630.0f));
	backBB.height = 68.0f;
	backBB.width = 192.0f;
}

void Credits::DrawScene(CXMMATRIX vp, ID3D11DeviceContext* context, LitTexEffect* texEffect)
{
	mBG->Draw(vp, context, texEffect);
}

void Credits::UpdateScene(float dt)
{
	mBG->Update(dt);
}

void Credits::CheckClick(POINT mousePos, JetpackArcher* instance)
{
	//if back button pressed
	if (mousePos.x > backBB.pos.x && mousePos.x < backBB.pos.x + backBB.width &&
		mousePos.y > backBB.pos.y && mousePos.y < backBB.pos.y + backBB.height)
	{
		instance->SetState(JetpackArcher::States::MAIN_MENU);
	}
}