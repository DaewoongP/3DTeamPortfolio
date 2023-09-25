#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CIdleState :
    public CStateMachine
{

private:
    explicit CIdleState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CIdleState(const CIdleState& rhs);
    virtual ~CIdleState() = default;

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
    void Action_Casual_Tick();
    void Action_Cmbt_Tick();

    void Bind_Notify();

    void Go_Turn();

    void Go_Start();

    void Go_Roll();

    void Go_Jump();

    void Go_Magic_Cast();

    void ActionType_Change();

public:
    static CIdleState* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END