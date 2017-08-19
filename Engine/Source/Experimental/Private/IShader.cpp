#include "IShader.h"
#include "YYUTDXManager.h"
#include "YYUTHelper.h"
#include <fstream>
#include <iostream>
#include <d3dcompiler.h>


const char* gSystemDir = "..\\Shader";

class CShaderInclude : public ID3DInclude
{
public:
	CShaderInclude(FString shaderDir, FString systemDir = gSystemDir) : m_ShaderDir(MoveTemp(shaderDir)), m_SystemDir(MoveTemp(systemDir)) {}

	HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
	{
		FString finalPath;
		switch (IncludeType)
		{
		case D3D_INCLUDE_LOCAL:
			finalPath = m_ShaderDir + TEXT("\\") + pFileName;
			break;
		case D3D_INCLUDE_SYSTEM:
			finalPath = m_SystemDir + TEXT("\\") + pFileName;
			break;
		default:
			assert(0);
		}
		std::ifstream includeFile(*finalPath, std::ios::in | std::ios::binary | std::ios::ate);
		if (includeFile.is_open())
		{
			long long fileSize = includeFile.tellg();
			char* buf = new char[fileSize];
			includeFile.seekg(0, std::ios::beg);
			includeFile.read(buf, fileSize);
			includeFile.close();
			*ppData = buf;
			*pBytes = (unsigned int)fileSize;
		}
		else
		{
			return E_FAIL;
		}
		return S_OK;
	}
	HRESULT __stdcall Close(LPCVOID pData)
	{
		char* buf = (char*)pData;
		delete[] buf;
		return S_OK;
	}
private:
	FString m_ShaderDir;
	FString m_SystemDir;
};

bool ComplieShaderFromFile(const FString &ShaderFileName,
	const FString &entry_point,
	const FString &shader_model,
	const TArray<ShaderMacroEntry>& ShaderMacroEntrys,
	const FString &IncludePath,
	TComPtr<ID3DBlob> &blob,
	/*out*/ FString &ErrorMsg,
	bool ColomMajor = true)
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
	TArray<D3D_SHADER_MACRO> D3DShaderMacro;
	if (ShaderMacroEntrys.Num())
	{
		for (const ShaderMacroEntry& Entry : ShaderMacroEntrys)
		{
			D3D_SHADER_MACRO Macro;
			Macro.Name = TCHAR_TO_ANSI(*Entry.MacroName);
			Macro.Definition = TCHAR_TO_ANSI(*Entry.Value);
			D3DShaderMacro.Emplace(MoveTemp(Macro));
		}
		D3D_SHADER_MACRO MacroEnd;
		MacroEnd.Definition = nullptr;
		MacroEnd.Name = nullptr;
		D3DShaderMacro.Emplace(MoveTemp(MacroEnd));
	}
	TUniquePtr<CShaderInclude> ShaderInclude = IncludePath.IsEmpty() ? nullptr : MakeUnique<CShaderInclude>(IncludePath);

	if (FAILED(hr = D3DCompileFromFile(*ShaderFileName,
		ShaderMacroEntrys.Num() ?  &D3DShaderMacro[0]:nullptr,
		ShaderInclude.Get(),
		TCHAR_TO_ANSI(*entry_point),
		TCHAR_TO_ANSI(*shader_model),
		shader_flags,
		0,
		&blob,
		&err_bob)))
	{
		if (err_bob)
		{
			ErrorMsg = (char*)err_bob->GetBufferPointer();
		}
		return false;
	}
	return true;
}

IShaderBind::~IShaderBind()
{

}

void IShaderBind::PostReflection(TComPtr<ID3DBlob> &Blob, TComPtr<ID3D11ShaderReflection>& ShaderReflector)
{
	//nothing
}

void IShaderBind::AddShaderMacros(const TArray<ShaderMacroEntry> & InShaderMacroEntrys)
{
	ShaderMacroEntrys.Append(InShaderMacroEntrys);
}

void IShaderBind::SetInclude(const FString & ShaderSrcInclude)
{
	ShaderIncludePath = ShaderSrcInclude;
}


