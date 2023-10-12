#pragma once

#include "StateMachine.h"

BEGIN(Client)

class CHover_Turn :
    public CStateMachine
{

private:
    explicit CHover_Turn(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CHover_Turn(const CHover_Turn& rhs);
    virtual ~CHover_Turn() = default;

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
    static CHover_Turn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END

