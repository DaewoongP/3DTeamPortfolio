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
}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eTransformState, _float4x4 TransformStateMatrix)
{
	m_TransformMatrix[eTransformState] = TransformStateMatrix;
}

_float4x4* CPipeLine::Get_TransformMatrix(D3DTRANSFORMSTATE eTransformState)
{	
	return &m_TransformMatrix[eTransformState];
}

_float4x4* CPipeLine::Get_TransformMatrix_Inverse(D3DTRANSFORMSTATE eTransformState)
{
	return &m_TransformMatrix_Inverse[eTransformState];
}

_float4* CPipeLine::Get_CamPosition()
{
	return &m_vCameraPos;
}

_float* CPipeLine::Get_CamFar()
{
	return &m_fCameraFar;
}

void CPipeLine::Tick()
{
	for (_uint i = 0; i < D3DTS_END; i++)
	{
		m_TransformMatrix_Inverse[i] = XMMatrixInverse(nullptr, *Get_TransformMatrix(static_cast<D3DTRANSFORMSTATE>(i)));
	}
	
	memcpy(&m_vCameraPos, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[3][0], sizeof m_vCameraPos);
}

void CPipeLine::Free()
{
}
