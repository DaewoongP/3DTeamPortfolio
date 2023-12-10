#pragma once
/* =============================================== */
//	[CEvent_Dragon_Hp_Down]
//  : �巡�� ü���� 30�۰� ���� �� ȣ���ϴ� Ŭ����
//  ���� ���� ������ ����ؾ� ��.
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CCard_Fig;
class CConjuredDragon;
END

BEGIN(Client)

class CEvent_Dragon_Hp_Down final : public CGameObject
{
private:
	explicit CEvent_Dragon_Hp_Down(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEvent_Dragon_Hp_Down(const CEvent_Dragon_Hp_Down& rhs);
	virtual ~CEvent_Dragon_Hp_Down() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

private:
	CConjuredDragon* m_pDragon = { nullptr };
	CCard_Fig* m_pCard_Fig = { nullptr };

	_bool m_isCallEvent = { false };

public:
	static CEvent_Dragon_Hp_Down* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CEvent_Dragon_Hp_Down* Clone(void* pArg) override;
	virtual void Free() override;
};

END