#pragma once
#include "Composite.h"

BEGIN(Engine)

class CHealth;

END

BEGIN(Client)

class CUI_Group_HP;
class CUI_Group_Finisher;
class CInventory;

class CPlayer_Information :
    public CComposite
{
public:
    enum PLAYER_ITEM
    {
        HEALING_POTION,         //힐 포션
        DEFENSIVE_POWER_UP,     //방어력 포션
        ATTACK_POWER_UP,        //공격력 포션
        SHOW_TIME,              //쿨타임 포션
        THUNDER_CLOUD,          //번개구름 생성 포션
        INVISIBILITY_PILL,      //투명해지는 포션
        MANDRAKE,               //맨드레이크
        BITE_CABBAGE,           //깨무는 양배추
        TENTACULAR,             //텐타큘라
        PLAYER_ITEM_END
    };

public:

private:
    explicit CPlayer_Information(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CPlayer_Information(const CPlayer_Information& rhs);
    virtual ~CPlayer_Information() = default;

public:
    //Get,Set

    //아이템 갯수 (enum 참고)
    _uint const * Get_ItemCountPtr(PLAYER_ITEM _eItem) const { return &m_vecItemCount[_eItem]; }
    //현제 채력 비율
    _float Get_Health_Percent() const { return m_pHealth->Get_Current_HP_Percent(); }
    //현제 피니셔 비율
    _float Get_Finisher_Percent() const { return m_pFinisher->Get_Current_HP_Percent(); }


public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);

public:
     //피니셔 사용 가능 여부
    _bool Is_Use_Fnisher();

    //피니셔 사용시 호출(게이지 감소)
    void Using_Fnisher();

    //피니셔 회복
    void Stack_Finisher() { m_pFinisher->Set_HP(m_pFinisher->Get_HP() + 1); }

    //체력 변경
    void fix_HP(_int _iNum);


private:
    //체력
    CHealth* m_pHealth = { nullptr };
    //피니셔 게이지
    CHealth* m_pFinisher = { nullptr };

    CUI_Group_HP* m_pUI_Health;
    CUI_Group_Finisher* m_pUI_Finisher;
    
    //아이템 갯수를 저장
    vector<_uint> m_vecItemCount = {};

    // 인벤토리
    CInventory* m_pInventory = { nullptr };


private:
    HRESULT Add_Components();

   

public:
    static CPlayer_Information* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END