#include "Parts.h"

CParts::CParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CParts::CParts(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CParts::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CParts::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("[CParts] Failed to Initialize (pArg is nullptr)");
		return E_FAIL;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_ParentMatrixDesc = *static_cast<PARENTMATRIXDESC*>(pArg);

	return S_OK;
}

void CParts::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CParts::Late_Tick(_float fTimeDelta)
{
	_float4x4 BoneMatrix;

	/* �θ�(���� ������ * ���� �Ĺ��ε� * �ǹ�) */
	BoneMatrix = m_ParentMatrixDesc.OffsetMatrix *
		(*m_ParentMatrixDesc.pCombindTransformationMatrix) *
		m_ParentMatrixDesc.PivotMatrix;

	// ������ ����.
	BoneMatrix.Right(XMVector3Normalize(BoneMatrix.Right()));
	BoneMatrix.Up(XMVector3Normalize(BoneMatrix.Up()));
	BoneMatrix.Look(XMVector3Normalize(BoneMatrix.Look()));

	// ������� * �θ��� ����� * �θ��� �������
	_float4x4 CombinedWorldMatrix = BoneMatrix * (*m_ParentMatrixDesc.pParentWorldMatrix);
	m_pTransform->Set_WorldMatrix(CombinedWorldMatrix);

	__super::Late_Tick(fTimeDelta);
}

HRESULT CParts::Render()
{
	return __super::Render();
}

void CParts::Free()
{
	__super::Free();
}
