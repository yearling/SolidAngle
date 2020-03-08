#include "YYUTHelper.h"
#include "WICTextureLoader.h"
#include <D3Dcompiler.h>
#include "DDSTextureLoader.h"
#include "YYUTDXManager.h"
#include "YRHIDefinitions.h"
DEFINE_LOG_CATEGORY(YYUTHELPERLog);

#define D3DERR(x) case x: ErrorCodeText = TEXT(#x); break;

static FString GetD3D11DeviceHungErrorString(HRESULT ErrorCode)
{
	FString ErrorCodeText;

	switch (ErrorCode)
	{
		D3DERR(DXGI_ERROR_DEVICE_HUNG)
			D3DERR(DXGI_ERROR_DEVICE_REMOVED)
			D3DERR(DXGI_ERROR_DEVICE_RESET)
			D3DERR(DXGI_ERROR_DRIVER_INTERNAL_ERROR)
			D3DERR(DXGI_ERROR_INVALID_CALL)
	default: ErrorCodeText = FString::Printf(TEXT("%08X"), (int32)ErrorCode);
	}

	return ErrorCodeText;
}


static FString GetD3D11ErrorString(HRESULT ErrorCode, ID3D11Device* Device)
{
	FString ErrorCodeText;

	switch (ErrorCode)
	{
		D3DERR(S_OK);
		D3DERR(D3D11_ERROR_FILE_NOT_FOUND)
			D3DERR(D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS)
#if WITH_D3DX_LIBS
			D3DERR(D3DERR_INVALIDCALL)
			D3DERR(D3DERR_WASSTILLDRAWING)
#endif	//WITH_D3DX_LIBS
			D3DERR(E_FAIL)
			D3DERR(E_INVALIDARG)
			D3DERR(E_OUTOFMEMORY)
			D3DERR(DXGI_ERROR_INVALID_CALL)
			D3DERR(E_NOINTERFACE)
			D3DERR(DXGI_ERROR_DEVICE_REMOVED)
	default: ErrorCodeText = FString::Printf(TEXT("%08X"), (int32)ErrorCode);
	}

	if (ErrorCode == DXGI_ERROR_DEVICE_REMOVED && Device)
	{
		HRESULT hResDeviceRemoved = Device->GetDeviceRemovedReason();
		ErrorCodeText += FString(TEXT(" ")) + GetD3D11DeviceHungErrorString(hResDeviceRemoved);
	}

	return ErrorCodeText;
}

void VerifyD3D11Result(HRESULT D3DResult, const ANSICHAR* Code, const ANSICHAR* Filename, uint32 Line, ID3D11Device* Device)
{
	check(FAILED(D3DResult));

	const FString& ErrorString = GetD3D11ErrorString(D3DResult, Device);

	UE_LOG(YYUTHELPERLog, Error, TEXT("%s failed \n at %s:%u \n with error %s"), ANSI_TO_TCHAR(Code), ANSI_TO_TCHAR(Filename), Line, *ErrorString);

	///*TerminateOnDeviceRemoved*/(D3DResult, Device);
	//TerminateOnOutOfMemory(D3DResult, false);

	UE_LOG(YYUTHELPERLog, Fatal, TEXT("%s failed \n at %s:%u \n with error %s"), ANSI_TO_TCHAR(Code), ANSI_TO_TCHAR(Filename), Line, *ErrorString);
}

#define VERIFYD3D11RESULT_EX(x, Device)	{HRESULT hr = x; if (FAILED(hr)) { VerifyD3D11Result(hr,#x,__FILE__,__LINE__, Device); }}
void ComplieShaderFromFile(const FString &file_name, const FString& entry_point, const FString& shader_model, TComPtr<ID3DBlob> &blob, bool ColomMajor /*= true*/)
{
	HRESULT hr = S_OK;
	DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
	if (!ColomMajor)
		shader_flags |= D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
#if defined( DEBUG ) ||  defined( _DEBUG)
	shader_flags |= D3DCOMPILE_DEBUG;
	shader_flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	TComPtr<ID3DBlob> err_bob;
	if (FAILED(hr = D3DCompileFromFile(*file_name,
		NULL,
		NULL,
		TCHAR_TO_ANSI(*entry_point),
		TCHAR_TO_ANSI(*shader_model),
		shader_flags,
		0,
		&blob,
		&err_bob)))
	{
		if (err_bob)
		{
			OutputDebugStringA((char*)err_bob->GetBufferPointer());
		}
	}
}

