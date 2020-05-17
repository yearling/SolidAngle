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

	Output.vPosition = float4(Input.vPosition, 1.0);
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
Texture2D txMRS;
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


float4 PSMain(VS_OUTPUT Input) :SV_Target
{
	float4 Diffuse = txDiffuse.Sample(samLinear, Input.vTexcoord[0]);
	float3 Normal = txNormal.Sample(samLinear, Input.vTexcoord[0]).xyz*2.0 - 1.0;

	float NDL = clamp(dot(Normal, g_lightDir), pow(0.05, 2.2), 1);
	float3 FinalColor = Diffuse * NDL;
	float4 FinalColorGarma = float4(FinalColor, 1.0);
	//float FixColor = 0.5;
	//float4 FinalColorGarma = float4(FixColor, FixColor, FixColor, 1.0);
	//FinalColorGarma = pow(FinalColorGarma, 1.0 / 2.2);
	FinalColorGarma.xyz = LinearToSrgbBranching(FinalColorGarma.xyz);
	FinalColorGarma.w = 1.0;
	return FinalColorGarma;
	//float3 NormalInLocalToShow = (NormalInLocal + 1.0)*0.5;
	//return float4(NormalInLocalToShow, 1.0);
}