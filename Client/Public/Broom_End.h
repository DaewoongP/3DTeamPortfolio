#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CBroom_End :
    public CStateMachine
{

private:
    explicit CBroom_End(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CBroom_End(const CBroom_End& rhs);
    virtual ~CBroom_End() = default;

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

    void Go_Loop();

public:
    static CBroom_End* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END