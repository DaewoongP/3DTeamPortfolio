#pragma once
#include "Composite.h"

BEGIN(Engine)

class CModel;

END

BEGIN(Client)

class CStateMachine;

class CStateContext :
    public CComposite
{
public:
public:

private:
    explicit CStateContext(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CStateContext(const CStateContext& rhs);
    virtual ~CStateContext() = default;

public:
    //Get,Set
    const _tchar* Get_PreStateKey() const { return m_wszPreStateKey; }
    const _tchar* Get_CurrentStateKey() const { return m_wszPreStateKey; }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);

public:
    HRESULT Set_StateMachine(const _tchar* _pTag, void * _pArg = nullptr);

    HRESULT Add_StateMachine(LEVELID _eLevelID, const _tchar* _ComponentTag, const _tchar* _StateTag, const _tchar* _ProtoTag, void* _pArg);

    _bool Is_Current_State(const _tchar* _pTag);

private:
    CStateMachine* m_pCurrentStateMachine = { nullptr };//À¯
    
    unordered_map<const _tchar*, CStateMachine*> m_pStateMachines;//À¯


    unordered_map<const _tchar*, CStateMachine*>::iterator m_iterCurrentStateMachines = { m_pStateMachines.end() };

    _tchar m_wszPreStateKey[MAX_PATH] = {};//À¯

private:

private:
    CStateMachine* Find_StateMachine(const _tchar* _pTag);


    HRESULT Ready_StateMachine();

public:
    static CStateContext* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END