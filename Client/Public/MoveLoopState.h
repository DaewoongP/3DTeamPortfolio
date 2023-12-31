#pragma once

#include "StateMachine.h"

BEGIN(Client)

class CMoveLoopState :
    public CStateMachine
{

private:
    explicit CMoveLoopState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CMoveLoopState(const CMoveLoopState& rhs);
    virtual ~CMoveLoopState() = default;

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
    static CMoveLoopState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END

