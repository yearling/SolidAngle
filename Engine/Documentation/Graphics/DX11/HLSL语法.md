# HLSL 

## 语法
senamic

1. 创建InputLayout时
		
		float3    vPosition		: ATTRIBUTE;
		float3    TangentX[2]	: ATTRIBUTE1;
		float3    TangentY		: ATTRIBUTE3;
    注意 TangentY是Attribute3，而不是2。
