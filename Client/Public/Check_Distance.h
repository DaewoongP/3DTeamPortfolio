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
	virtual HRESULT Tick_Behavior(const _float& fTimeDelta) override;

public:
	/* 
		1. ������ Ʈ������ ������
		2. �Ÿ��� ������(�̼��� �� �Ÿ��� üũ�մϴ�.)
		3. �Ÿ��� ���� ��� ��ǥ �Ÿ�
		4. ��ǥ �Ÿ����� ��������� Success�� ��ȯ�� ��� true, �־����� Success�� ��ȯ�� ��� false ����.
	*/
	void Set_Option(CTransform* pTransform, _bool isCompareDistance = false, const _float& fTargetDistance = 0.f, _bool isCloseDistance = false) {
		m_pOwnerTransform = pTransform;
		Safe_AddRef(m_pOwnerTransform);
		m_isCompareDistance = isCompareDistance;
		m_isCloseDistance = isCloseDistance;
		m_fTargetDistance = fTargetDistance;
	}

private:
	_bool m_isCompareDistance = { false };	// Ÿ�ٰ��� �Ÿ��� ��ǥ�Ÿ��� ���� ������.
	_bool m_isCloseDistance = { false };	// Ÿ�ٰ� �Ÿ��� ����� �� success�� ��ȯ���� �־��� �� ��ȯ�� �� �����ϴ� ����

	_float m_fTargetDistance = { 0.f };

	CTransform* m_pOwnerTransform = { nullptr };

public:
	static CCheck_Distance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCheck_Distance* Clone(void* pArg) override;
	virtual void Free() override;
};

END