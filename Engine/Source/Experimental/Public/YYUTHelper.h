#pragma once
#include "YYUT.h"
#include <string>

//D3DX11CompileShaderFromeFile的Asicall封装，ColomMajor是用来指定编译后的shader是行主还是列主，列主对矩阵乘法友好，manul建议用
void									ComplieShaderFromFile(std::string file_name,
	std::string entry_point,
	std::string shader_model,
	TComPtr<ID3DBlob> &blob,
	bool ColomMajor = true);
//用来指定对齐模式
inline int								Align(int value, int alignment)
{
	return (value + (alignment - 1))&~(alignment - 1);
}
//////////////////////////////////////////////////////////////////////////
//Create Shader
void									CreateComputerShader(TComPtr<ID3D11Device> device,
	std::string FileName,
	std::string MainPoint,
	TComPtr<ID3D11ComputeShader> & cs,
	std::string alias = "");

void									CreateVertexShader(TComPtr<ID3D11Device> &device,
	const std::string& FileName,
	const std::string& MainPoint,
	TComPtr<ID3D11VertexShader> &vs,
	const std::string& alias /*= ""*/);
void									CreatePixelShader(TComPtr<ID3D11Device> &device,
	std::string FileName,
	std::string MainPoint,
	TComPtr<ID3D11PixelShader> &ps,
	std::string alias = "");
void                                    CreateGeometryShader(TComPtr<ID3D11Device> &device,
	std::string FileName,
	std::string MainPoint,
	TComPtr<ID3D11GeometryShader> &gs,
	std::string alias = "");
//////////////////////////////////////////////////////////////////////////
//CreateLayout
void									CreateInputLayout(TComPtr<ID3D11Device> &device,
	std::string FileName,
	std::string VSMainPoint,
	const D3D11_INPUT_ELEMENT_DESC *pDesc,
	int number,
	TComPtr<ID3D11InputLayout> &InputLayout,
	std::string alias = "");
//////////////////////////////////////////////////////////////////////////
//Create Buffer
void									CreateBufferSRV_UAV(TComPtr<ID3D11Device> &device,
	int ByteWidth, TComPtr<ID3D11Buffer> &buffer,
	std::string alias = "");
void									CreateStagingReadBackBuffer(TComPtr<ID3D11Device> &device,
	int ByteWidth,
	int StructureByteStride,
	TComPtr<ID3D11Buffer> & buffer,
	std::string alias = "");

void									CreateConstantBufferCPUWrite(TComPtr<ID3D11Device> &device,
	int size,
	TComPtr<ID3D11Buffer> &buffer,
	std::string alias = "");

void									CreateConstantBufferDefault(TComPtr<ID3D11Device> &device,
	int size,
	TComPtr<ID3D11Buffer> &buffer,
	std::string alias = "");

void									CreateIndirectDrawArgsBuffer(TComPtr<ID3D11Device> &device,
	int ByteWidth,
	TComPtr<ID3D11Buffer> &buffer,
	std::string alias = "");

void									CreateVertexBuffer(TComPtr<ID3D11Device> &device,
	UINT ByteWidth,
	const void *pData,
	TComPtr<ID3D11Buffer> &buffer,
	std::string alias = "");
void									CreateVertexBufferDynamic(TComPtr<ID3D11Device> &device,
	UINT byteWidth,
	const void *pData,
	TComPtr<ID3D11Buffer> &buffer,
	std::string alias = "");
void									CreateIndexBuffer(TComPtr<ID3D11Device> &device,
	UINT ByteWidth,
	const void * pData,
	TComPtr<ID3D11Buffer> &buffer,
	std::string alias = "");
//////////////////////////////////////////////////////////////////////////
//Create Structured buffer
void									CreateStruturedBufferSRV_UAV(TComPtr<ID3D11Device> &device,
	int numbers,
	int perSize,
	TComPtr<ID3D11Buffer> & buffer,
	std::string alias = "");

void									CreateStruturedBufferSRV(TComPtr<ID3D11Device> &device,
	int numbers,
	int perSize,
	TComPtr<ID3D11Buffer> & buffer,
	std::string alias = "");

void									CreateStruturedBufferUAV(TComPtr<ID3D11Device> &device,
	int numbers,
	int perSize,
	TComPtr<ID3D11Buffer> & buffer,
	std::string alias = "");

