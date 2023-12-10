#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CBroom_Begin :
    public CStateMachine
{
private:
    explicit CBroom_Begin(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CBroom_Begin(const CBroom_Begin& rhs);
    virtual ~CBroom_Begin() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
public:
    virtual void OnStateEnter(void* _pArg = nullptr);

    virtual void OnStateTick();
    virtual void OnStateExit();

    void Go_Hover_Idle();
    void Go_Move();

private:
    _bool m_isAppearTrigger = { false };
    _float m_fTimeAcc = { 0.f };

public:
    static CBroom_Begin* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END