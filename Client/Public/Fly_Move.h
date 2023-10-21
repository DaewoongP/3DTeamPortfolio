#pragma once

#include "StateMachine.h"

BEGIN(Client)

class CFly_Move :
    public CStateMachine
{

private:
    explicit CFly_Move(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CFly_Move(const CFly_Move& rhs);
    virtual ~CFly_Move() = default;

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

private:
    _tchar m_wszPostAnimationTag[MAX_PATH] = {};


public:
    static CFly_Move* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END

