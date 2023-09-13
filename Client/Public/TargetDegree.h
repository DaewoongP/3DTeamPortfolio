#pragma once

/* =============================================== */
//	[CTargetDegree]
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