#include "YYUTHelper.h"
#include "WICTextureLoader.h"
#include <D3Dcompiler.h>
#include "DDSTextureLoader.h"


	void ComplieShaderFromFile(std::string file_name, std::string entry_point, std::string shader_model, TComPtr<ID3DBlob> &blob, bool ColomMajor/*=true*/)
	{
		HRESULT hr=S_OK;
		DWORD shader_flags=D3DCOMPILE_ENABLE_STRICTNESS;
		if(!ColomMajor)
			shader_flags|=D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
#if defined( DEBUG ) ||  defined( _DEBUG)
		shader_flags|=D3DCOMPILE_DEBUG;
		shader_flags|=D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
		TComPtr<ID3DBlob> err_bob;
		std::wstring wFileName(file_name.begin(),file_name.end());
		if(FAILED(hr=D3DCompileFromFile(wFileName.c_str(),
											NULL,
											NULL,
											entry_point.c_str(),
											shader_model.c_str(),
											shader_flags,
											0,
											&blob,
											&err_bob)))
		{
			if(err_bob)
			{
				std::string compile_err((char*)err_bob->GetBufferPointer());
				OutputDebugStringA(compile_err.c_str());
			}
		}
	}
	template<typename T>
	inline void AddAlias(T & buffer,const std::string& alias)
	{
		HRESULT hr=S_OK;
		if(!alias.empty())
		{
			char *p=new char[alias.length()];
			memcpy_s(p,alias.length(),alias.c_str(),alias.length());
			if(FAILED(hr = buffer->SetPrivateData( WKPDID_D3DDebugObjectName,(UINT) alias.length(), p)))
			{
			}
		}	
	}
	void CreateComputerShader(TComPtr<ID3D11Device> device, std::string FileName,std::string MainPoint,TComPtr<ID3D11ComputeShader> & cs, std::string alias)
	{
		HRESULT hr=S_OK;
		TComPtr<ID3DBlob> vs_blob;
		ComplieShaderFromFile(FileName,MainPoint,"cs_5_0",vs_blob);
		if(FAILED(hr=device->CreateComputeShader(vs_blob->GetBufferPointer(),vs_blob->GetBufferSize(),NULL,&cs)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(cs,alias);	
#endif
	}


	void CreateVertexShader(TComPtr<ID3D11Device> &device, const std::string& FileName, const std::string& MainPoint, TComPtr<ID3D11VertexShader> &vs, const std::string& alias /*= ""*/)
	{
		HRESULT hr=S_OK;
		TComPtr<ID3DBlob> vs_blob;
		ComplieShaderFromFile(FileName,MainPoint,"vs_5_0",vs_blob);
		if(FAILED(hr=device->CreateVertexShader(vs_blob->GetBufferPointer(),vs_blob->GetBufferSize(),NULL,&vs)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(vs,alias);	
#endif
	}

	void CreatePixelShader(TComPtr<ID3D11Device> &device, std::string FileName, std::string MainPoint, TComPtr<ID3D11PixelShader> &ps, std::string alias/*= ""*/)
	{
		HRESULT hr=S_OK;
		TComPtr<ID3DBlob> vs_blob;
		ComplieShaderFromFile(FileName,MainPoint,"ps_5_0",vs_blob);
		if(FAILED(hr=device->CreatePixelShader(vs_blob->GetBufferPointer(),vs_blob->GetBufferSize(),NULL,&ps)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(ps,alias);	
#endif
	}
	void CreateInputLayout(TComPtr<ID3D11Device> &device, std::string FileName, std::string VSMainPoint, const D3D11_INPUT_ELEMENT_DESC *pDesc, int number, TComPtr<ID3D11InputLayout> &InputLayout, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		TComPtr<ID3DBlob> vs_blob;
		ComplieShaderFromFile(FileName,VSMainPoint,"vs_5_0",vs_blob);	
		if(FAILED(hr=device->CreateInputLayout(pDesc,number,vs_blob->GetBufferPointer(),vs_blob->GetBufferSize(),&InputLayout)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(InputLayout,alias);	
#endif
	}
	void CreateStruturedBufferSRV_UAV(TComPtr<ID3D11Device> &device, int numbers,int perSize,TComPtr<ID3D11Buffer> & buffer,std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_BUFFER_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.ByteWidth=numbers*perSize;
		desc.StructureByteStride=perSize;
		desc.BindFlags=D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage=D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags=0;
		desc.MiscFlags=D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		if(FAILED(hr=device->CreateBuffer(&desc,NULL,&buffer)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(buffer,alias);
#endif
	}


	void CreateStruturedBufferSRV(TComPtr<ID3D11Device> &device, int numbers, int perSize, TComPtr<ID3D11Buffer> & buffer, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_BUFFER_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.ByteWidth=numbers*perSize;
		desc.StructureByteStride=perSize;
		desc.BindFlags=D3D11_BIND_SHADER_RESOURCE ;
		desc.Usage=D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags=0;
		desc.MiscFlags=D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		if(FAILED(hr=device->CreateBuffer(&desc,NULL,&buffer)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(buffer,alias);
#endif
	}

	void CreateStruturedBufferUAV(TComPtr<ID3D11Device> &device, int numbers, int perSize, TComPtr<ID3D11Buffer> & buffer, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_BUFFER_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.ByteWidth=numbers*perSize;
		desc.StructureByteStride=perSize;
		desc.BindFlags=D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage=D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags=0;
		desc.MiscFlags=D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		if(FAILED(hr=device->CreateBuffer(&desc,NULL,&buffer)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(buffer,alias);
#endif
	}

	void CreateAppendUAVForStructureBuffer(TComPtr<ID3D11Device> &device, int numbers,TComPtr<ID3D11Buffer> &buffer, TComPtr<ID3D11UnorderedAccessView> &uav, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Format=DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension=D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement=0;
		desc.Buffer.NumElements=numbers;
		desc.Buffer.Flags=D3D11_BUFFER_UAV_FLAG_APPEND;
		if(FAILED(hr=device->CreateUnorderedAccessView(buffer,&desc,&uav)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(uav,alias);
#endif
	}

	void CreateStagingReadBackBuffer(TComPtr<ID3D11Device> &device, int ByteWidth,int StructureByteStride,TComPtr<ID3D11Buffer> & buffer,std::string alias)
	{
		HRESULT hr=S_OK;
		D3D11_BUFFER_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.ByteWidth=ByteWidth;
		desc.StructureByteStride=StructureByteStride;
		desc.BindFlags=0;
		desc.Usage=D3D11_USAGE_STAGING;
		desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
		if(FAILED(hr=device->CreateBuffer(&desc,NULL,&buffer)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(buffer,alias);
#endif
	}

	void CreateSRVForStruturedBuffer(TComPtr<ID3D11Device> &device, int number, TComPtr<ID3D11Buffer> &buffer, TComPtr<ID3D11ShaderResourceView> &srv, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Format=DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension=D3D11_SRV_DIMENSION_BUFFER;
		desc.Buffer.ElementOffset=0;
		desc.Buffer.ElementWidth=number;
		if(FAILED(hr=device->CreateShaderResourceView(buffer,&desc,&srv)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(srv,alias);
#endif
	}

	void CreateUAVForStruturedBuffer(TComPtr<ID3D11Device> &device, int number, TComPtr<ID3D11Buffer> &buffer, TComPtr<ID3D11UnorderedAccessView> &srv, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Format=DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension=D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement=0;
		desc.Buffer.NumElements=number;
		desc.Buffer.Flags=0;
		if(FAILED(hr=device->CreateUnorderedAccessView(buffer,&desc,&srv)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(srv,alias);
#endif
	}

	void CreateConstantBufferCPUWrite(TComPtr<ID3D11Device> &device, int size, TComPtr<ID3D11Buffer> &buffer, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_BUFFER_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.ByteWidth=size;
		desc.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage=D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
		if(FAILED(hr=device->CreateBuffer(&desc,NULL,&buffer)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(buffer,alias);
#endif
	}

	void CreateConstantBufferDefault(TComPtr<ID3D11Device> &device, int size, TComPtr<ID3D11Buffer> &buffer, std::string alias/*=""*/)
	{
	
		HRESULT hr=S_OK;
		D3D11_BUFFER_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.ByteWidth=size;
		desc.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage=D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags=0;
		if(FAILED(hr=device->CreateBuffer(&desc,NULL,&buffer)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(buffer,alias);
#endif
	}

	void Create2DTextureImmuableSRV(TComPtr<ID3D11Device> &device, UINT width, UINT height, DXGI_FORMAT format, D3D11_SUBRESOURCE_DATA *data, TComPtr<ID3D11Texture2D> &tex2D, std::string alias /*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.Width = width;
		desc.Height = height;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		if(FAILED(hr=device->CreateTexture2D(&desc,data,&tex2D)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(tex2D,alias);
#endif
	}


	void Create2DTextureRTV_SRV(TComPtr<ID3D11Device> &device, UINT width, UINT height, DXGI_FORMAT format, TComPtr<ID3D11Texture2D> &tex2D, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.Width = width;
		desc.Height = height;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		if(FAILED(hr=device->CreateTexture2D(&desc,NULL,&tex2D)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(tex2D,alias);
#endif
	}

	void CreateSRVForTexture2D(TComPtr<ID3D11Device> &device, DXGI_FORMAT format,TComPtr<ID3D11Texture2D> &tex2D, TComPtr<ID3D11ShaderResourceView> &srv, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Format=format;
		desc.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels=1;
		desc.Texture2D.MostDetailedMip=0;
		if(FAILED(hr=device->CreateShaderResourceView(tex2D,&desc,&srv)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(srv,alias);
#endif
	}

	void CreateCountUAVForStruturedBuffer(TComPtr<ID3D11Device> &device, int number, TComPtr<ID3D11Buffer> &buffer, TComPtr<ID3D11UnorderedAccessView> &uav, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Format=DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension=D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement=0;
		desc.Buffer.NumElements=number;
		desc.Buffer.Flags=D3D11_BUFFER_UAV_FLAG_COUNTER;
		if(FAILED(hr=device->CreateUnorderedAccessView(buffer,&desc,&uav)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(uav,alias);
#endif
	}

	void CreateIndirectDrawArgsBuffer(TComPtr<ID3D11Device> &device, int ByteWidth, TComPtr<ID3D11Buffer> &buffer, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_BUFFER_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.ByteWidth=ByteWidth;
		desc.BindFlags=D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage=D3D11_USAGE_DEFAULT;
		desc.MiscFlags=D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		if(FAILED(hr=device->CreateBuffer(&desc,NULL,&buffer)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(buffer,alias);
#endif
	}

	void CreateUAVForBuffer(TComPtr<ID3D11Device> &device, DXGI_FORMAT format, int number, TComPtr<ID3D11Buffer> &buffer, TComPtr<ID3D11UnorderedAccessView> &uav, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Format=format;
		desc.ViewDimension=D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement=0;
		desc.Buffer.NumElements=number;
		desc.Buffer.Flags=0;
		if(FAILED(hr=device->CreateUnorderedAccessView(buffer,&desc,&uav)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(uav,alias);
#endif
	}
	void CreateVertexBuffer(TComPtr<ID3D11Device> &device, UINT ByteWidth, const void *pData, TComPtr<ID3D11Buffer> &buffer, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_BUFFER_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Usage=D3D11_USAGE_DEFAULT;
		desc.ByteWidth=ByteWidth;
		desc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags=0;
		D3D11_SUBRESOURCE_DATA sub;
		memset(&sub,0,sizeof(sub));
		sub.pSysMem=pData;
		if(FAILED(hr=device->CreateBuffer(&desc,&sub,&buffer)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(buffer,alias);
#endif
	}

	void CreateVertexBufferDynamic(TComPtr<ID3D11Device> &device, UINT ByteWidth, const void *pData, TComPtr<ID3D11Buffer> &buffer, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_BUFFER_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Usage=D3D11_USAGE_DYNAMIC;
		desc.ByteWidth=ByteWidth;
		desc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags=0;
		if(pData==NULL)
		{
			if(FAILED(hr=device->CreateBuffer(&desc,NULL,&buffer)))
			{
			}
		}
		else
		{
			D3D11_SUBRESOURCE_DATA sub;
			memset(&sub,0,sizeof(sub));
			sub.pSysMem=pData;
			if(FAILED(hr=device->CreateBuffer(&desc,&sub,&buffer)))
			{
			}
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(buffer,alias);
#endif
	}

	void CreateIndexBuffer(TComPtr<ID3D11Device> &device, UINT ByteWidth, const void * pData, TComPtr<ID3D11Buffer> &buffer, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_BUFFER_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Usage=D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth=ByteWidth;
		desc.BindFlags=D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags=0;
		D3D11_SUBRESOURCE_DATA sub;
		memset(&sub,0,sizeof(sub));
		sub.pSysMem=pData;
		if(FAILED(hr=device->CreateBuffer(&desc,&sub,&buffer)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(buffer,alias);
#endif
	}
	void CreateSamplerLinearWrap(TComPtr<ID3D11Device> &device, TComPtr<ID3D11SamplerState> &sample, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_SAMPLER_DESC samDesc;
		ZeroMemory( &samDesc, sizeof(samDesc) );
		samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samDesc.MaxAnisotropy = 1;
		samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samDesc.MaxLOD = D3D11_FLOAT32_MAX;
		if(FAILED(hr=device->CreateSamplerState(&samDesc,&sample)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(sample,alias);
#endif
	}


	void CreateSamplerLinearClamp(TComPtr<ID3D11Device> &device, TComPtr<ID3D11SamplerState> &sample, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_SAMPLER_DESC samDesc;
		ZeroMemory( &samDesc, sizeof(samDesc) );
		samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samDesc.MaxAnisotropy = 1;
		samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samDesc.MaxLOD = D3D11_FLOAT32_MAX;
		if(FAILED(hr=device->CreateSamplerState(&samDesc,&sample)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(sample,alias);
#endif
	}

	void CreateSRVFromeFile(TComPtr<ID3D11Device> &device, std::string FileName, TComPtr<ID3D11ShaderResourceView> &srv, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		std::wstring wFileName(FileName.begin(),FileName.end());
		/*if(FAILED(hr=CreateWICTextureFromFile(device,nullptr,wFileName.c_str(),NULL,&srv)))
		{
			THROW_EXCEPTION(YYUTException()<<err_str("create SRV from file["+FileName+"] failed")<<err_hr(hr));
		}*/
		if(FAILED(hr = CreateDDSTextureFromFile(device,wFileName.c_str(),NULL,&srv)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(srv,alias);
#endif
	}

	void CreateRasterState(TComPtr<ID3D11Device> &device, TComPtr<ID3D11RasterizerState> &RasterState, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_RASTERIZER_DESC desc;
		desc.FillMode = D3D11_FILL_SOLID;

		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.ScissorEnable = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;
		if(FAILED(hr= device->CreateRasterizerState( &desc, &RasterState )))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(RasterState,alias);
#endif
	}

	void CreateRasterState(TComPtr<ID3D11Device> &device, TComPtr<ID3D11RasterizerState> &RasterState, int DepthDias, float DepthBiasClamp, float SlopeScaledDepthBias, std::string alias/*= ""*/)
	{
		HRESULT hr=S_OK;
		D3D11_RASTERIZER_DESC desc;
		desc.FillMode = D3D11_FILL_SOLID;

		desc.CullMode = D3D11_CULL_NONE;
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = DepthDias;
		desc.DepthBiasClamp = DepthBiasClamp;
		desc.SlopeScaledDepthBias = SlopeScaledDepthBias;
		desc.DepthClipEnable = TRUE;
		desc.ScissorEnable = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;
		if(FAILED(hr= device->CreateRasterizerState( &desc, &RasterState )))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(RasterState,alias);
#endif
	}

	void CreateRasterStateNonCull(TComPtr<ID3D11Device> &device, TComPtr<ID3D11RasterizerState> &RasterState, std::string alias /*= ""*/)
	{
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

	void CreateBlendState(TComPtr<ID3D11Device> &device, TComPtr<ID3D11BlendState> &BlendState, bool Opaque/*=true*/, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;
		desc.RenderTarget[0].BlendEnable = (Opaque)?FALSE:TRUE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; 
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; 
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA; 
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA; 
		desc.RenderTarget[0].RenderTargetWriteMask =  D3D11_COLOR_WRITE_ENABLE_ALL;
		if(FAILED(hr=device->CreateBlendState(&desc,&BlendState)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(BlendState,alias);
#endif
	}

	void CreateDepthStencileState(TComPtr<ID3D11Device> &device, TComPtr<ID3D11DepthStencilState> &ds, bool Write/*=true*/, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = TRUE; 
		desc.DepthWriteMask = Write?D3D11_DEPTH_WRITE_MASK_ALL:D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS; 
		desc.StencilEnable = FALSE; 
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK; 
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK; 
		if(FAILED(hr=device->CreateDepthStencilState(&desc,&ds)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(ds,alias);
#endif
	}

	void CreateDepthStencileStateNoWriteNoTest(TComPtr<ID3D11Device> &device, TComPtr<ID3D11DepthStencilState> &ds, std::string alias /*= ""*/)
	{
		HRESULT hr = S_OK;
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = FALSE;
		desc.DepthWriteMask =  D3D11_DEPTH_WRITE_MASK_ZERO;
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

	void CreateRenderTarget(TComPtr<ID3D11Device> &device, DXGI_FORMAT format, TComPtr<ID3D11Texture2D>& texture, TComPtr<ID3D11RenderTargetView> &rtv, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;
		if(FAILED(hr=device->CreateRenderTargetView( texture, &desc, &rtv )))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(rtv,alias);
#endif
	}

	void CreateBufferSRV_UAV(TComPtr<ID3D11Device> &device, int ByteWidth,TComPtr<ID3D11Buffer> &buffer, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_BUFFER_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.BindFlags=D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth=ByteWidth;
		desc.CPUAccessFlags=0;
		desc.MiscFlags=0;
		desc.StructureByteStride=0;
		desc.Usage=D3D11_USAGE_DEFAULT;
		if(FAILED(hr=device->CreateBuffer(&desc,NULL,&buffer)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(buffer,alias);
#endif
	}

	void CreateSRVForBuffer(TComPtr<ID3D11Device> &device, DXGI_FORMAT format, UINT number,  TComPtr<ID3D11Buffer> &buffer,TComPtr<ID3D11ShaderResourceView> &srv, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Format=format;
		desc.ViewDimension=D3D11_SRV_DIMENSION_BUFFER;
		desc.Buffer.ElementOffset=0;
		desc.Buffer.ElementWidth=number;
		if(FAILED(hr=device->CreateShaderResourceView(buffer,&desc,&srv)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(srv,alias);
#endif
	}

	void Create2DTextureDSV_SRV(TComPtr<ID3D11Device> &device, UINT width, UINT height, DXGI_FORMAT format, TComPtr<ID3D11Texture2D> &tex2D, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.Width = width;
		desc.Height = height;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.CPUAccessFlags=0;
		desc.MiscFlags=0;
		if(FAILED(hr=device->CreateTexture2D(&desc,NULL,&tex2D)))
		{
		}

#if defined DEBUG | defined _DEBUG
		AddAlias(tex2D,alias);
#endif
	}

	void CreateDSVForTexture2D(TComPtr<ID3D11Device> &device, DXGI_FORMAT format, TComPtr<ID3D11Texture2D> &tex2D, TComPtr<ID3D11DepthStencilView> &dsv, std::string alias/*=""*/)
	{
		HRESULT hr=S_OK;
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Format=format;
		desc.ViewDimension=D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice=0;
		desc.Flags=0;
		if(FAILED(hr=device->CreateDepthStencilView(tex2D,&desc,&dsv)))
		{
		}
#if defined DEBUG | defined _DEBUG
		AddAlias(dsv,alias);
#endif
	}

	void Create2DTextureArrayDSV_SRV(TComPtr<ID3D11Device> &device, UINT width, UINT height, DXGI_FORMAT format, UINT ArraySize, TComPtr<ID3D11Texture2D> &tex2D, std::string alias/*=""*/)
	{
        HRESULT hr=S_OK;
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory( &desc, sizeof( desc ) );
        desc.Width = width;
        desc.Height = height;
        desc.ArraySize = ArraySize;
        desc.Format = format;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.CPUAccessFlags=0;
        desc.MiscFlags=0;
        if(FAILED(hr=device->CreateTexture2D(&desc,NULL,&tex2D)))
        {
        }
#if defined DEBUG | defined _DEBUG
        AddAlias(tex2D,alias);
#endif 
	}

    void CreateSamplerComparision(TComPtr<ID3D11Device> &device, TComPtr<ID3D11SamplerState> &sample, std::string alias/*= ""*/)
    {
        HRESULT hr=S_OK;
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
        if(FAILED(hr=device->CreateSamplerState(&samDesc,&sample)))
        {
        }
#if defined DEBUG | defined _DEBUG
        AddAlias(sample,alias);
#endif
    }

    void CreateSRVForTexture2DArray(TComPtr<ID3D11Device> &device, DXGI_FORMAT format, UINT arraySize, TComPtr<ID3D11Texture2D> &tex2D, TComPtr<ID3D11ShaderResourceView> &srv, std::string alias/*=""*/)
    {
        HRESULT hr=S_OK;
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        memset(&desc,0,sizeof(desc));
        desc.Format=format;
        desc.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        desc.Texture2DArray.MostDetailedMip = 0;
        desc.Texture2DArray.MipLevels = 1;
        desc.Texture2DArray.FirstArraySlice = 0;
        desc.Texture2DArray.ArraySize = arraySize;
        desc.Texture2D.MostDetailedMip=0;
        if(FAILED(hr=device->CreateShaderResourceView(tex2D,&desc,&srv)))
        {
        }
#if defined DEBUG | defined _DEBUG
        AddAlias(srv,alias);
#endif
    }

    void CreateDSVForTexture2DArray(TComPtr<ID3D11Device> &device, DXGI_FORMAT format, UINT arraySize, TComPtr<ID3D11Texture2D> &tex2D, TComPtr<ID3D11DepthStencilView> &dsv, std::string alias/*=""*/)
    {
        HRESULT hr=S_OK;
        D3D11_DEPTH_STENCIL_VIEW_DESC desc;
        memset(&desc,0,sizeof(desc));
        desc.Format=format;
        desc.ViewDimension=D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        desc.Texture2DArray.ArraySize = arraySize;
        desc.Texture2DArray.FirstArraySlice = 0;
        desc.Texture2DArray.MipSlice = 0;
        desc.Flags=0;
        if(FAILED(hr=device->CreateDepthStencilView(tex2D,&desc,&dsv)))
        {
        }
#if defined DEBUG | defined _DEBUG
        AddAlias(dsv,alias);
#endif
    }

    void CreateGeometryShader(TComPtr<ID3D11Device> &device, std::string FileName, std::string MainPoint, TComPtr<ID3D11GeometryShader> &gs, std::string alias /*= ""*/)
    {
        HRESULT hr=S_OK;
        TComPtr<ID3DBlob> vs_blob;
        ComplieShaderFromFile(FileName,MainPoint,"gs_5_0",vs_blob);
        if(FAILED(hr=device->CreateGeometryShader(vs_blob->GetBufferPointer(),vs_blob->GetBufferSize(),NULL,& gs)))
        {
        }
#if defined DEBUG | defined _DEBUG
        AddAlias(gs,alias);	
#endif
    }