bool IShaderBind::AddAlias(const FString & AliasName)
{
	AliasNameForDebug = AliasName;
	TComPtr<ID3D11DeviceChild> DeviceChild = GetInternalResource();
	if (DeviceChild && (!AliasNameForDebug.IsEmpty()))
	{
		HRESULT hr = S_OK;
		if (!AliasNameForDebug.IsEmpty())
		{
			if (FAILED(hr = DeviceChild->SetPrivateData(WKPDID_D3DDebugObjectName, AliasNameForDebug.Len()*sizeof(TCHAR),TCHAR_TO_ANSI(*AliasNameForDebug))))
			{
				return false;
			}
		}
	}
	return true;
}

bool IShaderBind::Update()
{
	TComPtr<ID3D11DeviceContext> DeviceContext = YYUTDXManager::GetInstance().GetD3DDC();
	for (TUniquePtr<YConstantBuffer> & ConstantBuffer : ConstantBuffers)
	{
		if (!ConstantBuffer->AllocResource())
			return false;
		D3D11_MAPPED_SUBRESOURCE MapResource;
		HRESULT hr = DeviceContext->Map(ConstantBuffer->D3DBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MapResource);
		memcpy_s(MapResource.pData, ConstantBuffer->CBSize, ConstantBuffer->ShadowBuffer.Get(), ConstantBuffer->CBSize);
		DeviceContext->Unmap(ConstantBuffer->D3DBuffer, 0);
	}
	return true;
}


bool IShaderBind::BindResource(const FString &ParaName, int n)
{

	return true;
}


bool IShaderBind::BindResourceHelp(const FString &ParaName, ScalarIndex& Index)
{
	auto FindResult = MapShaderVariableToScalar.Find(ParaName);
	if (!FindResult)
	{
		std::cout << "Bind [" << *ParaName << "] Shader Failed!!" << std::endl;
		return false;
	}
	Index = *FindResult;

	return true;
}

bool IShaderBind::BindResource(const FString &ParaName, float f)
{
	ScalarIndex Index;
	if (BindResourceHelp(ParaName, Index))
	{
		if (Index.Type != ScalarIndex::eType::FLOAT)
		{
			assert(0);
			return false;
		}
		YConstantBuffer::YCBScalar<float>::SetValue(ConstantBuffers[Index.ConstantBufferIndex].Get(), Index.ValueIndex, f);
		return true;
	}
	return false;
}

bool IShaderBind::BindResource(const FString &ParaName, float* f, int Num)
{
	ScalarIndex Index;
	if (BindResourceHelp(ParaName, Index))
	{
		assert(2 <= Num && Num <= 4);
		if (Num == 2)
		{
			YConstantBuffer::YCBVector2::SetValue(ConstantBuffers[Index.ConstantBufferIndex].Get(), Index.ValueIndex, f);
		}
		else if (Num == 3)
		{
			YConstantBuffer::YCBVector3::SetValue(ConstantBuffers[Index.ConstantBufferIndex].Get(), Index.ValueIndex, f);
		}
		else if (Num == 4)
		{
			YConstantBuffer::YCBVector4::SetValue(ConstantBuffers[Index.ConstantBufferIndex].Get(), Index.ValueIndex, f);
		}
	}
	return false;
}



bool IShaderBind::BindResource(const FString &ParaName, const DirectX::XMMATRIX *Mat, int Num)
{
	return true;
}

bool IShaderBind::BindResource(const FString &ParaName, XMFLOAT2 V2)
{
	ScalarIndex Index;
	if (BindResourceHelp(ParaName, Index))
	{
		if (Index.Type != ScalarIndex::eType::FLOAT2)
		{
			assert(0);
			return false;
		}
		YConstantBuffer::YCBVector2::SetValue(ConstantBuffers[Index.ConstantBufferIndex].Get(), Index.ValueIndex, (float*)&V2);
		return true;
	}
	return false;
}

bool IShaderBind::BindResource(const FString &ParaName, XMFLOAT3 V3)
{
	ScalarIndex Index;
	if (BindResourceHelp(ParaName, Index))
	{
		if (Index.Type != ScalarIndex::eType::FLOAT3)
		{
			assert(0);
			return false;
		}
		YConstantBuffer::YCBVector2::SetValue(ConstantBuffers[Index.ConstantBufferIndex].Get(), Index.ValueIndex, (float*)&V3);
		return true;
	}
	return false;
}

