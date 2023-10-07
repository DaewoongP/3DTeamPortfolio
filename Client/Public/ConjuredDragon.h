#pragma once
/* =============================================== */
//	[CConjuredDragon]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CSequence;
class CSelector;
class CRandomChoose;
END

BEGIN(Client)

class CConjuredDragon final : public CEnemy
{
private:
	explicit CConjuredDragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CConjuredDragon(const CConjuredDragon& rhs);
	virtual ~CConjuredDragon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;

private:
	HRESULT Make_AI();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);

private: /* 사망처리 전용 함수 */
	_float m_fDeadTimeAcc = { 0.f };
	void DeathBehavior(const _float& fTimeDelta);

private: /* 행동 묶음들 */
	HRESULT Make_Death(_Inout_ CSequence* pSequence);
	HRESULT Make_Alive(_Inout_ CSelector* pSelector);



private: /* Notify Func */

public:
	static CConjuredDragon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CConjuredDragon* Clone(void* pArg) override;
	virtual void Free() override;
};

END