#pragma once
#include "Core.h"
#include "YRenderInterface.h"
#include "YScene.h"
#include "IShader.h"
DECLARE_LOG_CATEGORY_EXTERN(LogForwardRender,Log,All)


class YForwardRender:public IYRenderInterface
{
public:
	YForwardRender();
	~YForwardRender();
	void InitRenders();
	void RenderScene(TRefCountPtr<YScene>& Scene, TSharedRef<YRenderInfo> RenderInfo) override;
private:
	TUniquePtr<IYRenderPolicy> DirectionalLightRenderPolicy;
	float							m_ScreenWidth;
	float							m_ScreenHeight;
	TComPtr<ID3D11RasterizerState>	m_rs;
	TComPtr<ID3D11BlendState>		m_bs;
	TComPtr<ID3D11DepthStencilState>m_ds;
};
