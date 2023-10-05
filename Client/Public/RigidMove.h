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
private:
	explicit CRigidMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRigidMove(const CRigidMove& rhs);
	virtual ~CRigidMove() = default;

public:
	/* 
		1. ���� ������ٵ� ������
		2. ���� �ø��� �����Ƽ ��
		3. �� �� ���� �ö󰡴� �� 
	*/
	void Set_Option(CRigidBody* pRigidBody, const _float3& _vForce, const _float& fTime) {
		m_pOwnerRigidBody = pRigidBody;
		Safe_AddRef(m_pOwnerRigidBody);
		m_vForce = _vForce;
		m_fTime = fTime;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta) override;

private:
	_float3 m_vForce;
	_float m_fTime = { 0.f };
	_float m_fTimeAcc = { 0.f };
	_float3 m_vLockPosition;
	_bool m_isFirst = { true };

private:
	CRigidBody* m_pOwnerRigidBody = { nullptr };

private:
	virtual void Reset_Behavior(HRESULT result) override;

public:
	static CRigidMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRigidMove* Clone(void* pArg) override;
	virtual void Free() override;
};

END