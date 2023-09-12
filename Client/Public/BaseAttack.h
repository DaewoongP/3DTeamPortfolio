#pragma once

/* =============================================== */
//	[CMagic]
//	: 객체의 마법을 담당하는 클래스
//	정 : 안철민
//	부 :
//
/* =============================================== */

#include "Magic.h"
#include "Client_Defines.h"

BEGIN(Client)

class CBaseAttack final : public CMagic
{
private:
	explicit CBaseAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBaseAttack(const CBaseAttack& rhs);
	virtual ~CBaseAttack() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	virtual _bool Action_Magic(class CTransform* pTarget);

public:
	static CBaseAttack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBaseAttack* Clone(void* pArg) override;
	virtual void Free() override;
};

END