bool IShaderBind::BindResource(const FString &ParaName, XMFLOAT4 V4)
{
	ScalarIndex Index;
	if (BindResourceHelp(ParaName, Index))
	{
		if (Index.Type != ScalarIndex::eType::FLOAT4)
		{
			assert(0);
			return false;
		}
		YConstantBuffer::YCBVector2::SetValue(ConstantBuffers[Index.ConstantBufferIndex].Get(), Index.ValueIndex, (float*)&V4);
		return true;
	}
	return false;
}
bool IShaderBind::BindResource(const FString &ParaName, const DirectX::XMMATRIX &Mat)
{
	ScalarIndex Index;
	if (BindResourceHelp(ParaName, Index))
	{
		if (Index.Type != ScalarIndex::eType::MATRIX4X4)
		{
			assert(0);
			return false;
		}
		YConstantBuffer::YCBMatrix4X4::SetValue(ConstantBuffers[Index.ConstantBufferIndex].Get(), Index.ValueIndex, Mat);
		return true;
	}
	return false;
}
bool IShaderBind::ReflectShader(TComPtr<ID3DBlob> Blob)
{
	TComPtr<ID3D11ShaderReflection> ShaderReflector;
	HRESULT hr = D3DReflect(Blob->GetBufferPointer(), Blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&ShaderReflector);
	if (FAILED(hr))
	{
		return false;
	}
	D3D11_SHADER_DESC ShaderDesc;
	ShaderReflector->GetDesc(&ShaderDesc);
	// 获取shader中数据
	// Constant Buffer's count: $Global, cbxxx
	uint32 ConstantBufferCount = ShaderDesc.ConstantBuffers;
	for (unsigned int i = 0; i < ConstantBufferCount; ++i)
	{
		D3D11_SHADER_BUFFER_DESC ShaderBufferDesc;
		ID3D11ShaderReflectionConstantBuffer* pConstBuffer = ShaderReflector->GetConstantBufferByIndex(i);
		pConstBuffer->GetDesc(&ShaderBufferDesc);
		ConstantBuffers.Emplace(MakeUnique<YConstantBuffer>(ShaderBufferDesc.Size));
		TUniquePtr<YConstantBuffer>& ConstantBuffer = ConstantBuffers.Last();
		ConstantBuffer->CBName = ShaderBufferDesc.Name;
		ConstantBuffer->CBType = (YConstantBuffer::eCBType)ShaderBufferDesc.Type;
		D3D11_SHADER_INPUT_BIND_DESC BindDesc;
		hr = ShaderReflector->GetResourceBindingDescByName(TCHAR_TO_ANSI(*ConstantBuffer->CBName), &BindDesc);
		if (FAILED(hr))
		{
			return false;
		}
		ConstantBuffer->BindSlotIndex = BindDesc.BindPoint;
		ConstantBuffer->BindSlotNum = BindDesc.BindCount;
		for (unsigned int j = 0; j < ShaderBufferDesc.Variables; j++)
		{
			ID3D11ShaderReflectionVariable* pVariable = pConstBuffer->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC VarDesc;
			pVariable->GetDesc(&VarDesc);

			ID3D11ShaderReflectionType* pType = pVariable->GetType();
			D3D11_SHADER_TYPE_DESC TypeDesc;
			pType->GetDesc(&TypeDesc);
			if (TypeDesc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_SCALAR)
			{
				if (TypeDesc.Type == D3D_SHADER_VARIABLE_TYPE::D3D_SVT_BOOL)
				{

				}
				else if (TypeDesc.Type == D3D_SHADER_VARIABLE_TYPE::D3D_SVT_INT)
				{
					assert(0 && "shader reflection not support");
				}
				else if (TypeDesc.Type == D3D_SHADER_VARIABLE_TYPE::D3D_SVT_FLOAT)
				{
					if (TypeDesc.Type == D3D_SHADER_VARIABLE_TYPE::D3D_SVT_FLOAT)
					{
						AddScalarVariable(VarDesc.Name, i, VarDesc.StartOffset, ScalarIndex::eType::FLOAT);
					}
					else
					{
						assert(0 && "shader reflection not support");
					}
				}
				else
				{
					assert(0 && "shader reflection not support scalar type");
				}
			}
			else if (TypeDesc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_VECTOR)
			{
				if (TypeDesc.Type == D3D_SHADER_VARIABLE_TYPE::D3D_SVT_FLOAT)
				{
					assert(TypeDesc.Rows == 1);
					if (TypeDesc.Columns == 2)
					{
						AddScalarVariable(VarDesc.Name, i, VarDesc.StartOffset, ScalarIndex::eType::FLOAT2);
					}
					else if (TypeDesc.Columns == 3)
					{
						AddScalarVariable(VarDesc.Name, i, VarDesc.StartOffset, ScalarIndex::eType::FLOAT3);
					}
					else if (TypeDesc.Columns == 4)
					{
						AddScalarVariable(VarDesc.Name, i, VarDesc.StartOffset, ScalarIndex::eType::FLOAT4);
					}
					else
					{
						assert(0);
					}
				}
			}
			else if (TypeDesc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_MATRIX_COLUMNS)
			{
				if (TypeDesc.Type == D3D_SHADER_VARIABLE_TYPE::D3D_SVT_FLOAT && TypeDesc.Columns == 4 && TypeDesc.Rows == 4)
				{
					AddScalarVariable(VarDesc.Name, i, VarDesc.StartOffset, ScalarIndex::eType::MATRIX4X4);
				}
				else
				{
					assert(0 && "shader reflection not support matrix type");
				}
			}
			else if (TypeDesc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_MATRIX_ROWS)
			{
				assert(0 && "shader not support matrix row major");
			}
		}
	}
	PostReflection(Blob, ShaderReflector);
	return true;
}

