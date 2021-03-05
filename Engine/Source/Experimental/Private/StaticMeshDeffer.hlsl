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
	half3     TangentX		: ATTRIBUTE1;
	half4     TangentZ		: ATTRIBUTE2;
    half2     TexCoords[2]  : ATTRIBUTE3;
};
struct VS_OUTPUT
{
	float2 vTexcoord[2]	: TEXCOORD;
	float4 vPosition	: SV_POSITION;
	float4 vColor		: COLOR0;
	float3x3 TangentToLocal : ATTRIBUTE;
};

VS_OUTPUT VSMain(VS_INPUT Input)
{
	VS_OUTPUT Output;

	matrix matWVP = mul(g_world, g_VP);

	Output.vPosition = mul(float4(Input.vPosition,1.0), matWVP);
	Output.vTexcoord[0] = Input.TexCoords[0];
	Output.vTexcoord[1] = Input.TexCoords[1];

	float3 TangentX = normalize(Input.TangentX.xyz);
	float3 TangentZ = normalize(Input.TangentZ.xyz);
	float3 TangentY = cross(TangentZ, TangentX)* Input.TangentZ.w;
	Output.TangentToLocal = float3x3(TangentX, TangentY, TangentZ);
	return Output;
}


Texture2D txDiffuse;
Texture2D txNormal;
SamplerState samLinear;

float3 sRGBToLinear(float3 Color)
{
	Color = max(6.10352e-5, Color); // minimum positive non-denormal (fixes black problem on DX11 AMD and NV)
	return Color > 0.04045 ? pow(Color * (1.0 / 1.055) + 0.0521327, 2.4) : Color * (1.0 / 12.92);
}

float LinearToSrgbBranchingChannel(float lin)
{
	if (lin < 0.00313067)
		return lin * 12.92;
	return pow(lin, (1.0 / 2.4)) * 1.055 - 0.055;
}

float3 LinearToSrgbBranching(float3 lin)
{
	return half3(
		LinearToSrgbBranchingChannel(lin.r),
		LinearToSrgbBranchingChannel(lin.g),
		LinearToSrgbBranchingChannel(lin.b));
}
struct PixelShaderOutput
{
	float4 Diffuse				: SV_Target0;			//Color
	float2 Normal				: SV_Target1;			//Normal map
	float4 MRS       			: SV_Target2;
};


PixelShaderOutput PSMain(VS_OUTPUT Input)
{
	PixelShaderOutput OutPut;

	float4 Diffuse = txDiffuse.Sample(samLinear, Input.vTexcoord[0]);
    //Diffuse = pow(Diffuse, 2.2);
	float4 NormalTextureValue = txNormal.Sample(samLinear, Input.vTexcoord[0]);
	float3 NormalizedNormal = normalize((NormalTextureValue * 2.0 - 1.0).xyz);
	float3 NormalInLocal = normalize(mul(NormalizedNormal, Input.TangentToLocal));
	OutPut.Diffuse = Diffuse;

	OutPut.Normal = float2((NormalInLocal.xy + float2(1.0, 1.0))*0.5);
	OutPut.MRS = float4(1.0, 0.0, 0.0, 1.0);
	return OutPut;
}