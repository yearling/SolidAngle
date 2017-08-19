	Texture2D	g_Texture				:register (t0);
	SamplerState g_samWrapLinear		:register ( s0 );
    
    /*SamplerState Sampler
    {
        Filter = MIN_MAG_MIP_LINEAR;
        AddressU = Wrap;
        AddressV = Wrap;
    };*/
    
    struct VS_OUTPUT
    {
        float4 Pos : SV_POSITION;
        float4 Dif : COLOR;
        float2 Tex : TEXCOORD;
    };
    
    VS_OUTPUT VSmain( float3 vPos : POSITION,
                  float4 Dif : COLOR,
                  float2 vTexCoord0 : TEXCOORD )
    {
        VS_OUTPUT Output;
    
        Output.Pos = float4( vPos, 1.0f );
        Output.Dif = Dif;
        Output.Tex = vTexCoord0;
    
        return Output;
    }
    
    float4 PSmain( VS_OUTPUT In ) : SV_Target
    {
        //return g_Texture.Sample( Sampler, In.Tex ) * In.Dif;
		return g_Texture.SampleLevel(g_samWrapLinear,In.Tex,0) *In.Dif;
    }
    
    float4 PSUntex( VS_OUTPUT In ) : SV_Target
    {
        return In.Dif;
    }
    
    //technique10 RenderUI
    //{
    //    pass P0
    //    {
    //        SetVertexShader( CompileShader( vs_4_0, VS() ) );
    //        SetGeometryShader( NULL );
    //        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    //        SetDepthStencilState( DisableDepth, 0 );
    //        SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    //    }
    //}
    //technique10 RenderUIUntex
    //{
    //    pass P0
    //    {
    //        SetVertexShader( CompileShader( vs_4_0, VS() ) );
    //        SetGeometryShader( NULL );
    //        SetPixelShader( CompileShader( ps_4_0, PSUntex() ) );
    //        SetDepthStencilState( DisableDepth, 0 );
    //        SetBlendState( UIBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    //    }
    //}
    //technique10 RestoreState
    //{
    //    pass P0
    //    {
    //        SetDepthStencilState( EnableDepth, 0 );
    //        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    //    }
    //};