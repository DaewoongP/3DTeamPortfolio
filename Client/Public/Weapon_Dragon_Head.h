#pragma once

/* =============================================== */
//	[CWeapon_Dragon_Head]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Parts.h"
#include "Client_Defines.h"

BEGIN(Client)

class CWeapon_Dragon_Head : public CParts
{
private:
	explicit CWeapon_Dragon_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_Dragon_Head(const CWeapon_Dragon_Head& rhs);
	virtual ~CWeapon_Dragon_Head() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CWeapon_Dragon_Head* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_Dragon_Head* Clone(void* pArg) override;
	virtual void Free() override;
};

END