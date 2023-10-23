#pragma once
/* =============================================== */
//	[CEvent_Dragon_Death]
//  : 드래곤이 죽었을때 호출하는 클래스
//  생텀 레벨 에서만 사용해야 함.
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CCard_Fig;
class CConjuredDragon;
END

BEGIN(Client)

class CEvent_Dragon_Death final : public CGameObject
{
private:
	explicit CEvent_Dragon_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Dragon_Death(const CEvent_Dragon_Death& rhs);
	virtual ~CEvent_Dragon_Death() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

private:
	CConjuredDragon* m_pDragon = { nullptr };
	CCard_Fig* m_pCard_Fig = { nullptr };

	_bool m_isCallEvent = { false };

public:
	static CEvent_Dragon_Death* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Dragon_Death* Clone(void* pArg) override;
	virtual void Free() override;
};

END