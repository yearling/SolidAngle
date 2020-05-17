#pragma once
#include "Core.h"
#include "YRenderInterface.h"
#include "YScene.h"
#include "IShader.h"
#include "FullScreenQuad.h"
DECLARE_LOG_CATEGORY_EXTERN(LogDeferedRender, Log, All)


class YDeferedRender :public IYRenderInterface
{
public:
	YDeferedRender();
	~YDeferedRender();
	void InitRenders();
	void RenderScene(TRefCountPtr<YScene>& Scene, TSharedRef<YRenderInfo> RenderInfo) override;
private:
	TUniquePtr<IYRenderPolicy>		BasePassRenderPolicy;
	TUniquePtr<IYRenderPolicy>		DirectLightPolicy;
	float							m_ScreenWidth;
	float							m_ScreenHeight;
	TComPtr<ID3D11RasterizerState>	m_rs;
	TComPtr<ID3D11BlendState>		m_bs;
	TComPtr<ID3D11DepthStencilState>m_ds;

	//GBuffer
	TComPtr<ID3D11Texture2D>		DiffuseBuffer;
	TComPtr<ID3D11Texture2D>		NormalBuffer;
	TComPtr<ID3D11Texture2D>		MRSBuffer;
	TComPtr<ID3D11ShaderResourceView> DiffuseSRV;
	TComPtr<ID3D11ShaderResourceView> NormalSRV;
	TComPtr<ID3D11ShaderResourceView> MSRSRV;
	TComPtr<ID3D11RenderTargetView> DiffuseRTV;
	TComPtr<ID3D11RenderTargetView> NormalRTV;
	TComPtr<ID3D11RenderTargetView> MSRRTV;
	

};
