#pragma once
#include "StateMachine.h"


BEGIN(Client)
class CCraftState : public CStateMachine
{
private:
    explicit CCraftState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CCraftState(const CCraftState& rhs);
    virtual ~CCraftState() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);

public:
    virtual void OnStateEnter(void* _pArg = nullptr);
    virtual void OnStateTick();
    virtual void OnStateExit();

public:
    static CCraftState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END