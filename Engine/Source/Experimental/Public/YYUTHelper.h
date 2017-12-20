#pragma once
#include "YYUT.h"
#include <string>
#include "YYUTDXManager.h"

template<typename T>
inline void AddAlias(T & buffer, const FString& alias)
{
	HRESULT hr = S_OK;
	if (!alias.IsEmpty())
	{
		TCHAR *p = new TCHAR[alias.Len() + 1];
		memcpy_s(p, alias.Len(), *alias, alias.Len());
		p[alias.Len()] = TEXT('\0');
		if (FAILED(hr = buffer->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)alias.Len() + 1, p)))
		{
		}
	}
}

//D3DX11CompileShaderFromeFile的Asicall封装，ColomMajor是用来指定编译后的shader是行主还是列主，列主对矩阵乘法友好，manul建议用
void ComplieShaderFromFile(const FString &file_name,
	const FString& entry_point,
	const FString& shader_model,
	TComPtr<ID3DBlob> &blob,
	bool ColomMajor = true);
//用来指定对齐模式
inline int								Align(int value, int alignment)
{
	return (value + (alignment - 1))&~(alignment - 1);
}
//////////////////////////////////////////////////////////////////////////
//Create Shader
void CreateComputerShader(const FString& FileName,
	const FString& MainPoint,
	TComPtr<ID3D11ComputeShader> & cs,
	const FString & alias = "");

void CreateVertexShader(const FString& FileName,
	const FString& MainPoint,
	TComPtr<ID3D11VertexShader> &vs,
	const FString& alias /*= ""*/);
void CreatePixelShader(const FString& FileName,
	const FString& MainPoint,
	TComPtr<ID3D11PixelShader> &ps,
	const FString& alias = "");
