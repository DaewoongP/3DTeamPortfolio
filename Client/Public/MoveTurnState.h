#pragma once

#include "StateMachine.h"

BEGIN(Client)

class CMoveTurnState :
    public CStateMachine
{

private:
    explicit CMoveTurnState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CMoveTurnState(const CMoveTurnState& rhs);
    virtual ~CMoveTurnState() = default;

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


private:

private:
    virtual void Bind_Notify() override;

    void Go_Idle();

    void Go_Start();

    void LookFront();




public:
    static CMoveTurnState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END

