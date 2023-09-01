#pragma once
/* =============================================== */
// 
//	정 : 안철민
//	부 :
//
/* =============================================== */

#include "Model.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	explicit CBone();
	explicit CBone(const CBone& rhs);
	virtual ~CBone() = default;

public:
	// 뼈의 이름 반환
	const _tchar* Get_Name() const { return m_szName; }
	// 뼈의 최종 상태 행렬 반환.
	_float4x4			Get_CombinedTransformationMatrix() const { return m_CombinedTransformationMatrix; }
	const _float4x4* Get_CombinedTransformationMatrixPtr() const { return &m_CombinedTransformationMatrix; }
	_float4x4			Get_OffsetMatrix() const { return m_OffsetMatrix; }
	_float3			Get_TransformationMatrix_Position() { return _float3(m_TransformationMatrix._41, m_TransformationMatrix._42, m_TransformationMatrix._43); }
	_float3			Get_TransformationMatrix_Scale() {
		return _float3(XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_TransformationMatrix).r[0])),
			XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_TransformationMatrix).r[1])),
			XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_TransformationMatrix).r[2])));
	}
	_uint			Get_ParentNodeIndex() { return m_iParentIndex; }

	void				Set_OffsetMatrix(const _float4x4& OffsetMatrix) { m_OffsetMatrix = OffsetMatrix; }
	void				Set_TransformationMatrix(_float4x4 TransformationMatrix) { m_TransformationMatrix = TransformationMatrix; }

public:
	HRESULT Initialize(Engine::NODE Node);
	void	Invalidate_CombinedTransformationMatrix(const CModel::BONES& Bones);
	void	Invalidate_CombinedTransformationMatrix_Basic(const CModel::BONES& Bones);
	void	Reset_CombinedTransformationMatrix();

private:
	_tchar			m_szName[MAX_STR] = TEXT("");
	_float4x4		m_TransformationMatrix;
	_float4x4		m_CombinedTransformationMatrix;
	_float4x4		m_OffsetMatrix;

	_int			m_iParentIndex = { -1 };
	_uint			m_iIndex = { 0 };

public:
	static CBone* Create(Engine::NODE Node);
	CBone* Clone();
	virtual void Free() override;
};

END