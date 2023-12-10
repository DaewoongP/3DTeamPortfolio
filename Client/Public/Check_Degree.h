#pragma once

/* =============================================== */
//	[CCheck_Degree]
//	: Ÿ�ٰ� �ڽ��� ������ Look ���� �������� ������������ ����ϴ� Ŭ����
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CCheck_Degree final : public CBehavior
{
private:
	explicit CCheck_Degree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCheck_Degree(const CCheck_Degree& rhs);
	virtual ~CCheck_Degree() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick_Behavior(const _float& fTimeDelta) override;

public:
	void Set_Option(CTransform* pTransform) {
		m_pOwnerTransform = pTransform;
		Safe_AddRef(m_pOwnerTransform);
	}

private:
	CTransform* m_pOwnerTransform = { nullptr };

public:
	static CCheck_Degree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCheck_Degree* Clone(void* pArg) override;
	virtual void Free() override;
};

END