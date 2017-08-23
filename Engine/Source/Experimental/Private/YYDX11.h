#pragma once
#include "YYUTApplication.h"
#include <memory>
class FBXReader;
class FirstPersionCamera;
class ICamera;
class RenderScene;
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
	virtual void					Render();
	void							OnResize(void);
	void							OnMinimize(void);
	void							RenderShadowMap(void);
protected:
	virtual LRESULT					MyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) throw();
	bool							m_bResize;
	int								m_width;
	int								m_height;
	void							SwichCamera();
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
	XMFLOAT2						m_vMouseDelta;
	float							m_fYawOrigin;
	float							m_fPitchOrigin;
	bool							m_bSwitchButtonDown;
	bool							m_bInit;
	bool                            m_bShowColorLayer;
	int64							m_LastTickCycles;
	double							m_LastFrameTime;
private:
	//std::shared_ptr<DepthPeeling::ObjPrimitive> m_spObj;
	std::shared_ptr<RenderScene>		m_pRenderMesh;
	std::unique_ptr<class FBXReader>	m_pFbxReader;
};