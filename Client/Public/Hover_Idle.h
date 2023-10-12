#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CHover_Idle :
    public CStateMachine
{
    
private:
    explicit CHover_Idle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CHover_Idle(const CHover_Idle& rhs);
    virtual ~CHover_Idle() = default;

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
    void Go_Start();
    void ActionType_Change();

public:
    static CHover_Idle* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END