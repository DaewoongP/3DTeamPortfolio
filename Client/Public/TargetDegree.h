#pragma once

/* =============================================== */
//	[CTargetDegree]
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

class CTargetDegree final : public CBehavior
{
private:
	explicit CTargetDegree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTargetDegree(const CTargetDegree& rhs);
	virtual ~CTargetDegree() = default;

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
	static CTargetDegree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CTargetDegree* Clone(void* pArg) override;
	virtual void Free() override;
};

END