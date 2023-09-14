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

public:
    static CIdleState* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END