#pragma once

/* =============================================== */
//	[CRigidMove]
//	: ���� �ߴ� �ൿ.
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CRigidBody;
END

BEGIN(Client)
class CCheck_Degree;
END

BEGIN(Client)

class CRigidMove final : public CBehavior
{
public:
	enum MOVEDIRECTION { DIR_RIGHT, DIR_UP, DIR_LOOK, DIR_END };

private:
	explicit CRigidMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRigidMove(const CRigidMove& rhs);
	virtual ~CRigidMove() = default;

public:
	/* 
		1. ���� ������ٵ� ������
		2. ���� Ʈ������ ������
		3. �̵� ����
		4. �̵� ��
		5. �� �� ���� �ö󰡴� �� 
	*/
	void Set_Option(CRigidBody* pRigidBody, CTransform* pTransform, MOVEDIRECTION eDirection, const _float& _fForce, const _float& fTime) {
		m_pOwnerRigidBody = pRigidBody;
		Safe_AddRef(m_pOwnerRigidBody);
		m_pOwnerTransform = pTransform;
		Safe_AddRef(m_pOwnerTransform);
		m_eDirection = eDirection;
		m_fForce = _fForce;
		m_fTime = fTime;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta) override;

private:
	MOVEDIRECTION m_eDirection = { DIR_END };
	_float m_fForce;
	_float m_fTime = { 0.f };

	_float m_fTimeAcc = { 0.f };

private:
	CRigidBody* m_pOwnerRigidBody = { nullptr };
	CTransform* m_pOwnerTransform = { nullptr };

private:
	virtual void Reset_Behavior(HRESULT result) override;

public:
	static CRigidMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRigidMove* Clone(void* pArg) override;
	virtual void Free() override;
};

END