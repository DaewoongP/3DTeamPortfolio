#pragma once
#include "StateMachine_Enemy.h"

BEGIN(Client)

class CPensive_Charge_Attack :
    public CStateMachine_Enemy
{
    
private:
    explicit CPensive_Charge_Attack(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CPensive_Charge_Attack(const CPensive_Charge_Attack& rhs);
    virtual ~CPensive_Charge_Attack() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
public:
    virtual void OnStateEnter(void* _pArg = nullptr);
    virtual void OnStateTick();
    virtual void OnStateExit();

private:
    _float m_fAction_Change_Duration{ 0.0f };

private:
    void Action_None_Tick();
    void Bind_Notify();

public:
    static CPensive_Charge_Attack* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END