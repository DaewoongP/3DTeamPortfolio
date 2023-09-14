#pragma once
#include "Composite.h"

BEGIN(Engine)

class CModel;

END

BEGIN(Client)

class CStateMachine :
    public CComposite
{
    //컴포넌트니까 전부 오너를 가질 수 있다.
    //오너의 모델을 가지고 있다가 변경한다.
protected:
    enum LOOKANGLE
    {
        L45,
        L90,
        L135,
        L180, 
        R45,
        R90,
        R135,
        R180
    };
public:

protected:
    explicit CStateMachine(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CStateMachine(const CStateMachine& rhs);
    virtual ~CStateMachine() = default;

public:
    //Get,Set

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
public:
    virtual void OnStateEnter() {};
    virtual void OnStateTick()  {};
    virtual void OnStateExit()  {};
public:
    void Set_OwnerModel(CModel* _pOwnerModel);
    void Set_OwnerLookAngle(_float* _pOwnerLookAngle);

protected:
    CModel* m_pOwnerModel = { nullptr };
    _float* m_pOwnerLookAngle = { nullptr };
    
    
public:
    static CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END