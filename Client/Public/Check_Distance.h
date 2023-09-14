#pragma once

/* =============================================== */
//	[CCheck_Distance]
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

class CCheck_Distance final : public CBehavior
{
private:
	explicit CCheck_Distance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCheck_Distance(const CCheck_Distance& rhs);
	virtual ~CCheck_Distance() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta) override;

public:
	void Set_Transform(CTransform* pTransform) {
		m_pOwnerTransform = pTransform;
		Safe_AddRef(m_pOwnerTransform);
	}

private:
	CTransform* m_pOwnerTransform = { nullptr };

public:
	static CCheck_Distance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCheck_Distance* Clone(void* pArg) override;
	virtual void Free() override;
};

END