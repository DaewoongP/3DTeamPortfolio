#pragma once
#include "StateMachine_Enemy.h"

BEGIN(Client)

class CPensive_Appearence :
    public CStateMachine_Enemy
{
    
private:
    explicit CPensive_Appearence(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CPensive_Appearence(const CPensive_Appearence& rhs);
    virtual ~CPensive_Appearence() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
public:
    virtual void OnStateEnter(void* _pArg = nullptr);
    virtual void OnStateTick();
    virtual void OnStateExit();

public:
    static CPensive_Appearence* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END