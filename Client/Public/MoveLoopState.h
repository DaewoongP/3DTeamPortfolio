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
    virtual void OnStateEnter();
    virtual void OnStateTick();
    virtual void OnStateExit();

    void LookFront();

    void Over_135();

    void Go_Idle();

    void Go_Roll();

    void Go_Jump();

    void Switch_Joging_Sprint();

    void Go_Magic_Cast();


public:
    static CMoveLoopState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END

