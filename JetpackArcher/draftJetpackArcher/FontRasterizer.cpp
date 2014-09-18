#include "FontRasterizer.h"


const int MAX_CHARS = 2000; //on the screen per frame

FontRasterizer::FontRasterizer(BaseCamera* cam, CXMMATRIX proj, LitTexEffect* effect,
	int numRows, int numCols, ID3D11ShaderResourceView* texture,
	ID3D11Device* device) : 
	m2DCam(cam),
	mEffect(effect),
	mNumRows(numRows),
	mNumCols(numCols),
	mFontImage(texture)
{
	XMStoreFloat4x4(&m2DProj, proj);
	InitVB(device);
	InitIB(device);
}


FontRasterizer::~FontRasterizer()
{
}

void FontRasterizer::InitVB(ID3D11Device* device)
{
	std::vector<Vertex::NormalTexVertex> vertices(MAX_CHARS);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex::NormalTexVertex) * MAX_CHARS;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbd, &vinitData, &VB));
}

void FontRasterizer::InitIB(ID3D11Device* device)
{
	std::vector<UINT> indices(MAX_CHARS * 6);

	int currIndex = 0;
	for (int i = 0; i < MAX_CHARS; ++i)
	{
		indices[currIndex] = i * 4;
		indices[currIndex + 1] = (i * 4) + 1;
		indices[currIndex + 2] = (i * 4) + 2;

		indices[currIndex + 3] = (i * 4) + 0;
		indices[currIndex + 4] = (i * 4) + 2;
		indices[currIndex + 5] = (i * 4) + 3;

		currIndex += 6;
	}


	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(device->CreateBuffer(&ibd, &iinitData, &IB));
}

void FontRasterizer::DrawFont(ID3D11DeviceContext* context, FXMVECTOR pos, float fontWidth, float fontHeight, int charsPerLine, std::string text)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(context->Map(VB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	Vertex::NormalTexVertex* v = reinterpret_cast<Vertex::NormalTexVertex*> (mappedData.pData);

	float x = pos.m128_f32[0];
	float y = pos.m128_f32[1];
	for (int i = 0; i < text.size(); ++i)
	{

		//draw a character
		DrawCharacter(text[i], x, y, fontWidth, fontHeight, v, i);

		x += fontWidth;
		if (i % charsPerLine == charsPerLine - 1)
		{
			y -= fontHeight;
			x = pos.m128_f32[0];
		}
	}

	context->Unmap(VB, 0);

	XMMATRIX vp = XMMatrixIdentity();
	XMMATRIX proj = XMLoadFloat4x4(&m2DProj);
	XMMATRIX view = m2DCam->GetView();

	vp = vp * view * proj;

	context->IASetInputLayout(Vertex::GetNormalTexVertLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	mEffect->SetPerObjectParams(XMMatrixIdentity(), XMMatrixIdentity(),
		vp, mFontImage);

	mEffect->Draw(context, VB, IB, text.size() * 6);

	
}

void FontRasterizer::DrawCharacter(char c, float xPos, float yPos,
	float charWidth, float charHeight, Vertex::NormalTexVertex* vb,
	int charCount)
{
	int index = c - ' ';
	int vbIndex = charCount * 4;

	int row = index / mNumCols;
	int col = index % mNumCols;

	float u = (float)col / (float)mNumCols;
	float v = (float)row / (float)mNumRows;

	float uWidth = 1.0f / mNumCols;
	float vHeight = 1.0f / mNumRows;

	//top left
	vb[vbIndex].pos.x = xPos;
	vb[vbIndex].pos.y = yPos;
	vb[vbIndex].pos.z = 0.0f;
	vb[vbIndex].tex.x = u;
	vb[vbIndex].tex.y = v;
	XMStoreFloat3(&vb[vbIndex].normal, XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));

	//top right
	vb[vbIndex + 1].pos.x = xPos + charWidth;
	vb[vbIndex + 1].pos.y = yPos;
	vb[vbIndex + 1].pos.z = 0.0f;
	vb[vbIndex + 1].tex.x = u + uWidth;
	vb[vbIndex + 1].tex.y = v;
	XMStoreFloat3(&vb[vbIndex + 1].normal, XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));

	//bottom right
	vb[vbIndex + 2].pos.x = xPos + charWidth;
	vb[vbIndex + 2].pos.y = yPos - charHeight;
	vb[vbIndex + 2].pos.z = 0.0f;
	vb[vbIndex + 2].tex.x = u + uWidth;
	vb[vbIndex + 2].tex.y = v + vHeight;
	XMStoreFloat3(&vb[vbIndex + 2].normal, XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));

	//bottom left
	vb[vbIndex + 3].pos.x = xPos;
	vb[vbIndex + 3].pos.y = yPos - charHeight;
	vb[vbIndex + 3].pos.z = 0.0f;
	vb[vbIndex + 3].tex.x = u;
	vb[vbIndex + 3].tex.y = v + vHeight;
	XMStoreFloat3(&vb[vbIndex + 3].normal, XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));

}