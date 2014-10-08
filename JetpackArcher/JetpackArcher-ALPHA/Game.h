#pragma once
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
#include "Enemy.h"
#include "Projectile.h"
#include "Effect.h"
#include "FontRasterizer.h"
#include "Sprite.h"
#include "xnacollision.h"
#include "fmod.hpp"

class JetpackArcher;
struct JetpackParticle
{
	XMFLOAT3 pos;
	XMFLOAT3 vel;
	XMFLOAT2 size;
};

const int MAX_PARTICLES = 100;

class Game
{
	enum CollisionSide
	{
		top,
		right,
		bot,
		left
	};

public:
	Game();
	~Game();

	bool Init(ID3D11Device* md3dDevice);
	virtual void InitBoundingBoxes();
	virtual void UpdateScene(ID3D11DeviceContext* md3dImmediateContext, ID3D11Device* md3dDevice, float dt, JetpackArcher* instance);
	virtual void DrawScene(ID3D11DeviceContext* md3dImmediateContext, CXMMATRIX vp, IDXGISwapChain* mSwapChain, ID3D11RenderTargetView* mRenderTargetView,
		ID3D11DepthStencilView* mDepthStencilView, PointLightOptimized mPointLight, SpotLightOptimized mSpotLight, XMFLOAT4 mAmbientColour);

	virtual void OnMouseDown(HWND mhMainWnd, WPARAM btnState, int x, int y);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildParticleVB(ID3D11Device* md3dDevice);
	void BuildBlendStates(ID3D11Device* md3dDevice);
	void BuildDSStates(ID3D11Device* md3dDevice);
	void UpdateParticleVB(ID3D11DeviceContext* md3dImmediateContext);

	ID3D11BlendState* mAdditiveBS;
	ID3D11BlendState* mTransparentBS;
	ID3D11DepthStencilState* mFontDS;
	ID3D11DepthStencilState* mNoDepthDS;

	ID3D11Buffer* mParticleVB;

	void BuildVertexLayout();
	void UpdateKeyboardInput(ID3D11Device* md3dDevice, float dt);

	CollisionSide RectRectCollision(BoundingBoxes::BoundingBox r1, BoundingBoxes::BoundingBox r2, Sprite* sprite);
	void SpriteRectCollision(Sprite* sprite, BoundingBoxes::BoundingBox bb);
	bool EnemyProjCollision(Sprite* sprite, Projectile* arrow);
	bool PlayerEnemyCollision(Sprite* player, Sprite* enemy);

	void DrawParticles(ID3D11DeviceContext* md3dImmediateContext);

	LitTexEffect* mLitTexEffect;
	ParticleEffect* mParticleEffect;

	BaseCamera* m2DCam;

	FontRasterizer* mHealthFont;
	FontRasterizer* mFuelFont;
	FontRasterizer* mControlsFont;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 m2DProj;

	Sprite* mBG;

	Sprite* mGreenHBar;
	std::vector<Sprite*> greenBarVec;
	Sprite* mGreenHBarP;
	Sprite* mRedHBar;
	std::vector<Sprite*> redBarVec;
	Sprite* mRedHBarP;

	Sprite* mGreenFuel;
	Sprite* mRedFuel;

	Player* mPlayer;

	Enemy* mEnemy1;
	Enemy* mEnemy2;
	Enemy* mEnemy3;
	Enemy* mEnemy4;
	Enemy* mEnemy5;
	Enemy* mEnemy6;
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

	Sprite* EOLobj;   //end of level object
	BoundingBoxes::BoundingBox EOLobjBB;

	std::vector<Projectile*> mProjectiles;
	Sprite::Frame* projectileFrame = new Sprite::Frame();

	std::vector<JetpackParticle> mParticles;

	ID3D11ShaderResourceView* mParticleTexture;

	bool mMouseReleased;
	POINT mLastMousePos;

public:
	std::vector<Sprite::Frame*> hbFrameG;
	std::vector<Sprite::Frame*> hbFrameR;
	std::vector<Sprite::Frame*> fbFrameG;
	std::vector<Sprite::Frame*> fbFrameR;
	std::vector<Sprite::Frame*> projFrame;
	std::vector<Sprite::Frame*> EOLobjFrames;

	float cooldownTimer = 0.0f;
	bool canShoot = true;
	bool isFacingRight = true;
	float recoverTime = 0.0f;
	float fuelRecoverTime = 0.0f;

	float currHealth;
	float maxHealth;
	float ratio;
	float redXPos;
	float redXScale;

	float currHealthP;
	float maxHealthP;
	float ratioP;
	float redXPosP;
	float redXScaleP;

	float currHealthF;
	float maxHealthF;
	float ratioF;
	float redXPosF;
	float redXScaleF;

	int mFuel = 5.0f;
	bool canUseJetpack = true;

	bool EOLobjActive = false;
};