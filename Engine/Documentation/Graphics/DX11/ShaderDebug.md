# Shader Debug Parameter
1. 为了方便使用VS自带的Graphic调试器与NSight,需要在编译shader时使用 
	1. D3DXSHADER_PREFER_FLOW_CONTROL
	2. D3DXSHADER_DEBUG
	3. D3DXSHADER_SKIPOPTIMIZATION
这三个编译选项来打开shader的debug信息。 