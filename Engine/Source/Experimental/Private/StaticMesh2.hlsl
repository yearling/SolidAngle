cbuffer ChangePerFrame 
{
	matrix g_view;
	matrix g_projection;
	matrix g_VP;
	matrix g_InvVP;
	float3 g_lightDir;
}

cbuffer ChangePerMesh 
{
	matrix g_world;
}


struct VS_INPUT
{
	float3    vPosition		: ATTRIBUTE;
	half3     TangentX : ATTRIBUTE1;
	half4     TangentZ : ATTRIBUTE2;
    half2     TexCoords[2] : ATTRIBUTE3;
};
struct VS_OUTPUT
{
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
	float4 vPosition	: SV_POSITION;
	float4 vColor		: COLOR0;
	float3x3 TangentMat : ATTRIBUTE;
};

VS_OUTPUT VSMain(VS_INPUT Input)
{
	VS_OUTPUT Output;
	matrix matWVP = mul(g_world, g_VP);
	Output.vPosition = mul(float4(Input.vPosition,1.0), matWVP);
	half3 TangetZ = Input.TangentZ.xyz;
	Output.vNormal = normalize(mul(TangetZ, (float3x3) g_world));
	Output.vTexcoord = Input.TexCoords[0];
	float3 TangentX = normalize(Input.TangentX.xyz);
	float4 InputTangentZ = Input.TangentZ;
	float3 TangentZ = normalize(InputTangentZ.xyz);
	float3 TangentY = cross(TangentZ, TangentX)*InputTangentZ.w;
	Output.TangentMat = float3x3(TangentX, TangentY, TangentZ);
	return Output;
}


Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

float4 PSMain(VS_OUTPUT Input) :SV_Target
{
    //float3 Color = Input.vNormal * 0.5 + 0.5;
	float3 Color = Input.vNormal;
	float4 NormalValue = txDiffuse.Sample(samLinear, Input.vTexcoord);
	float3 NormalizeNormal = normalize((NormalValue * 2.0 - 1.0).xyz);
	float3 NormalInLocal = normalize(mul(NormalizeNormal, Input.TangentMat));
	//float NDL = clamp(dot(NormalInLocal,g_lightDir),0.2,1);
    //return float4(NDL, NDL, NDL,1.0);
	float3 NormalInLocalToShow = (NormalInLocal + 1.0)*0.5;
	return float4(NormalInLocalToShow, 1.0);
}