void IShaderBind::AddScalarVariable(const FString &Name, uint32 InConstantBufferIndex, uint32 InValueIndex, ScalarIndex::eType InType)
{
	ScalarIndex Index;
	Index.ConstantBufferIndex = InConstantBufferIndex;
	Index.ValueIndex = InValueIndex;
	Index.Type = InType;
	if (!MapShaderVariableToScalar.Find(Name))
	{
		MapShaderVariableToScalar.Add(Name, Index);
	}
	else
	{
		assert(0 && "should not have same name variable");
	}
}

YVSShader::YVSShader()
{
}

YVSShader::~YVSShader()
{

}

bool YVSShader::CreateShader(const FString &FileName, const FString &MainPoint)
{
	TComPtr<ID3D11Device> Device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	TComPtr<ID3DBlob> VSBlob;
	FString ErrorMsg;
	if (!ComplieShaderFromFile(FileName, MainPoint, "vs_5_0", ShaderMacroEntrys, ShaderIncludePath, VSBlob, ErrorMsg))
	{
		std::cout << "VS Shader file compile failed!! \n FileName: " << *FileName << std::endl;
		std::cout << "Error msg is :" << *ErrorMsg << std::endl;
		if (ShaderMacroEntrys.Num())
		{
			std::cout << "ShaderMacroEntrys:" << std::endl;
			for (ShaderMacroEntry &Entry : ShaderMacroEntrys)
			{
				std::cout << "\"" << *Entry.MacroName << "\"" << '[' << *Entry.Value << ']' << std::endl;
			}
		}
		if (!ShaderIncludePath.IsEmpty())
		{
			std::cout << "ShaderInclude:" << *ShaderIncludePath << std::endl;
		}
		return false;
	}
	if (FAILED(hr = Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &VertexShader)))
	{
		std::cout << "VS Shader create failed!! \n FileName: " << *FileName << "   MainPoint:" << *MainPoint << std::endl;
		return false;
	}
	ReflectShader(VSBlob);
	assert(VertexShader);
	if (!AliasNameForDebug.IsEmpty())
	{
		AddAlias(AliasNameForDebug);
	}
	return true;
}

bool YVSShader::Update()
{
	TComPtr<ID3D11DeviceContext> DeviceContext = YYUTDXManager::GetInstance().GetD3DDC();
	if (IShaderBind::Update() && VertexShader)
	{
		DeviceContext->VSSetShader(VertexShader, nullptr, 0);
		DeviceContext->IASetInputLayout(InputLayout);
		for (TUniquePtr<YConstantBuffer> & ConstantBuffer : ConstantBuffers)
		{
			DeviceContext->VSSetConstantBuffers(ConstantBuffer->BindSlotIndex, 1, &ConstantBuffer->D3DBuffer);
		}
		return true;
	}
	return false;
}

