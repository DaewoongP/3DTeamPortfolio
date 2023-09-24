#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CProtegoState :
    public CStateMachine
{

private:
    explicit CProtegoState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CProtegoState(const CProtegoState& rhs);
    virtual ~CProtegoState() = default;

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

private:
    void Go_Idle();

    void Stupefy();
    void Powerful_Stupefy();


public:
    static CProtegoState* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END