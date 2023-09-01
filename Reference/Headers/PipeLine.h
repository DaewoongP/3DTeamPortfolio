#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
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
	// ī�޶󿡼� ��, ���� ��� ����
	void Set_Transform(D3DTRANSFORMSTATE eTransformState, _float4x4 TransformStateMatrix);
	// ī�޶󿡼� Far �� ����.
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