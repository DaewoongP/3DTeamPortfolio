#pragma once

/* =============================================== */
//	[CTurn]
//	: Ÿ���� ���� Y�� �������� ȸ�� �Ѵ�.
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
class CCheck_Degree;
END

BEGIN(Client)

class CTurn final : public CBehavior
{
private:
	explicit CTurn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTurn(const CTurn& rhs);
	virtual ~CTurn() = default;

public:
	/*	1. ������ Ʈ������ ������
		2. ƽ �� ȸ���ӵ�
		3. ��ǥ ȸ�� �� */
	void Set_Option(CTransform* pTransform, const _float& _fTickPerDegree = 1.f, const _float& _fTargetDegree = 0.f) {
		m_pOwnerTransform = pTransform;
		Safe_AddRef(m_pOwnerTransform);
		m_fTickPerDegree = _fTickPerDegree;
		m_fTargetDegree = _fTargetDegree;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta) override;

private:
	_float m_fTickPerDegree = { 0.f };
	_float m_fTargetDegree = { 0.f };

private:
	CCheck_Degree* m_pCheckDegree = { nullptr };
	CTransform* m_pOwnerTransform = { nullptr };

private:
	virtual HRESULT Assemble_Childs() override;

public:
	static CTurn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CTurn* Clone(void* pArg) override;
	virtual void Free() override;
};

END