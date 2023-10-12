#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CUseItemState :
    public CStateMachine
{
public:
    typedef struct tagUseItemDesc
    {
        ITEM_ID eItem_Id = { ITEM_ID_END };
        function<void()> funcPotion = { nullptr };
    }USEITEMDESC;

private:
    explicit CUseItemState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CUseItemState(const CUseItemState& rhs);
    virtual ~CUseItemState() = default;

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
    void Go_Idle();

public:
    static CUseItemState* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END