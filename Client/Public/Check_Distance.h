#pragma once

/* =============================================== */
//	[CCheck_Distance]
//	: 타겟과 자신의 각도와 Look 기준 왼쪽인지 오른쪽인지를 계산하는 클래스
//	정 : 주성환
//	부 :
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
		1. 본인의 트랜스폼 포인터
		2. 거리를 비교할지(미설정 시 거리만 체크합니다.)
		3. 거리를 비교할 경우 목표 거리
		4. 목표 거리보다 가까워질때 Success를 반환할 경우 true, 멀어질때 Success를 반환할 경우 false 설정.
	*/
	void Set_Option(CTransform* pTransform, _bool isCompareDistance = false, const _float& fTargetDistance = 0.f, _bool isCloseDistance = false) {
		m_pOwnerTransform = pTransform;
		Safe_AddRef(m_pOwnerTransform);
		m_isCompareDistance = isCompareDistance;
		m_isCloseDistance = isCloseDistance;
		m_fTargetDistance = fTargetDistance;
	}

private:
	_bool m_isCompareDistance = { false };	// 타겟과의 거리를 목표거리와 비교할 것인지.
	_bool m_isCloseDistance = { false };	// 타겟과 거리가 가까울 때 success를 반환할지 멀어질 때 반환할 지 결정하는 변수

	_float m_fTargetDistance = { 0.f };

	CTransform* m_pOwnerTransform = { nullptr };

public:
	static CCheck_Distance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCheck_Distance* Clone(void* pArg) override;
	virtual void Free() override;
};

END