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

struct VS_INPUT
{
	float3   vPosition		: POSITION;
	float3    TangentX		: NORMAL0;
	float3    TangentY		: NORMAL1;
	float4    TangentZ		: NORMAL2;
	float2	TexCoords[4]	: ATTRIBUTE5;
	uint4    BlendIndices	: ATTRIBUTE6;
	uint4    BlendIndicesExtra	: ATTRIBUTE7;
	float4	BlendIndices	: ATTRIBUTE8;
	float4	BlendIndicesExtra	: ATTRIBUTE9;
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
	//matrix matWVP = mul(g_VP, g_world);
	Output.vPosition = mul(float4(Input.vPosition,1.0f), matWVP);
	Output.vNormal = normalize(mul(Input.vNormal, (float3x3) g_world));
	Output.vTexcoord = Input.vTexcoord;
	Output.vColor = Input.vColor;
	return Output;
}

float4 PSColor(VS_OUTPUT Input) :SV_Target
{
	return Input.vColor;
}