#pragma once
#include "YYUT.h"

class ICamera
{
public:
	ICamera();
	virtual							~ICamera();

	virtual void					FrameMove(float elapse_time) = 0;

	virtual void					Reset();
	virtual void					SetViewParam(const FVector &eye, const FVector &lookat);
	virtual void					SetProjParam(float FOV, float aspect, float near_plane, float far_plane);
	void							SetWindow(int width, int height);
	void							AddPitchYaw(float pitch, float yaw);
	void							SetVelocity(FVector elocity);
	float							GetPitch() const { return m_fCameraPitchAngle; }
	float							GetYaw() const { return m_fCameraYawAngle; }
	void							SetPitch(float pitch) { m_fCameraPitchAngle = pitch; }
	void							SetYaw(float yaw) { m_fCameraYawAngle = yaw; }
	FVector							GetCameraPos() const { return m_vEye; }
public:
	FMatrix							GetView() const;
	FMatrix							GetViewInverse() const;
	FMatrix							GetProject() const;
	FMatrix							GetProjInv() const;
	FMatrix							GetViewProject() const;
	FMatrix							GetViewProjInv() const;
	FVector							GetEyePt() const;
	FVector							GetDir() const;

	float							GetNearClip() const;
	float							GetFarClip() const;


protected:
	FMatrix							m_matView;
	FMatrix							m_matProjection;
	FVector							m_vDir;
	FVector2D						m_vMouseDelata;
	FVector							m_vEye;
	FVector							m_vLookat;
	float							m_fCameraYawAngle;
	float							m_fCameraPitchAngle;
	FVector							m_vVelocity;
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