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
#include "BoundingBoxes.h"

class MainMenu;
class Splash;
class Credits;
class Game;
class GameOver;
class GameWon;
class JetpackArcher : public D3DApp
{
public:
	enum States
	{
		SPLASH,
		MAIN_MENU,
		GAME,
		CREDITS,
		GAME_WON,
		GAME_OVER
	};

	JetpackArcher(HINSTANCE hInstance);
	~JetpackArcher();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildBlendStates();
	void BuildDSStates();

	void BuildSceneLights();
	PointLightOptimized mPointLight;
	SpotLightOptimized mSpotLight;
	XMFLOAT4 mAmbientColour;

	BaseCamera* m2DCam;
	ID3D11BlendState* mAdditiveBS;
	ID3D11BlendState* mTransparentBS;
	ID3D11DepthStencilState* mFontDS;
	ID3D11DepthStencilState* mNoDepthDS;

	void UpdateKeyboardInput(float dt);

	bool mMouseReleased;
	POINT mLastMousePos;

	LitTexEffect* mLitTexEffect;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 m2DProj;

	MainMenu* mainMenu;
	Game* game;
	Credits* credits;
	Splash* splash;
	GameOver* gameOver;
	GameWon* gameWon;

public:
	States GetState();
	void SetState(States state);
	States mCurrState;

	float splashTimer = 0.0f;

	void Destroy();
};