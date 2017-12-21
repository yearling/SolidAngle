cbuffer ChangePerFrame 
{
	matrix g_view;
	matrix g_projection;
	matrix g_VP;
	matrix g_InvVP;
	float3 g_lightDir;
	float  g_pad;
}

cbuffer ChangePerMesh 
{
	matrix g_world;
}

#define FBoneMatrix float3x4
Buffer<float4> BoneMatrices;

struct FVertexFactoryInput
{
	float4    vPosition		: ATTRIBUTE;
	float3    TangentX		: ATTRIBUTE1;
	float3    TangentY		: ATTRIBUTE2;
	float4    TangentZ		: ATTRIBUTE3;
	float2	  TexCoords[4]	: ATTRIBUTE4;
	float4    VertexColor	: ATTRIBUTE8;
	uint4     BlendIndices	: ATTRIBUTE9;
	uint4     BlendIndicesExtra	: ATTRIBUTE10;
	float4	  BlendWeights	: ATTRIBUTE11;
	float4	  BlendWeightsExtra	: ATTRIBUTE12;
};

FBoneMatrix GetBoneMatrix(int Index)
{
	float4 A = BoneMatrices[Index * 3];
	float4 B = BoneMatrices[Index * 3 + 1];
	float4 C = BoneMatrices[Index * 3 + 2];
	return FBoneMatrix(A, B, C);
}

FBoneMatrix CalcBoneMatrix(FVertexFactoryInput Input)
{
	FBoneMatrix BoneMatrix = Input.BlendWeights.x * GetBoneMatrix(Input.BlendIndices.x);
	BoneMatrix += Input.BlendWeights.y * GetBoneMatrix(Input.BlendIndices.y);
	BoneMatrix += Input.BlendWeights.z * GetBoneMatrix(Input.BlendIndices.z);
	BoneMatrix += Input.BlendWeights.w * GetBoneMatrix(Input.BlendIndices.w);
	BoneMatrix += Input.BlendWeightsExtra.x * GetBoneMatrix(Input.BlendIndicesExtra.x);
	BoneMatrix += Input.BlendWeightsExtra.y * GetBoneMatrix(Input.BlendIndicesExtra.y);
	BoneMatrix += Input.BlendWeightsExtra.z * GetBoneMatrix(Input.BlendIndicesExtra.z);
	BoneMatrix += Input.BlendWeightsExtra.w * GetBoneMatrix(Input.BlendIndicesExtra.w);
	return BoneMatrix;
}



struct VS_OUTPUT
{
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
	float4 vPosition	: SV_POSITION;
	float4 vColor		: COLOR0;
};

VS_OUTPUT VSMain(FVertexFactoryInput Input)
{
	VS_OUTPUT Output;
	FBoneMatrix BoneTransformMatrix = CalcBoneMatrix(Input);
	//float4 SkinPos = mul(Input.vPosition, BoneTransformMatrix);
	float4 SkinPos = float4(mul( BoneTransformMatrix, Input.vPosition),1.0);
	Output.vPosition = mul(SkinPos, g_VP);
	Output.vNormal = normalize(mul(Input.TangentZ.xyz, (float3x3) g_world));
	//Output.vPosition = mul( matWVP, float4(Input.vPosition, 1.0f));
	//Output.vNormal = normalize(mul((float3x3) g_world, Input.TangentZ.xyz));
	Output.vTexcoord = Input.TexCoords[0];
	Output.vColor = Input.VertexColor+ CalcBoneMatrix(Input)[0];
	return Output;
}

float4 PSMain(VS_OUTPUT Input) :SV_Target
{
	float NDL = clamp(dot(Input.vNormal,g_lightDir),0.2,1);
	return float4(NDL, NDL, NDL, 1.0f);
	//return float4(1,1,1,1);
}