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
	half3     TangentX	: ATTRIBUTE1;
	half4     TangentZ  : ATTRIBUTE2;
	float2	  TexCoords[4]	: ATTRIBUTE4;
	float4    VertexColor	: ATTRIBUTE8;
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
	Output.vNormal = normalize(mul(Input.TangentZ.xyz, (float3x3) g_world));
	Output.vTexcoord = Input.TexCoords[0];
	Output.vColor = Input.VertexColor;
	return Output;
}

float4 PSMain(VS_OUTPUT Input) :SV_Target
{
	float NDL = clamp(dot(Input.vNormal,g_lightDir),0.2,1);
	return float4(NDL, NDL, NDL, 1.0f);
	//return float4(1,1,1,1);
}