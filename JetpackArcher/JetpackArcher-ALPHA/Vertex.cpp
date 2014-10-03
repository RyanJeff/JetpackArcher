#include "Vertex.h"

ID3D11InputLayout* Vertex::mBasicVertLayout = 0;
ID3D11InputLayout* Vertex::mNormalTexVertLayout = 0;
ID3D11InputLayout* Vertex::mTerrainVertLayout = 0;
ID3D11InputLayout* Vertex::mParticleVertLayout = 0;

void Vertex::InitBasicLayout(ID3D11Device* device, ID3DX11EffectTechnique* tech)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3DX11_PASS_DESC passDesc;
    tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(vertexDesc, 1, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mBasicVertLayout));
}

void Vertex::InitLitTexLayout(ID3D11Device* device, ID3DX11EffectTechnique* tech)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3DX11_PASS_DESC passDesc;
    tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(vertexDesc, 3, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mNormalTexVertLayout));
}

void Vertex::InitTerrainVertLayout(ID3D11Device* device, ID3DX11EffectTechnique* tech)
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3DX11_PASS_DESC passDesc;
    tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(vertexDesc, 4, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mTerrainVertLayout));
}

void Vertex::InitParticleVertLayout(ID3D11Device* device, ID3DX11EffectTechnique* tech)
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(vertexDesc, 2 , passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mParticleVertLayout));
}

void Vertex::CleanLayouts()
{
	ReleaseCOM(mTerrainVertLayout);
	ReleaseCOM(mNormalTexVertLayout);
	ReleaseCOM(mBasicVertLayout);
	ReleaseCOM(mParticleVertLayout);
}