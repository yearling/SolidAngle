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
	float4    vPosition		: ATTRIBUTE;
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
};

VS_OUTPUT VSMain(VS_INPUT Input)
{
	VS_OUTPUT Output;
	matrix matWVP = mul(g_world, g_VP);
	Output.vPosition = mul(Input.vPosition, matWVP);
    half3 TangetZ = Input.TangentZ.xyz / half3(127.0f, 127.0f, 127.0f) - 1.0f;
	Output.vNormal = normalize(mul(TangetZ, (float3x3) g_world));
	Output.vTexcoord = Input.TexCoords[0];
	return Output;
}

float4 PSMain(VS_OUTPUT Input) :SV_Target
{
	float NDL = clamp(dot(Input.vNormal,g_lightDir),0.2,1);
    float3 Color = Input.vNormal * 0.5 + 0.5;
    return float4(Color,1.0f);
	//return float4(1,1,1,1);
}