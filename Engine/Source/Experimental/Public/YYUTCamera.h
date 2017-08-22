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
	virtual void					SetViewParamF(const FVector &eye,const FVector &lookat);
	virtual void					SetProjParam(float FOV,float aspect,float near_plane,float far_plane);
	virtual void					SetProjParamF(float FOV,float aspect,float near_plane,float far_plane);
	void							SetWindow(int width, int height);
	void							AddPitchYaw(float pitch, float yaw);
	void							SetVelocity(const XMFLOAT3 &velocity);
	float							GetPitch() const { return m_fCameraPitchAngle; }
	float							GetYaw() const { return m_fCameraYawAngle; }
	void							SetPitch(float pitch) { m_fCameraPitchAngle = pitch; }
	void							SetYaw(float yaw) { m_fCameraYawAngle = yaw; }
public:
	const XMMATRIX					GetView() const;
	FMatrix							GetViewF() const;
	const XMMATRIX					GetViewInverse() const;
	FMatrix							GetViewInverseF() const;
	const XMMATRIX					GetProject() const;
	FMatrix							GetProjectF() const;
	const XMMATRIX					GetProjInv() const;
	FMatrix							GetProjInvF() const;
	const XMMATRIX					GetViewProject() const;
	FMatrix							GetViewProjectF() const;
	const XMMATRIX					GetViewProjInv() const;
	FMatrix							GetViewProjInvF() const;
	const XMVECTOR					GetEyePt() const;
	FVector							GetEyePtF() const;
	const XMVECTOR					GetDir() const;
	FVector							GetDirF() const;

	float							GetNearClip() const;
	float							GetFarClip() const;


protected:
	XMFLOAT4X4						m_matView;
	FMatrix							m_matViewF;
	XMFLOAT4X4						m_matProjection;
	FMatrix							m_matProjectionF;
	XMFLOAT3						m_vDir;
	FVector							m_vDirF;
	XMFLOAT2						m_vMouseDelata;
	FVector2D						m_vMouseDelataF;
	XMFLOAT3						m_vEye;
	FVector							m_vEyeF;
	XMFLOAT3						m_vLookat;
	FVector							m_vLookatF;
	float							m_fCameraYawAngle;
	float							m_fCameraPitchAngle;
	XMFLOAT3						m_vVelocity;
	FVector							m_vVelocityF;
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