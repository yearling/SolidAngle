#pragma once
#include "YYUTApplication.h"
#include <memory>
#include "Core.h"
#include "YScene.h"
#include "YRenderInterface.h"
#include "SWorld.h"
class FBXReader;
class FirstPersionCamera;
class ICamera;
class RenderScene;
DECLARE_LOG_CATEGORY_EXTERN(YYDX11, Log, All);
class DX11Demo :public YYUTApplication
{
public:
	DX11Demo();
	~DX11Demo();
	virtual void					Initial(void);
	virtual int						Run(void);
	virtual void					Exit(void);
	virtual void					GameMain(void);
	virtual void					Update(float ElapseTime);
	virtual void					Render(float ElapseTime);
	void							PostRender();
	void							OnResize(void);
	void							OnMinimize(void);
	float							GetFPS() const;
protected:
	virtual LRESULT					MyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) throw();
	bool							m_bResize;
	int								m_width;
	int								m_height;
	void							SwichCamera();
	void							RayCastTriangleInfo();
private:
	FirstPersionCamera*				m_pCamera;
	FirstPersionCamera*				m_pLightCamera;
	ICamera*						m_pCurrentCamera;
	bool							m_bMouseLDown;
	bool							m_bMouseMDown;
	bool							m_bMouseRDown;
	XMFLOAT3						m_vVelocity;
	FVector							m_vVelocityF;
	POINT							m_LastMousePosition;
	FVector2D						CurrentMousePose;
	XMFLOAT2						m_vMouseDelta;
	float							m_fYawOrigin;
	float							m_fPitchOrigin;
	bool							m_bSwitchButtonDown;
	bool							m_bInit;
	bool                            m_bShowColorLayer;
	int64							m_LastTickCycles;
	double							m_LastFrameTime;
	float							m_FPS;
	int32							m_LastSecondFrames;
	double							m_LastSecond;
	bool							bRayCastTrianglePerFrame;
private:
	//std::shared_ptr<DepthPeeling::ObjPrimitive> m_spObj;
	std::shared_ptr<RenderScene>		m_pSceneRender;
	TRefCountPtr<YScene>            MainScene;
	TRefCountPtr<SWorld>	        World;
	TUniquePtr<IYRenderInterface>    MainRender;
	TUniquePtr<IYRenderInterface>    MainRenderDeffer;
    bool                            UseDefferedRender=false;
};