void CreateGeometryShader(const FString& FileName,
	const FString& MainPoint,
	TComPtr<ID3D11GeometryShader> &gs,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//CreateLayout
void CreateInputLayout(const FString& FileName,
	const FString& VSMainPoint,
	const D3D11_INPUT_ELEMENT_DESC *pDesc,
	int number,
	TComPtr<ID3D11InputLayout> &InputLayout,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Create Buffer
void CreateBufferSRV_UAV(int ByteWidth, TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");
void CreateStagingReadBackBuffer(int ByteWidth,
	int StructureByteStride,
	TComPtr<ID3D11Buffer> & buffer,
	const FString& alias = "");

void CreateConstantBufferCPUWrite(int size,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");

void CreateConstantBufferDefault(int size,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");

void									CreateIndirectDrawArgsBuffer(
	int ByteWidth,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");

void									CreateVertexBuffer(
	UINT ByteWidth,
	const void *pData,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");
void									CreateVertexBufferDynamic(
	UINT byteWidth,
	const void *pData,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");
void									CreateIndexBuffer(
	UINT ByteWidth,
	const void * pData,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Create Structured buffer
void CreateStruturedBufferSRV_UAV(int numbers,
	int perSize,
	TComPtr<ID3D11Buffer> & buffer,
	const FString& alias = "");

template<bool CPUWrite,bool GPUWrite,bool CreateSRV,bool CreateUAV>
void CreateStruturedBuffer(int numbers, int perSize, TComPtr<ID3D11Buffer> & buffer, const FString& alias = "");

template<bool CPUWrite,bool GPUWrite,bool CreateSRV,bool CreateUAV>
void CreateTBuffer(int32 ByteCounts, TComPtr<ID3D11Buffer> & buffer, const FString& alias = "");

void CreateStruturedBufferUAV(int numbers,
	int perSize,
	TComPtr<ID3D11Buffer> & buffer,
	const FString& alias = "");

//////////////////////////////////////////////////////////////////////////
//Create 2D Texture
void									Create2DTextureImmuableSRV(
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	D3D11_SUBRESOURCE_DATA *data,
	TComPtr<ID3D11Texture2D> &tex2D,
	const FString& alias = "");

void									Create2DTextureRTV_SRV(
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	const FString& alias = "");
void Create2DTextureDSV_SRV(UINT width,
	UINT height,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	const FString& alias = "");
void Create2DTextureArrayDSV_SRV(UINT width,
	UINT height,
	DXGI_FORMAT format,
	UINT ArraySize,
	TComPtr<ID3D11Texture2D> &tex2D,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Create UAV
void CreateAppendUAVForStructureBuffer(int numbers,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	const FString& alias = "");

void CreateUAVForStruturedBuffer(int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	const FString& alias = "");
void									CreateCountUAVForStruturedBuffer(
	int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	const FString& alias = "");
void									CreateUAVForBuffer(
	DXGI_FORMAT format,
	int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Create SRV
void CreateSRVForStruturedBuffer(int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11ShaderResourceView> &srv,
	const FString& alias = "");

void CreateSRVForTBuffer(DXGI_FORMAT format,
	int32 number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11ShaderResourceView> &srv,
	const FString& alias = "");

void									CreateSRVForTexture2D(
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	TComPtr<ID3D11ShaderResourceView> &srv,
	const FString& alias = "");
void CreateSRVForTexture2DArray(DXGI_FORMAT format,
	UINT arraySize,
	TComPtr<ID3D11Texture2D> &tex2D,
	TComPtr<ID3D11ShaderResourceView> &srv,
	const FString& alias = "");
void									CreateSRVFromeFile(
	const FString& FileName,
	TComPtr<ID3D11ShaderResourceView> &srv,
	const FString& alias = "");

void CreateSRVForBuffer(DXGI_FORMAT format,
	UINT number,
	TComPtr<ID3D11Buffer> buffer,
	TComPtr<ID3D11ShaderResourceView> &srv,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//DRV
void CreateDSVForTexture2D(DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	TComPtr<ID3D11DepthStencilView> &dsv,
	const FString& alias = "");
void CreateDSVForTexture2DArray(DXGI_FORMAT format,
	UINT arraySize,
	TComPtr<ID3D11Texture2D> &tex2D,
	TComPtr<ID3D11DepthStencilView> &dsv,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//math
inline float							RandomVariance(float median, float variance)
{
	float fUnitRandom = (float)rand() / (float)RAND_MAX;
	return median + (2 * fUnitRandom - 1)*variance;
}
inline float							RandomFromTo(float lowest, float largest)
{
	float fUnitRandom = (float)rand() / (float)RAND_MAX;
	return (1.0f - fUnitRandom)*lowest + fUnitRandom*largest;
}
//////////////////////////////////////////////////////////////////////////
//sampler
void									CreateSamplerLinearWrap(
	TComPtr<ID3D11SamplerState> &sample,
	const FString& alias = "");

void									CreateSamplerLinearClamp(
	TComPtr<ID3D11SamplerState> &sample,
	const FString& alias = "");
void CreateSamplerComparision(TComPtr<ID3D11SamplerState> &sample,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Raster State
void									CreateRasterState(
	TComPtr<ID3D11RasterizerState> &RasterState,
	const FString& alias = "");
void									CreateRasterState(
	TComPtr<ID3D11RasterizerState> &RasterState,
	int DepthDias,
	float DepthBiasClamp,
	float SlopeScaledDepthBias,
	const FString& alias = "");

void CreateRasterStateNonCull(TComPtr<ID3D11RasterizerState> &RasterState,
	const FString& alias = "");

void CreateBlendState(TComPtr<ID3D11BlendState> &BlendState,
	bool Opaque = true,
	const FString& alias = "");
void CreateDepthStencileState(TComPtr<ID3D11DepthStencilState> &ds,
	bool Write = true,
	const FString& alias = "");

void CreateDepthStencileStateNoWriteNoTest(TComPtr<ID3D11DepthStencilState> &ds,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Render Target
void CreateRenderTarget(DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D>& texture,
	TComPtr<ID3D11RenderTargetView> &rtv,
	const FString& alias = "");


