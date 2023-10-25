#pragma once
#include "Composite.h"

BEGIN(Client)
class CRacer;
END

BEGIN(Client)

// 비행 게임을 관리하는 하나의 관리자임.
// 게임 메니져는 움직이는 게임 오브젝트의 trans를 가져오고
// racer라는 객체를 만들어 racer가 trans를 따라가게 만들거임.
// racer는 공과 충돌이 가능하고
// 그로인한 점수 증가가 가능함

class CFlyGameManager :
    public CComposite
{
private:
    explicit CFlyGameManager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CFlyGameManager(const CFlyGameManager& rhs);
    virtual ~CFlyGameManager() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
    
private:
    HRESULT Add_Components();

private:
    vector<CRacer*>         m_pRacerGroup = {};
    vector<CGameObject*>    m_pRacerOwnerGroup = {};
    vector<_uint>           m_pScoreGroup = {};

private:
    //게임 진행시간임.
    _float                  m_fGameTimer = { 180.f };
    //게임 진행 여부임
    _bool                   m_isGameContinue = { false };

public:
    static CFlyGameManager* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END