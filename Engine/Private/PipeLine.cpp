#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
	for (_uint i = 0; i < D3DTS_END; ++i)
	{
		m_TransformMatrix[i] = XMMatrixIdentity();
		m_TransformMatrix_Inverse[i] = XMMatrixIdentity();
	}

	ZEROMEM(&m_vCameraPos);
	ZEROMEM(&m_vCameraUp);
}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eTransformState, _float4x4 TransformStateMatrix)
{
	m_TransformMatrix[eTransformState] = TransformStateMatrix;
}

const _float4x4* CPipeLine::Get_TransformMatrix(D3DTRANSFORMSTATE eTransformState)
{	
	return &m_TransformMatrix[eTransformState];
}

const _float4x4* CPipeLine::Get_TransformMatrix_Inverse(D3DTRANSFORMSTATE eTransformState)
{
	return &m_TransformMatrix_Inverse[eTransformState];
}

const _float4* CPipeLine::Get_CamPosition()
{
	return &m_vCameraPos;
}

const _float3* CPipeLine::Get_CamUp()
{
	return &m_vCameraUp;
}

const _float3* CPipeLine::Get_CamLook()
{
	return &m_vCameraLook;
}

const _float3* CPipeLine::Get_CamRight()
{
	return &m_vCameraRight;
}

const _float* CPipeLine::Get_CamFar()
{
	return &m_fCameraFar;
}

void CPipeLine::Tick()
{
	std::lock_guard<std::mutex> lock(mtx);

	for (_uint i = 0; i < D3DTS_END; i++)
	{
		m_TransformMatrix_Inverse[i] = XMMatrixInverse(nullptr, *Get_TransformMatrix(static_cast<D3DTRANSFORMSTATE>(i)));
	}

	memcpy(&m_vCameraRight, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[0][0], sizeof m_vCameraRight);
	memcpy(&m_vCameraUp, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[1][0], sizeof m_vCameraUp);
	memcpy(&m_vCameraLook, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[2][0], sizeof m_vCameraLook);
	memcpy(&m_vCameraPos, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[3][0], sizeof m_vCameraPos);
}

void CPipeLine::Free()
{
}
