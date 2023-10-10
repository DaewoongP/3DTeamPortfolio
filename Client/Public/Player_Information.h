#pragma once
#include "Composite.h"

BEGIN(Engine)

class CHealth;

END

BEGIN(Client)
class CPotionTap;
class CUI_Group_HP;
class CUI_Group_Finisher;
class CInventory;
class CUI_Group_Finisher_Icon;
class CUI_Group_Potion;

class CPlayer_Information :
    public CComposite
{
public:
    enum PLAYER_ITEM
    {
        HEALING_POTION,         //�� ����
        ENDURUS_POTION,     //���� ����
        MAXIMA_POTION,        //���ݷ� ����
        FOCUS_POTION,              //��Ÿ�� ����
        THUNDERBREW_POTION,          //�������� ���� ����
        INVISIBILITY_POTION,      //���������� ����
        MANDRAKE,               //�ǵ巹��ũ
        CHINESES_CHOPPING_CABBAGE,           //������ �����
        TENTACULAR,             //��Ÿŧ��
        PLAYER_ITEM_END
    };

public:

private:
    explicit CPlayer_Information(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CPlayer_Information(const CPlayer_Information& rhs);
    virtual ~CPlayer_Information() = default;

public:
    //Get,Set

    //������ ���� (enum ����)
    _uint const * Get_ItemCountPtr(PLAYER_ITEM _eItem) const { return &m_vecItemCount[_eItem]; }
    //���� ä�� ����
    _float Get_Health_Percent() const { return m_pHealth->Get_Current_HP_Percent(); }
    //���� �Ǵϼ� ����
    _float Get_Finisher_Percent() const { return m_pFinisher->Get_Current_HP_Percent(); }
    CHealth* Get_Health() { return m_pHealth; }
    CInventory* Get_Inventory() { return m_pInventory; }
    CPotionTap* Get_PotionTap() { return m_pPotionTap; }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);

public:
     //�Ǵϼ� ��� ���� ����
    _bool Is_Use_Fnisher();

    //�Ǵϼ� ���� ȣ��(������ ����)
    void Using_Fnisher();

    //�Ǵϼ� ȸ��
    void Stack_Finisher() { m_pFinisher->Set_HP(m_pFinisher->Get_HP() + 1); }

    //ü�� ����
    void fix_HP(_int _iNum);


private:
    //ü��
    CHealth* m_pHealth = { nullptr };
    //�Ǵϼ� ������
    CHealth* m_pFinisher = { nullptr };

    CUI_Group_HP* m_pUI_Health;
    CUI_Group_Finisher* m_pUI_Finisher;
    
    //������ ������ ����
    vector<_uint> m_vecItemCount = {};

    // �κ��丮
    CInventory* m_pInventory = { nullptr };
    CPotionTap* m_pPotionTap = { nullptr };

    CUI_Group_Finisher_Icon* m_pFinisherIcon = { nullptr };
    CUI_Group_Potion* m_pPotion = { nullptr };
private:
    HRESULT Add_Components();

   

public:
    static CPlayer_Information* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComposite* Clone(void* pArg) override;
    virtual void Free() override;
};

END