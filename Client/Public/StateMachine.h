#pragma once
#include "Composite.h"

BEGIN(Engine)

class CModel;
class CTransform;

END

BEGIN(Client)

class CStateMachine :
    public CComposite
{
    //컴포넌트니까 전부 오너를 가질 수 있다.
    //오너의 모델을 가지고 있다가 변경한다.

public:

protected:
    explicit CStateMachine(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CStateMachine(const CStateMachine& rhs);
    virtual ~CStateMachine() = default;

public:
    //Get,Set
public:
    HRESULT Set_StateMachine(const _tchar* _pTag);
public:
    void Set_OwnerModel(CModel* _pOwnerModel);
    void Set_OwnerLookAngle(_float* _pOwnerLookAngle);
    void Set_IsDirectionKeyPressed(_bool* _pIsDirectionKeyPressed);
    void Set_PlayerTransform(CTransform* _pPlayerTransform);
    void Set_IsSprint(_bool* _pIsSprint) { m_pIsSprint = _pIsSprint; }
    void Set_ActionSwitch(_uint* _pIActionSwitch) { m_pIActionSwitch = _pIActionSwitch; }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);

public:
    virtual void OnStateEnter() {};
    virtual void OnStateTick()  {};
    virtual void OnStateExit()  {};

    virtual void Bind_Notify()  {};

protected:
    CModel* m_pOwnerModel = { nullptr };
    _float* m_pOwnerLookAngle = { nullptr };
    _bool* m_pIsDirectionKeyPressed = { nullptr };
    CTransform* m_pPlayerTransform = { nullptr };
    
    _float m_f45Angle = { XMConvertToRadians(45.0f) };
    _float m_f135Angle = { XMConvertToRadians(135.0f) };
    
    _bool* m_pIsSprint = { nullptr };
    _uint* m_pIActionSwitch = { nullptr };


public:
    static CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END