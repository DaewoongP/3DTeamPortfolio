#pragma once
#include "StateMachine_Enemy.h"

BEGIN(Client)

class CPensive_Hit :
    public CStateMachine_Enemy
{
    
private:
    explicit CPensive_Hit(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CPensive_Hit(const CPensive_Hit& rhs);
    virtual ~CPensive_Hit() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
public:
    virtual void OnStateEnter(void* _pArg = nullptr);
    virtual void OnStateTick();
    virtual void OnStateExit();

private:
    _float m_fAction_Change_Duration{ 0.0f };

private:
    void Action_None_Tick();
    void Bind_Notify();

public:
    static CPensive_Hit* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END