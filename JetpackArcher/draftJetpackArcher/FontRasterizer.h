#pragma once

#include "BaseCamera.h"
#include "Effect.h"
#include "Vertex.h"

class FontRasterizer
{
private:
	BaseCamera* m2DCam;
	XMFLOAT4X4 m2DProj;
	LitTexEffect* mEffect;
	int mNumRows;
	int mNumCols;

	ID3D11ShaderResourceView* mFontImage;

	ID3D11Buffer* VB;
	ID3D11Buffer* IB;

	void DrawCharacter(char c, float xPos, float yPos,
		float charWidth, float charHeight, Vertex::NormalTexVertex* v,
		int charCount);

	void InitVB(ID3D11Device* device);
	void InitIB(ID3D11Device* device);

public:
	FontRasterizer(BaseCamera* cam, CXMMATRIX proj, LitTexEffect* effect,
				   int numRows, int numCols, ID3D11ShaderResourceView* texture,
				   ID3D11Device* device);
	~FontRasterizer();

	void DrawFont(ID3D11DeviceContext* context, FXMVECTOR pos, 
				float fontWidth, float fontHeight, int charsPerLine,
				std::string text);


};

