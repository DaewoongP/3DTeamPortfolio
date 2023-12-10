#pragma once
#include "StateMachine_Enemy.h"

BEGIN(Client)

class CPensive_Idle :
    public CStateMachine_Enemy
{
    
private:
    explicit CPensive_Idle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CPensive_Idle(const CPensive_Idle& rhs);
    virtual ~CPensive_Idle() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
public:
    virtual void OnStateEnter(void* _pArg = nullptr);
    virtual void OnStateTick(_float fTimeDelta);
    virtual void OnStateExit();

private:
    _float m_fMinChangeTimer = { 0.f };
    _float m_fMaxChangeTimer = { 1.f };
    _float m_fActionTimer    = { 0.0f };
    _float m_fActionSetTimer = { 0.0f };
    _uint  m_iActionIndex = { 0 };

public:
    static CPensive_Idle* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END