//////////////////////////////////////////////////////////////////////////
//Create 2D Texture
void									Create2DTextureImmuableSRV(TComPtr<ID3D11Device> &device,
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	D3D11_SUBRESOURCE_DATA *data,
	TComPtr<ID3D11Texture2D> &tex2D,
	std::string alias = "");

void									Create2DTextureRTV_SRV(TComPtr<ID3D11Device> &device,
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	std::string alias = "");
void									Create2DTextureDSV_SRV(TComPtr<ID3D11Device> &device,
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	std::string alias = "");
void									Create2DTextureArrayDSV_SRV(TComPtr<ID3D11Device> &device,
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	UINT ArraySize,
	TComPtr<ID3D11Texture2D> &tex2D,
	std::string alias = "");
//////////////////////////////////////////////////////////////////////////
//Create UAV
void									CreateAppendUAVForStructureBuffer(TComPtr<ID3D11Device> &device,
	int numbers,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	std::string alias = "");

void									CreateUAVForStruturedBuffer(TComPtr<ID3D11Device> &device,
	int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	std::string alias = "");
void									CreateCountUAVForStruturedBuffer(TComPtr<ID3D11Device> &device,
	int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	std::string alias = "");
void									CreateUAVForBuffer(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11UnorderedAccessView> &uav,
	std::string alias = "");
//////////////////////////////////////////////////////////////////////////
//Create SRV
void									CreateSRVForStruturedBuffer(TComPtr<ID3D11Device> &device,
	int number,
	TComPtr<ID3D11Buffer> &buffer,
	TComPtr<ID3D11ShaderResourceView> &srv,
	std::string alias = "");

void									CreateSRVForTexture2D(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	TComPtr<ID3D11ShaderResourceView> &srv,
	std::string alias = "");
void									CreateSRVForTexture2DArray(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	UINT arraySize,
	TComPtr<ID3D11Texture2D> &tex2D,
	TComPtr<ID3D11ShaderResourceView> &srv,
	std::string alias = "");
void									CreateSRVFromeFile(TComPtr<ID3D11Device> &device,
	std::string FileName,
	TComPtr<ID3D11ShaderResourceView> &srv,
	std::string alias = "");

void									CreateSRVForBuffer(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	UINT number,
	TComPtr<ID3D11Buffer> buffer,
	TComPtr<ID3D11ShaderResourceView> &srv,
	std::string alias = "");
//////////////////////////////////////////////////////////////////////////
//DRV
void									CreateDSVForTexture2D(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D> &tex2D,
	TComPtr<ID3D11DepthStencilView> &dsv,
	std::string alias = "");
void									CreateDSVForTexture2DArray(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	UINT arraySize,
	TComPtr<ID3D11Texture2D> &tex2D,
	TComPtr<ID3D11DepthStencilView> &dsv,
	std::string alias = "");
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
	std::string alias = "");

void									CreateSamplerLinearClamp(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11SamplerState> &sample,
	std::string alias = "");
void                                    CreateSamplerComparision(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11SamplerState> &sample,
	std::string alias = "");
//////////////////////////////////////////////////////////////////////////
//Raster State
void									CreateRasterState(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11RasterizerState> &RasterState,
	std::string alias = "");
void									CreateRasterState(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11RasterizerState> &RasterState,
	int DepthDias,
	float DepthBiasClamp,
	float SlopeScaledDepthBias,
	std::string alias = "");

void									CreateRasterStateNonCull(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11RasterizerState> &RasterState,
	std::string alias = "");

void									CreateBlendState(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11BlendState> &BlendState,
	bool Opaque = true,
	std::string alias = "");
void									CreateDepthStencileState(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11DepthStencilState> &ds,
	bool Write = true,
	std::string alias = "");

void									CreateDepthStencileStateNoWriteNoTest(TComPtr<ID3D11Device> &device,
	TComPtr<ID3D11DepthStencilState> &ds,
	std::string alias = "");
//////////////////////////////////////////////////////////////////////////
//Render Target
void									CreateRenderTarget(TComPtr<ID3D11Device> &device,
	DXGI_FORMAT format,
	TComPtr<ID3D11Texture2D>& texture,
	TComPtr<ID3D11RenderTargetView> &rtv,
	std::string alias = "");


