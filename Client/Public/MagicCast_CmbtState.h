#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CHardLandState :
    public CStateMachine
{

private:
    explicit CHardLandState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CHardLandState(const CHardLandState& rhs);
    virtual ~CHardLandState() = default;

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

private:
    void Go_Idle();

    void Go_Loop();

public:
    static CHardLandState* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END