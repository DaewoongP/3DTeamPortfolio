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
    typedef struct tagStateContextDesc
    {
        CModel* pOwnerModel = { nullptr };
        _float* pOwnerLookAngle = { nullptr };
    }STATECONTEXTDESC;
private:
    explicit CStateContext(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CStateContext(const CStateContext& rhs);
    virtual ~CStateContext() = default;

public:
    //Get,Set

    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);

private:
    CModel* m_pOwnerModel = { nullptr };
    _float* m_pOwnerLookAngle = { nullptr };

    CStateMachine* m_pCurrentStateMachine = { nullptr };

    unordered_map<const _tchar*, CStateMachine*> m_pStateMachines;

private:
    HRESULT Set_StateMachine(const _tchar* _pTag);

    CStateMachine* Find_StateMachine(const _tchar* _pTag);

    void Add_StateMachine(const _tchar* _pTag, CStateMachine* _pState);

public:
    static CStateContext* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END