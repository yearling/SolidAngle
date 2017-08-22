Texture2D txDiffuse : register (t0);
SamplerState samLinear : register( s0);

cbuffer ChangePerFrame :register(b0)
{
	matrix g_view;
	matrix g_projection;
	matrix g_VP;
	matrix g_InvVP;
	float3 g_lightDir;
	float  g_pad;
}

cbuffer ChangePerMesh :register(b1)
{
	matrix g_world;
}

struct VS_INPUT
{
	float3 vPosition	: POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
	float4 vColor		: COLOR0;
};
struct VS_OUTPUT
{
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
	float4 vPosition	: SV_POSITION;
	float4 vColor		: COLOR0;
};

VS_OUTPUT VSMain( VS_INPUT Input )
{
	VS_OUTPUT Output;
	matrix matWVP = mul(g_world, g_VP);
	//matrix matWVP = mul(g_VP, g_world);
	Output.vPosition = mul( float4(Input.vPosition,1.0f), matWVP);
	Output.vNormal = normalize(mul(Input.vNormal, (float3x3) g_world));
	Output.vTexcoord = Input.vTexcoord;
	Output.vColor = Input.vColor;
	return Output;
}

float4 PSMain(VS_OUTPUT Input):SV_Target
{
	float fLighting = saturate(dot( g_lightDir , Input.vNormal ))+0.2;
	//float fLighting = saturate( dot( float3(1,1,1) , Input.vNormal )+0.4 );
	//return (0.2f + fLighting)*txDiffuse.Sample( samLinear, Input.vTexcoord );
	return float4(fLighting, fLighting, fLighting,1.0f);
	//return float4(saturate(Input.vNormal),1.0f);
}

float4 PSColor(VS_OUTPUT Input) :SV_Target
{
	return float4(Input.vColor.xyz,1.0f);
}