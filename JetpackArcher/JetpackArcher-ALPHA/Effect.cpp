#include "Effect.h"
#include "Vertex.h"

Effect::Effect(void)
{
}

Effect::~Effect(void)
{

	ReleaseCOM(mEffect);   //use the macro from the author's code
}

void Effect::LoadEffect(std::wstring filename, ID3D11Device* device)
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(filename.c_str(), 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if( compilationMsgs != 0 )
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if(FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, device, &mEffect));

	// Done with compiled shader.
	ReleaseCOM(compiledShader);
	ReleaseCOM(compilationMsgs);

	LoadEffectParams();
}

//note: the expected names of the corresponding params in the fx file are gWVP for the wvp matrix, and Tech for the technique
void Effect::LoadEffectParams()
{
	mTech  = mEffect->GetTechniqueByName("TestTech");
	mfxWVP = mEffect->GetVariableByName("gWVP")->AsMatrix();
}

void Effect::Draw(ID3D11DeviceContext* context, ID3D11Buffer* vb, ID3D11Buffer* ib, int indexCount)
{
	UINT stride = sizeof(Vertex::NormalTexVertex);
    UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer( ib, DXGI_FORMAT_R32_UINT, 0);

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexed(indexCount, 0, 0);
	}
}

void Effect::Draw(ID3D11DeviceContext* context, ID3D11Buffer* vb, ID3D11Buffer* ib, int startIndex, int indexCount)
{
	UINT stride = sizeof(Vertex::NormalTexVertex);
    UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer( ib, DXGI_FORMAT_R32_UINT, 0);

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexed(indexCount, startIndex, 0);
	}
}

void Effect::SetPerObjectParams(CXMMATRIX wvp)
{
	mfxWVP->SetMatrix(reinterpret_cast<const float*>(&wvp));
}

void SkyBoxEffect::LoadEffectParams()
{
	mfxCubeMap = mEffect->GetVariableByName("gCubeMap")->AsShaderResource();
	Effect::LoadEffectParams();
}

void SkyBoxEffect::SetPerObjectParams(CXMMATRIX wvp, ID3D11ShaderResourceView* skybox)
{
	mfxCubeMap->SetResource(skybox);
	Effect::SetPerObjectParams(wvp);
}

void SkyBoxEffect::Draw(ID3D11DeviceContext* context, ID3D11Buffer* vb, ID3D11Buffer* ib, int indexCount)
{
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R16_UINT, 0);

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexed(indexCount, 0, 0);
	}
}

LitTexEffect::~LitTexEffect()
{
}

void LitTexEffect::SetPerFrameParams(FXMVECTOR ambient, FXMVECTOR eyePos, const PointLightOptimized& point, const SpotLightOptimized& spot)
{
	mfxEyePos->SetRawValue(&eyePos, 0, 12);
	mfxPointLight->SetRawValue(&point, 0, sizeof(PointLightOptimized));
	mfxSpotLight->SetRawValue(&spot, 0, sizeof(SpotLightOptimized));
	mfxAmbientLight->SetRawValue(&ambient, 0, sizeof(XMVECTOR));
}

void LitTexEffect::SetPerObjectParams(CXMMATRIX world, CXMMATRIX invTranspose, CXMMATRIX wvp, ID3D11ShaderResourceView* texture)
{
	Effect::SetPerObjectParams(wvp);
	mfxInvTranspose->SetMatrix(reinterpret_cast<const float*>(&invTranspose));
	mfxWorld->SetMatrix(reinterpret_cast<const float*>(&world));
	mfxDiffuseMap->SetResource(texture);
}

void LitTexEffect::SetDiffuseMap(ID3D11ShaderResourceView* texture)
{
	mfxDiffuseMap->SetResource(texture);
}

void LitTexEffect::LoadEffectParams()
{
	Effect::LoadEffectParams();

	mfxAmbientLight = mEffect->GetVariableByName("gAmbientLight")->AsVector();
	mfxEyePos = mEffect->GetVariableByName("gEyePos")->AsVector();
	mfxPointLight = mEffect->GetVariableByName("gPointLight");
	mfxSpotLight = mEffect->GetVariableByName("gSpotLight");
	mfxWorld = mEffect->GetVariableByName("gW")->AsMatrix();
	mfxInvTranspose = mEffect->GetVariableByName("gInvTransposeW")->AsMatrix();
	mfxDiffuseMap = mEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

TerrainEffect::~TerrainEffect()
{	
	delete [] mfxDiffuseMaps;
}

void TerrainEffect::SetPerObjectParams(CXMMATRIX world, CXMMATRIX invTranspose, CXMMATRIX wvp, ID3D11ShaderResourceView* texture, 
	ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* texture3, ID3D11ShaderResourceView* texture4, ID3D11ShaderResourceView* blendMap)
{
	LitTexEffect::SetPerObjectParams(world, invTranspose, wvp, texture);

	mfxDiffuseMaps[0]->SetResource(texture2);
	mfxDiffuseMaps[1]->SetResource(texture3);
	mfxDiffuseMaps[2]->SetResource(texture4);
	mfxBlendMap->SetResource(blendMap);
}

void TerrainEffect::LoadEffectParams()
{
	LitTexEffect::LoadEffectParams();

	mfxDiffuseMaps = new ID3DX11EffectShaderResourceVariable*[3];
	mfxDiffuseMaps[0] = mEffect->GetVariableByName("gDiffuseMap2")->AsShaderResource();
	mfxDiffuseMaps[1] = mEffect->GetVariableByName("gDiffuseMap3")->AsShaderResource();
	mfxDiffuseMaps[2] = mEffect->GetVariableByName("gDiffuseMap4")->AsShaderResource();
	mfxBlendMap = mEffect->GetVariableByName("gBlend")->AsShaderResource();
}

void TerrainEffect::Draw(ID3D11DeviceContext* context, ID3D11Buffer* vb, ID3D11Buffer* ib, int indexCount)
{
	UINT stride = sizeof(Vertex::TerrainVertex);
    UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer( ib, DXGI_FORMAT_R32_UINT, 0);

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, context);
		context->DrawIndexed(indexCount, 0, 0);
	}
}

void ParticleEffect::LoadEffectParams()
{
	Effect::LoadEffectParams();
	mfxEyePos = mEffect->GetVariableByName("gEyePosW")->AsVector();
	mfxDiffuseMap = mEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

void ParticleEffect::SetPerFrameParams(FXMVECTOR eyePos)
{
	mfxEyePos->SetRawValue(&eyePos, 0, 12);
}

void ParticleEffect::SetPerObjectParams(CXMMATRIX wvp, ID3D11ShaderResourceView* texture)
{
	Effect::SetPerObjectParams(wvp);
	mfxDiffuseMap->SetResource(texture);
}

void ParticleEffect::SetDiffuseMap(ID3D11ShaderResourceView* texture)
{
	mfxDiffuseMap->SetResource(texture);
}