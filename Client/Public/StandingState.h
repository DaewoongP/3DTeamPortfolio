#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CStandingState :
    public CStateMachine
{

private:
    explicit CStandingState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CStandingState(const CStandingState& rhs);
    virtual ~CStandingState() = default;

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
    void Go_Idle();

public:
    static CStandingState* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END