void YVSShader::PostReflection(TComPtr<ID3DBlob> &Blob, TComPtr<ID3D11ShaderReflection>& ShaderReflector)
{
	D3D11_SHADER_DESC ShaderDesc;
	ShaderReflector->GetDesc(&ShaderDesc);

	TArray<D3D11_INPUT_ELEMENT_DESC> lInputLayoutDesc;
	for (unsigned lI = 0; lI < ShaderDesc.InputParameters; lI++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC lParamDesc;
		ShaderReflector->GetInputParameterDesc(lI, &lParamDesc);

		D3D11_INPUT_ELEMENT_DESC lElementDesc;
		lElementDesc.SemanticName = lParamDesc.SemanticName;
		lElementDesc.SemanticIndex = lParamDesc.SemanticIndex;

		lElementDesc.InputSlot = 0;
		lElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		lElementDesc.InstanceDataStepRate = 0;
		lElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		if (lParamDesc.Mask == 1)
		{
			if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) lElementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) lElementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) lElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (lParamDesc.Mask <= 3)
		{
			if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) lElementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) lElementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) lElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (lParamDesc.Mask <= 7)
		{
			if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) lElementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) lElementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) lElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (lParamDesc.Mask <= 15)
		{
			if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) lElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) lElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (lParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) lElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		lInputLayoutDesc.Emplace(lElementDesc);
	}
	TComPtr<ID3D11Device> Device = YYUTDXManager::GetInstance().GetD3DDevice();
	if (FAILED(Device->CreateInputLayout(&lInputLayoutDesc[0], ShaderDesc.InputParameters, Blob->GetBufferPointer(), Blob->GetBufferSize(), &InputLayout)))
	{
		std::cout << "VS Shader create layout failed!! \n FileName: " << std::endl;
	}
}

TComPtr<ID3D11DeviceChild> YVSShader::GetInternalResource() const
{
	return TComPtr<ID3D11DeviceChild>(VertexShader);
}

IShader::~IShader()
{

}

bool YConstantBuffer::AllocResource()
{
	if (D3DBuffer)
		return true;
	else
	{
		HRESULT hr = S_OK;
		TComPtr<ID3D11Device> Device = YYUTDXManager::GetInstance().GetD3DDevice();
		D3D11_BUFFER_DESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.ByteWidth = CBSize;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		if (FAILED(hr = Device->CreateBuffer(&desc, NULL, &D3DBuffer)))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

YPSShader::YPSShader()
{

}

YPSShader::~YPSShader()
{

}

bool YPSShader::CreateShader(const FString &FileName, const FString &MainPoint)
{
	TComPtr<ID3D11Device> Device = YYUTDXManager::GetInstance().GetD3DDevice();
	HRESULT hr = S_OK;
	TComPtr<ID3DBlob> VSBlob;
	FString ErrorMsg;
	if (!ComplieShaderFromFile(FileName, MainPoint, "ps_5_0", ShaderMacroEntrys, ShaderIncludePath, VSBlob, ErrorMsg))
	{
		std::cout << "VS Shader file compile failed!! \n FileName: " << *FileName << std::endl;
		std::cout << "Error msg is :" << *ErrorMsg << std::endl;
		if (ShaderMacroEntrys.Num())
		{
			std::cout << "ShaderMacroEntrys:" << std::endl;
			for (ShaderMacroEntry &Entry : ShaderMacroEntrys)
			{
				std::cout << "\"" << *Entry.MacroName << "\"" << '[' << *Entry.Value << ']' << std::endl;
			}
		}
		if (!ShaderIncludePath.IsEmpty())
		{
			std::cout << "ShaderInclude:" << *ShaderIncludePath << std::endl;
		}
		return false;
	}
	if (FAILED(hr = Device->CreatePixelShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), NULL, &PixShader)))
	{
		std::cout << "VS Shader create failed!! \n FileName: " << *FileName << "   MainPoint:" << *MainPoint << std::endl;
		return false;
	}
	ReflectShader(VSBlob);
	assert(PixShader);
	if (!AliasNameForDebug.IsEmpty())
	{
		AddAlias(AliasNameForDebug);
	}
	return true;
}

bool YPSShader::Update()
{
	TComPtr<ID3D11DeviceContext> DeviceContext = YYUTDXManager::GetInstance().GetD3DDC();
	if (IShaderBind::Update() && PixShader)
	{
		DeviceContext->PSSetShader(PixShader, nullptr, 0);
		for (TUniquePtr<YConstantBuffer> & ConstantBuffer : ConstantBuffers)
		{
			DeviceContext->PSSetConstantBuffers(ConstantBuffer->BindSlotIndex, 1, &ConstantBuffer->D3DBuffer);
		}
		return true;
	}
	return false;
}

TComPtr<ID3D11DeviceChild> YPSShader::GetInternalResource() const
{
	return TComPtr<ID3D11DeviceChild>(PixShader);
}
