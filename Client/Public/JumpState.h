#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CJumpState :
    public CStateMachine
{

private:
    explicit CJumpState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CJumpState(const CJumpState& rhs);
    virtual ~CJumpState() = default;

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
    _float m_fJumpMoveSpeed = { 0.0f };

private:
    void Go_Idle();

    void Go_Loop();

    void Go_HardLand();

    void LookFront();

    void JumpMoveSpeed();

public:
    static CJumpState* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END