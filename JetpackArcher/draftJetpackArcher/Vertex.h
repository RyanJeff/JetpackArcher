#pragma once

#include "d3dUtil.h"


class Vertex
{
public:

	struct BasicVertex
	{
		XMFLOAT3 pos;

		BasicVertex() : pos(0.0f, 0.0f, 0.0f)
		{
		}
	};

	struct NormalTexVertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 tex;
	};

	struct TerrainVertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 tiledTex;
		XMFLOAT2 tex;
	};

	struct ParticleVertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 size;
	};

private:
	static ID3D11InputLayout* mBasicVertLayout;
	static ID3D11InputLayout* mNormalTexVertLayout;
	static ID3D11InputLayout* mTerrainVertLayout;
	static ID3D11InputLayout* mParticleVertLayout;
	

public:
	static void InitBasicLayout(ID3D11Device* device, ID3DX11EffectTechnique* tech);
	static void InitLitTexLayout(ID3D11Device* device, ID3DX11EffectTechnique* tech);
	static void InitTerrainVertLayout(ID3D11Device* device, ID3DX11EffectTechnique* tech);
	static void InitParticleVertLayout(ID3D11Device* device, ID3DX11EffectTechnique* tech);

	
	static ID3D11InputLayout* GetBasicVertLayout()
	{
		return mBasicVertLayout;
	}

	static ID3D11InputLayout* GetNormalTexVertLayout()
	{
		return mNormalTexVertLayout;
	}

	static ID3D11InputLayout* GetTerrainVertLayout()
	{
		return mTerrainVertLayout;
	}

	static ID3D11InputLayout* GetParticleVertLayout()
	{
		return mParticleVertLayout;
	}

	static void CleanLayouts();

private:
	Vertex(){}
};