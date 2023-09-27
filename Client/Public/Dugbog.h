#pragma once
/* =============================================== */
//	[CDugbog]
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
END

BEGIN(Client)
class CRandom_Select;
END

BEGIN(Client)

class CDugbog final : public CEnemy
{
private:
	explicit CDugbog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDugbog(const CDugbog& rhs);
	virtual ~CDugbog() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;

private:
	_bool m_isAbleLevioso = { false };

private:
	virtual HRESULT Make_AI() override;
	virtual HRESULT Make_Notifies() override;
	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;

#ifdef _DEBUG
	_int m_iIndex = { 0 };
	void Tick_ImGui();
#endif // _DEBUG

private: /* 사망처리 전용 함수 */
	_float m_fDeadTimeAcc = { 0.f };
	void DeathBehavior(const _float& fTimeDelta);

private: /* 행동 묶음 */
	HRESULT Make_Idle_Break(_Inout_ CRandom_Select* pRandom_Select);
	HRESULT Make_Death(_Inout_ CSequence* pSequence);
	HRESULT Make_Alive(_Inout_ CSelector* pSelector);

	HRESULT Make_Hit_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Check_Spell(_Inout_ CSelector* pSelector);
	HRESULT Make_Run_Attack(_Inout_ CSelector* pSelector);
	HRESULT Make_Tongue_Attack(_Inout_ CSequence* pSequence);

	HRESULT Make_Turns(_Inout_ CSequence* pSequence);
	HRESULT Make_Turn_Runs(_Inout_ CSequence* pSequence);
	HRESULT Make_Levioso_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Air_Hit(_Inout_ CSelector* pSelector);
	HRESULT Make_Air_Hit_Tongue(_Inout_ CSequence* pSequence);

private: /* Notify Functions */
	void Enter_Light_Attack();
	void Enter_Heavy_Attack();
	void Enable_Levioso() {
		m_isAbleLevioso = true;
	}
	void Disable_Levioso() {
		m_isAbleLevioso = false;
	}
	void Exit_Attack();

public:
	static CDugbog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END