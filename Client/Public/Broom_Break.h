#pragma once
#include "StateMachine.h"

BEGIN(Client)

class CBroom_Break :
    public CStateMachine
{
private:
    explicit CBroom_Break(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CBroom_Break(const CBroom_Break& rhs);
    virtual ~CBroom_Break() = default;

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
    _uint m_iBasicSpellCombo = { 0 };
    _bool* m_pisReadySpell = { nullptr };
    _float m_fFixAngleSpeed = { 0.0f };

    _uint m_iBasicSpellRandom{ 0 };

    _bool m_isLastBasicSpellBack = { false };

    vector<wstring> m_vecSpellActionTextList;

private:
    void Go_Idle();


public:
    static CBroom_Break* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END