#pragma once

#include "StateMachine.h"

BEGIN(Client)

class CHover_Move :
    public CStateMachine
{

private:
    explicit CHover_Move(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CHover_Move(const CHover_Move& rhs);
    virtual ~CHover_Move() = default;

public:
    //Get,Set

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
public:
    virtual void OnStateEnter(void* _pArg = nullptr);

    virtual void OnStateTick();
    virtual void OnStateExit();

    virtual void Bind_Notify();

    void LookFront();

    void Go_Idle();

    void Switch_MoveType();


public:
    static CHover_Move* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END

