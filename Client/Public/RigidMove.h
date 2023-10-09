#pragma once

/* =============================================== */
//	[CRigidMove]
//	: 위로 뜨는 행동.
//	정 : 주성환
//	부 :
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
		1. 본인 리지드바디 포인터
		2. 본인 트랜스폼 포인터
		3. 이동 방향
		4. 이동 힘
		5. 몇 초 까지 올라가는 지 
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