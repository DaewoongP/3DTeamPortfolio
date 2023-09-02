#pragma once
/* =============================================== */
// 
//	정 : 박대웅
//	부 :
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)

public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

public:
	explicit CPipeLine();
	virtual ~CPipeLine() = default;

public:
	// 카메라에서 뷰, 투영 행렬 세팅
	void Set_Transform(D3DTRANSFORMSTATE eTransformState, _float4x4 TransformStateMatrix);
	// 카메라에서 Far 값 세팅.
	void Set_CameraFar(_float fCamFar) { m_fCameraFar = fCamFar; }

	const _float4x4* Get_TransformMatrix(D3DTRANSFORMSTATE eTransformState);
	const _float4x4* Get_TransformMatrix_Inverse(D3DTRANSFORMSTATE eTransformState);
	const _float4* Get_CamPosition();
	const _float* Get_CamFar();

public:
	void Tick();

private:
	_float4x4				m_TransformMatrix[D3DTS_END];
	_float4x4				m_TransformMatrix_Inverse[D3DTS_END];
	_float4					m_vCameraPos;
	_float					m_fCameraFar = { 0.f };

public:
	virtual void Free() override;
};

END