void CreateComputerShader(const FString& FileName, const FString& MainPoint, TComPtr<ID3D11ComputeShader> & cs, const FString & alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	TComPtr<ID3DBlob> vs_blob;
	ComplieShaderFromFile(FileName, MainPoint, "cs_5_0", vs_blob);
	if (FAILED(hr = device->CreateComputeShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), NULL, &cs)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(cs, alias);
#endif
}


void CreateVertexShader(const FString& FileName, const FString& MainPoint, TComPtr<ID3D11VertexShader> &vs, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	TComPtr<ID3DBlob> vs_blob;
	ComplieShaderFromFile(FileName, MainPoint, "vs_5_0", vs_blob);
	if (FAILED(hr = device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), NULL, &vs)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(vs, alias);
#endif
}

void CreatePixelShader(const FString& FileName, const FString& MainPoint, TComPtr<ID3D11PixelShader> &ps, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	TComPtr<ID3DBlob> vs_blob;
	ComplieShaderFromFile(FileName, MainPoint, "ps_5_0", vs_blob);
	if (FAILED(hr = device->CreatePixelShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), NULL, &ps)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(ps, alias);
#endif
}
void CreateInputLayout(const FString& FileName, const FString& VSMainPoint, const D3D11_INPUT_ELEMENT_DESC *pDesc, int number, TComPtr<ID3D11InputLayout> &InputLayout, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	TComPtr<ID3DBlob> vs_blob;
	ComplieShaderFromFile(FileName, VSMainPoint, "vs_5_0", vs_blob);
	if (FAILED(hr = device->CreateInputLayout(pDesc, number, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &InputLayout)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(InputLayout, alias);
#endif
}
void CreateStruturedBufferSRV_UAV(int numbers, int perSize, TComPtr<ID3D11Buffer> & buffer, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = numbers*perSize;
	desc.StructureByteStride = perSize;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	if (FAILED(hr = device->CreateBuffer(&desc, NULL, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}




void CreateStruturedBufferUAV(int numbers, int perSize, TComPtr<ID3D11Buffer> & buffer, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = numbers*perSize;
	desc.StructureByteStride = perSize;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	if (FAILED(hr = device->CreateBuffer(&desc, NULL, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}

void CreateAppendUAVForStructureBuffer(int numbers, TComPtr<ID3D11Buffer> &buffer, TComPtr<ID3D11UnorderedAccessView> &uav, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = numbers;
	desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	if (FAILED(hr = device->CreateUnorderedAccessView(buffer, &desc, &uav)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(uav, alias);
#endif
}

void CreateStagingReadBackBuffer(int ByteWidth, int StructureByteStride, TComPtr<ID3D11Buffer> & buffer, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = ByteWidth;
	desc.StructureByteStride = StructureByteStride;
	desc.BindFlags = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	if (FAILED(hr = device->CreateBuffer(&desc, NULL, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}

void CreateSRVForStruturedBuffer(int number, TComPtr<ID3D11Buffer> &buffer, TComPtr<ID3D11ShaderResourceView> &srv, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = number;
	if (FAILED(hr = device->CreateShaderResourceView(buffer, &desc, &srv)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(srv, alias);
#endif
}

void CreateSRVForTBuffer(DXGI_FORMAT format, int32 number, TComPtr<ID3D11Buffer> &buffer, TComPtr<ID3D11ShaderResourceView> &srv, const FString& alias /*= ""*/)
{	
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = number;
	if (FAILED(hr = device->CreateShaderResourceView(buffer, &desc, &srv)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(srv, alias);
#endif


}

void CreateUAVForStruturedBuffer(int number, TComPtr<ID3D11Buffer> &buffer, TComPtr<ID3D11UnorderedAccessView> &uav, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = number;
	desc.Buffer.Flags = 0;
	if (FAILED(hr = device->CreateUnorderedAccessView(buffer, &desc, &uav)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(uav, alias);
#endif
}

void CreateConstantBufferCPUWrite(int size, TComPtr<ID3D11Buffer> &buffer, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED(hr = device->CreateBuffer(&desc, NULL, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}

void CreateConstantBufferDefault(int size, TComPtr<ID3D11Buffer> &buffer, const FString& alias /*= ""*/)
{

	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	if (FAILED(hr = device->CreateBuffer(&desc, NULL, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}

void Create2DTextureImmuableSRV(UINT width, UINT height, DXGI_FORMAT format, D3D11_SUBRESOURCE_DATA *data, TComPtr<ID3D11Texture2D> &tex2D, const FString& alias /*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	if (FAILED(hr = device->CreateTexture2D(&desc, data, &tex2D)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(tex2D, alias);
#endif
}


void Create2DTextureRTV_SRV(UINT width, UINT height, DXGI_FORMAT format, TComPtr<ID3D11Texture2D> &tex2D, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	if (FAILED(hr = device->CreateTexture2D(&desc, NULL, &tex2D)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(tex2D, alias);
#endif
}

void CreateSRVForTexture2D(DXGI_FORMAT format, TComPtr<ID3D11Texture2D> &tex2D, TComPtr<ID3D11ShaderResourceView> &srv, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;
	desc.Texture2D.MostDetailedMip = 0;
	if (FAILED(hr = device->CreateShaderResourceView(tex2D, &desc, &srv)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(srv, alias);
#endif
}

void CreateCountUAVForStruturedBuffer(int number, TComPtr<ID3D11Buffer> &buffer, TComPtr<ID3D11UnorderedAccessView> &uav, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = number;
	desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	if (FAILED(hr = device->CreateUnorderedAccessView(buffer, &desc, &uav)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(uav, alias);
#endif
}

void CreateIndirectDrawArgsBuffer(int ByteWidth, TComPtr<ID3D11Buffer> &buffer, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = ByteWidth;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	if (FAILED(hr = device->CreateBuffer(&desc, NULL, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}

void CreateUAVForBuffer(DXGI_FORMAT format, int number, TComPtr<ID3D11Buffer> &buffer, TComPtr<ID3D11UnorderedAccessView> &uav, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = format;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = number;
	desc.Buffer.Flags = 0;
	if (FAILED(hr = device->CreateUnorderedAccessView(buffer, &desc, &uav)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(uav, alias);
#endif
}
void CreateVertexBufferStatic(UINT ByteWidth, const void *pData, TComPtr<ID3D11Buffer> &buffer, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = ByteWidth;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA sub;
	memset(&sub, 0, sizeof(sub));
	sub.pSysMem = pData;
	if (FAILED(hr = device->CreateBuffer(&desc, &sub, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}

void CreateVertexBufferStatic(YRHIResourceCreateInfo & RHIResourceInfo, TRefCountPtr<ID3D11Buffer> &buffer, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = RHIResourceInfo.ResourceArray->GetResourceDataSize();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA sub;
	memset(&sub, 0, sizeof(sub));
	sub.pSysMem = RHIResourceInfo.ResourceArray->GetResourceData();
	ID3D11Buffer* D3DBuffer = nullptr;
	if (FAILED(hr = device->CreateBuffer(&desc, &sub,&D3DBuffer)))
	{

	}
	else
	{
		buffer = TRefCountPtr<ID3D11Buffer>(D3DBuffer, false);
	}
	if (!RHIResourceInfo.ResourceArray->GetAllowCPUAccess())
	{
		RHIResourceInfo.ResourceArray->Discard();
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}
TRefCountPtr<YRHIVertexBuffer> CreateVertexBuffer(uint32 Size, uint32 InUsage, YRHIResourceCreateInfo& CreateInfo)
{
	check(Size);
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(D3D11_BUFFER_DESC));
	Desc.ByteWidth = Size;
	Desc.Usage = (InUsage & YBUF_AnyDynamic) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = (InUsage & YBUF_AnyDynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	if (InUsage & YBUF_UnorderedAccess)
	{
		Desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

	/*	static bool bRequiresRawView = (GMaxRHIFeatureLevel < ERHIFeatureLevel::SM5);
		if (bRequiresRawView)
		{
			Desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		}*/
	}

	if (InUsage & YBUF_ByteAddressBuffer)
	{
		Desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	}

	if (InUsage & YBUF_StreamOutput)
	{
		Desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
	}

	if (InUsage & YBUF_DrawIndirect)
	{
		Desc.MiscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	}

	if (InUsage & YBUF_ShaderResource)
	{
		Desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}
	// If a resource array was provided for the resource, create the resource pre-populated
	D3D11_SUBRESOURCE_DATA InitData;
	D3D11_SUBRESOURCE_DATA* pInitData = NULL;
	if (CreateInfo.ResourceArray)
	{
		check(Size == CreateInfo.ResourceArray->GetResourceDataSize());
		InitData.pSysMem = CreateInfo.ResourceArray->GetResourceData();
		InitData.SysMemPitch = Size;
		InitData.SysMemSlicePitch = 0;
		pInitData = &InitData;
	}

	TRefCountPtr<ID3D11Buffer> VertexBufferResource;
	VERIFYD3D11RESULT_EX(device->CreateBuffer(&Desc, pInitData, VertexBufferResource.GetInitReference()), device);

	//UpdateBufferStats(VertexBufferResource, true);

	if (CreateInfo.ResourceArray)
	{
		// Discard the resource array's contents.
		CreateInfo.ResourceArray->Discard();
	}
	return new YRHIVertexBuffer(VertexBufferResource, Size, InUsage);
}


void CreateVertexBufferDynamic(UINT ByteWidth, const void *pData, TComPtr<ID3D11Buffer> &buffer, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = ByteWidth;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	if (pData == NULL)
	{
		if (FAILED(hr = device->CreateBuffer(&desc, NULL, &buffer)))
		{
		}
	}
	else
	{
		D3D11_SUBRESOURCE_DATA sub;
		memset(&sub, 0, sizeof(sub));
		sub.pSysMem = pData;
		if (FAILED(hr = device->CreateBuffer(&desc, &sub, &buffer)))
		{
		}
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}

void CreateIndexBuffer(UINT ByteWidth, const void * pData, TComPtr<ID3D11Buffer> &buffer, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = ByteWidth;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA sub;
	memset(&sub, 0, sizeof(sub));
	sub.pSysMem = pData;
	if (FAILED(hr = device->CreateBuffer(&desc, &sub, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}

TRefCountPtr<YRHIIndexBuffer> CreateIndexBuffer(uint32 InStride, uint32 InSize, uint32 InUsage, YRHIResourceCreateInfo& CreateInfo)
{
	// Describe the index buffer.
	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(D3D11_BUFFER_DESC));
	Desc.ByteWidth = InSize;
	Desc.Usage = (InUsage & YBUF_AnyDynamic) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Desc.CPUAccessFlags = (InUsage & YBUF_AnyDynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
	Desc.MiscFlags = 0;

	if (InUsage & YBUF_UnorderedAccess)
	{
		Desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}

	if (InUsage & YBUF_DrawIndirect)
	{
		Desc.MiscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	}

	if (InUsage & YBUF_ShaderResource)
	{
		Desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	// If a resource array was provided for the resource, create the resource pre-populated
	D3D11_SUBRESOURCE_DATA InitData;
	D3D11_SUBRESOURCE_DATA* pInitData = NULL;
	if (CreateInfo.ResourceArray)
	{
		check(InSize == CreateInfo.ResourceArray->GetResourceDataSize());
		InitData.pSysMem = CreateInfo.ResourceArray->GetResourceData();
		InitData.SysMemPitch = InSize;
		InitData.SysMemSlicePitch = 0;
		pInitData = &InitData;
	}

	TRefCountPtr<ID3D11Buffer> IndexBufferResource;
	TComPtr<ID3D11Device> Direct3DDevice = YYUTDXManager::GetInstance().GetD3DDevice();
	VERIFYD3D11RESULT_EX(Direct3DDevice->CreateBuffer(&Desc, pInitData, IndexBufferResource.GetInitReference()), Direct3DDevice);

	//UpdateBufferStats(IndexBufferResource, true);

	if (CreateInfo.ResourceArray)
	{
		// Discard the resource array's contents.
		CreateInfo.ResourceArray->Discard();
	}

	return new YRHIIndexBuffer(IndexBufferResource, InStride, InSize, InUsage);
}

void CreateSamplerLinearWrap(TComPtr<ID3D11SamplerState> &sample, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if (FAILED(hr = device->CreateSamplerState(&samDesc, &sample)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(sample, alias);
#endif
}


void CreateSamplerLinearClamp(TComPtr<ID3D11SamplerState> &sample, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if (FAILED(hr = device->CreateSamplerState(&samDesc, &sample)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(sample, alias);
#endif
}

void CreateSRVFromeFile(const FString& FileName, TComPtr<ID3D11ShaderResourceView> &srv, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	if (FAILED(hr = CreateDDSTextureFromFile(device, *FileName, NULL, &srv)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(srv, alias);
#endif
}

void CreateRasterState(TComPtr<ID3D11RasterizerState> &RasterState, const FString& alias/*=""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_RASTERIZER_DESC desc;
	desc.FillMode = D3D11_FILL_SOLID;

	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = TRUE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;
	if (FAILED(hr = device->CreateRasterizerState(&desc, &RasterState)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(RasterState, alias);
#endif
}

void CreateRasterState(TComPtr<ID3D11RasterizerState> &RasterState, int DepthDias, float DepthBiasClamp, float SlopeScaledDepthBias, const FString& alias/*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_RASTERIZER_DESC desc;
	desc.FillMode = D3D11_FILL_SOLID;

	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = DepthDias;
	desc.DepthBiasClamp = DepthBiasClamp;
	desc.SlopeScaledDepthBias = SlopeScaledDepthBias;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;
	if (FAILED(hr = device->CreateRasterizerState(&desc, &RasterState)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(RasterState, alias);
#endif
}

void CreateRasterStateNonCull(TComPtr<ID3D11RasterizerState> &RasterState, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_RASTERIZER_DESC desc;
	desc.FillMode = D3D11_FILL_SOLID;

	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;
	if (FAILED(hr = device->CreateRasterizerState(&desc, &RasterState)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(RasterState, alias);
#endif
}

void CreateBlendState(TComPtr<ID3D11BlendState> &BlendState, bool Opaque /*= true*/, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BLEND_DESC desc;
	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;
	desc.RenderTarget[0].BlendEnable = (Opaque) ? FALSE : TRUE;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	if (FAILED(hr = device->CreateBlendState(&desc, &BlendState)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(BlendState, alias);
#endif
}

void CreateDepthStencileState(TComPtr<ID3D11DepthStencilState> &ds, bool Write /*= true*/, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = TRUE;
	desc.DepthWriteMask = Write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.StencilEnable = FALSE;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	if (FAILED(hr = device->CreateDepthStencilState(&desc, &ds)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(ds, alias);
#endif
}

void CreateDepthStencileStateNoWriteNoTest(TComPtr<ID3D11DepthStencilState> &ds, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = FALSE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.StencilEnable = FALSE;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	if (FAILED(hr = device->CreateDepthStencilState(&desc, &ds)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(ds, alias);
#endif
}

void CreateRenderTarget(DXGI_FORMAT format, TComPtr<ID3D11Texture2D>& texture, TComPtr<ID3D11RenderTargetView> &rtv, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	desc.Format = format;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;
	if (FAILED(hr = device->CreateRenderTargetView(texture, &desc, &rtv)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(rtv, alias);
#endif
}

void CreateBufferSRV_UAV(int ByteWidth, TComPtr<ID3D11Buffer> &buffer, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.ByteWidth = ByteWidth;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	if (FAILED(hr = device->CreateBuffer(&desc, NULL, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}

void CreateSRVForBuffer(DXGI_FORMAT format, UINT number, TComPtr<ID3D11Buffer> buffer, TComPtr<ID3D11ShaderResourceView> &srv, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = number;
	if (FAILED(hr = device->CreateShaderResourceView(buffer, &desc, &srv)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(srv, alias);
#endif
}

void Create2DTextureDSV_SRV(UINT width, UINT height, DXGI_FORMAT format, TComPtr<ID3D11Texture2D> &tex2D, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	if (FAILED(hr = device->CreateTexture2D(&desc, NULL, &tex2D)))
	{
	}

#if defined DEBUG | defined _DEBUG
	AddAlias(tex2D, alias);
#endif
}

void CreateDSVForTexture2D(DXGI_FORMAT format, TComPtr<ID3D11Texture2D> &tex2D, TComPtr<ID3D11DepthStencilView> &dsv, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = format;
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;
	desc.Flags = 0;
	if (FAILED(hr = device->CreateDepthStencilView(tex2D, &desc, &dsv)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(dsv, alias);
#endif
}

void Create2DTextureArrayDSV_SRV(UINT width, UINT height, DXGI_FORMAT format, UINT ArraySize, TComPtr<ID3D11Texture2D> &tex2D, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = ArraySize;
	desc.Format = format;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	if (FAILED(hr = device->CreateTexture2D(&desc, NULL, &tex2D)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(tex2D, alias);
#endif 
}

void CreateSamplerComparision(TComPtr<ID3D11SamplerState> &sample, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_SAMPLER_DESC samDesc =
	{
		D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,// D3D11_FILTER Filter;
		D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressU;
		D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressV;
		D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressW;
		0,//FLOAT MipLODBias;
		0,//UINT MaxAnisotropy;
		D3D11_COMPARISON_LESS , //D3D11_COMPARISON_FUNC ComparisonFunc;
		0.0,0.0,0.0,0.0,//FLOAT BorderColor[ 4 ];
		0,//FLOAT MinLOD;
		0//FLOAT MaxLOD;   
	};
	if (FAILED(hr = device->CreateSamplerState(&samDesc, &sample)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(sample, alias);
#endif
}

void CreateSRVForTexture2DArray(DXGI_FORMAT format, UINT arraySize, TComPtr<ID3D11Texture2D> &tex2D, TComPtr<ID3D11ShaderResourceView> &srv, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.MostDetailedMip = 0;
	desc.Texture2DArray.MipLevels = 1;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.ArraySize = arraySize;
	desc.Texture2D.MostDetailedMip = 0;
	if (FAILED(hr = device->CreateShaderResourceView(tex2D, &desc, &srv)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(srv, alias);
#endif
}

void CreateDSVForTexture2DArray(DXGI_FORMAT format, UINT arraySize, TComPtr<ID3D11Texture2D> &tex2D, TComPtr<ID3D11DepthStencilView> &dsv, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = format;
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.ArraySize = arraySize;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.MipSlice = 0;
	desc.Flags = 0;
	if (FAILED(hr = device->CreateDepthStencilView(tex2D, &desc, &dsv)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(dsv, alias);
#endif
}

void CreateGeometryShader(const FString& FileName, const FString& MainPoint, TComPtr<ID3D11GeometryShader> &gs, const FString& alias /*= ""*/)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	TComPtr<ID3DBlob> vs_blob;
	ComplieShaderFromFile(FileName, MainPoint, "gs_5_0", vs_blob);
	if (FAILED(hr = device->CreateGeometryShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), NULL, &gs)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(gs, alias);
#endif
}


template<>
void CreateStruturedBuffer<true, false, true, false>(int numbers, int perSize, TComPtr<ID3D11Buffer> & buffer, const FString& alias)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = numbers*perSize;
	desc.StructureByteStride = perSize;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	if (FAILED(hr = device->CreateBuffer(&desc, NULL, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
};

template<>
void CreateStruturedBuffer<false, true, true, false>(int numbers, int perSize, TComPtr<ID3D11Buffer> & buffer, const FString& alias)
{
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = numbers*perSize;
	desc.StructureByteStride = perSize;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	if (FAILED(hr = device->CreateBuffer(&desc, NULL, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
};

template<>
void CreateTBuffer<true,false,true,false>(int32 ByteCounts, TComPtr<ID3D11Buffer> & buffer, const FString& alias)
{	
	TComPtr<ID3D11Device> device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = ByteCounts;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	if (FAILED(hr = device->CreateBuffer(&desc, NULL, &buffer)))
	{
	}
#if defined DEBUG | defined _DEBUG
	AddAlias(buffer, alias);
#endif
}