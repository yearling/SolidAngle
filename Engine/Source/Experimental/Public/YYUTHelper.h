#pragma once
#include "YYUT.h"
#include <string>

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
void CreateComputerShader(TComPtr<ID3D11Device> device,
	const FString& FileName,
	const FString& MainPoint,
	TComPtr<ID3D11ComputeShader> & cs,
	const FString & alias = "");

void CreateVertexShader(TComPtr<ID3D11Device> &device,
	const FString& FileName,
	const FString& MainPoint,
	TComPtr<ID3D11VertexShader> &vs,
	const FString& alias /*= ""*/);
void CreatePixelShader(TComPtr<ID3D11Device> &device,
	const FString& FileName,
	const FString& MainPoint,
	TComPtr<ID3D11PixelShader> &ps,
	const FString& alias = "");
void                                    CreateGeometryShader(TComPtr<ID3D11Device> &device,
	const FString& FileName,
	const FString& MainPoint,
	TComPtr<ID3D11GeometryShader> &gs,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//CreateLayout
void CreateInputLayout(TComPtr<ID3D11Device> &device,
	const FString& FileName,
	const FString& VSMainPoint,
	const D3D11_INPUT_ELEMENT_DESC *pDesc,
	int number,
	TComPtr<ID3D11InputLayout> &InputLayout,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Create Buffer
void									CreateBufferSRV_UAV(TComPtr<ID3D11Device> &device,
	int ByteWidth, TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");
void									CreateStagingReadBackBuffer(TComPtr<ID3D11Device> &device,
	int ByteWidth,
	int StructureByteStride,
	TComPtr<ID3D11Buffer> & buffer,
	const FString& alias = "");

void									CreateConstantBufferCPUWrite(TComPtr<ID3D11Device> &device,
	int size,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");

void									CreateConstantBufferDefault(TComPtr<ID3D11Device> &device,
	int size,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");

void									CreateIndirectDrawArgsBuffer(TComPtr<ID3D11Device> &device,
	int ByteWidth,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");

void									CreateVertexBuffer(TComPtr<ID3D11Device> &device,
	UINT ByteWidth,
	const void *pData,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");
void									CreateVertexBufferDynamic(TComPtr<ID3D11Device> &device,
	UINT byteWidth,
	const void *pData,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");
void									CreateIndexBuffer(TComPtr<ID3D11Device> &device,
	UINT ByteWidth,
	const void * pData,
	TComPtr<ID3D11Buffer> &buffer,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Create Structured buffer
void									CreateStruturedBufferSRV_UAV(TComPtr<ID3D11Device> &device,
	int numbers,
	int perSize,
	TComPtr<ID3D11Buffer> & buffer,
	const FString& alias = "");

void									CreateStruturedBufferSRV(TComPtr<ID3D11Device> &device,
	int numbers,
	int perSize,
	TComPtr<ID3D11Buffer> & buffer,
	const FString& alias = "");

void									CreateStruturedBufferUAV(TComPtr<ID3D11Device> &device,
	int numbers,
	int perSize,
	TComPtr<ID3D11Buffer> & buffer,
	const FString& alias = "");

//////////////////////////////////////////////////////////////////////////
//Create 2D Texture
void									Create2DTextureImmuableSRV(TComPtr<ID3D11Device> &device,
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	D3D11_SUBRESOURCE_DATA *data,
	TComPtr<ID3D11Texture2D> &tex2D,
	const FString& alias = "");

void									Create2DTextureRTV_SRV(TComPtr<ID3D11Device> &device,
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	const FString& alias = "");
void									Create2DTextureDSV_SRV(TComPtr<ID3D11Device> &device,
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	const FString& alias = "");
void									Create2DTextureArrayDSV_SRV(TComPtr<ID3D11Device> &device,
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	UINT ArraySize,
	TComPtr<ID3D11Texture2D> &tex2D,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Create UAV
void									CreateAppendUAVForStructureBuffer(TComPtr<ID3D11Device> &device,
	int numbers,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	const FString& alias = "");

void									CreateUAVForStruturedBuffer(TComPtr<ID3D11Device> &device,
	int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	const FString& alias = "");
void									CreateCountUAVForStruturedBuffer(TComPtr<ID3D11Device> &device,
	int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	const FString& alias = "");
void									CreateUAVForBuffer(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Create SRV
void									CreateSRVForStruturedBuffer(TComPtr<ID3D11Device> &device,
	int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11ShaderResourceView> &srv,
	const FString& alias = "");

void									CreateSRVForTexture2D(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	TComPtr<ID3D11ShaderResourceView> &srv,
	const FString& alias = "");
void									CreateSRVForTexture2DArray(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	UINT arraySize,
	TComPtr<ID3D11Texture2D> &tex2D,
	TComPtr<ID3D11ShaderResourceView> &srv,
	const FString& alias = "");
void									CreateSRVFromeFile(TComPtr<ID3D11Device> &device,
	const FString& FileName,
	TComPtr<ID3D11ShaderResourceView> &srv,
	const FString& alias = "");

void									CreateSRVForBuffer(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	UINT number,
	TComPtr<ID3D11Buffer> buffer,
	TComPtr<ID3D11ShaderResourceView> &srv,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//DRV
void									CreateDSVForTexture2D(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	TComPtr<ID3D11DepthStencilView> &dsv,
	const FString& alias = "");
void									CreateDSVForTexture2DArray(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
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
void									CreateSamplerLinearWrap(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11SamplerState> &sample,
	const FString& alias = "");

void									CreateSamplerLinearClamp(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11SamplerState> &sample,
	const FString& alias = "");
void                                    CreateSamplerComparision(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11SamplerState> &sample,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Raster State
void									CreateRasterState(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11RasterizerState> &RasterState,
	const FString& alias = "");
void									CreateRasterState(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11RasterizerState> &RasterState,
	int DepthDias,
	float DepthBiasClamp,
	float SlopeScaledDepthBias,
	const FString& alias = "");

void									CreateRasterStateNonCull(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11RasterizerState> &RasterState,
	const FString& alias = "");

void									CreateBlendState(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11BlendState> &BlendState,
	bool Opaque = true,
	const FString& alias = "");
void									CreateDepthStencileState(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11DepthStencilState> &ds,
	bool Write = true,
	const FString& alias = "");

void									CreateDepthStencileStateNoWriteNoTest(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11DepthStencilState> &ds,
	const FString& alias = "");
//////////////////////////////////////////////////////////////////////////
//Render Target
void									CreateRenderTarget(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D>& texture,
	TComPtr<ID3D11RenderTargetView> &rtv,
	const FString& alias = "");


