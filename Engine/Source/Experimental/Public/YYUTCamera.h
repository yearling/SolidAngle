#pragma once
#include "YYUT.h"

class ICamera
{
public:
	ICamera();
	virtual							~ICamera();

	virtual void					FrameMove(float elapse_time) = 0;

	virtual void					Reset();
	virtual void					SetViewParam(const XMFLOAT3 &eye,const XMFLOAT3 &lookat);
	virtual void					SetProjParam(float FOV,float aspect,float near_plane,float far_plane);
	void							SetWindow(int width, int height);
	void							AddPitchYaw(float pitch, float yaw);
	void							SetVelocity(const XMFLOAT3 &velocity);
	float							GetPitch() const { return m_fCameraPitchAngle; }
	float							GetYaw() const { return m_fCameraYawAngle; }
	void							SetPitch(float pitch) { m_fCameraPitchAngle = pitch; }
	void							SetYaw(float yaw) { m_fCameraYawAngle = yaw; }
public:
	const XMMATRIX					GetView() const;
	const XMMATRIX					GetViewInverse() const;
	const XMMATRIX					GetProject() const;
	const XMMATRIX					GetProjInv() const;
	const XMMATRIX					GetViewProject() const;
	const XMMATRIX					GetViewProjInv() const;
	const XMVECTOR					GetEyePt() const;
	const XMVECTOR					GetDir() const;

	float							GetNearClip() const;
	float							GetFarClip() const;


protected:
	XMFLOAT4X4						m_matView;
	XMFLOAT4X4						m_matProjection;
	XMFLOAT3						m_vDir;
	XMFLOAT2						m_vMouseDelata;
	XMFLOAT3						m_vEye;
	XMFLOAT3						m_vLookat;
	float							m_fCameraYawAngle;
	float							m_fCameraPitchAngle;
	XMFLOAT3						m_vVelocity;
	float							m_fFov;
	float							m_fAspect;
	float							m_fNearPlane;
	float							m_fFarPlane;
	UINT							m_Width;
	UINT							m_Height;
};

class FirstPersionCamera :public ICamera
{
public:
	FirstPersionCamera();
	virtual							~FirstPersionCamera();

	virtual void					FrameMove(float elapse_time);
};