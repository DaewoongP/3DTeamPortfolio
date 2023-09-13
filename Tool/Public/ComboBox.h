#pragma once

#include "CustomWidget.h"

BEGIN(Tool)
class CComboBox : public CCustomWidget
{
public:
    enum FLAG { NONE, TABLE, FLAG_END };

private:
    explicit CComboBox();
    virtual ~CComboBox() = default;
    
public:
    HRESULT Initialize(const _char* _szTag, const _char* _szName, initializer_list<string> _Items, const _char* _pStartName);
    HRESULT Initialize(const _char* _szTag, const _char* _szName, vector<string> _Items, const _char* _pStartName);
    HRESULT Initialize(const _char* _szTag, const _char* _szName, const _char** _pItems, _uint iSize, const _char* pStartName);

public:
    string Get_Current_Item() { return m_strCurrentItem; };
    _uint Get_Current_Item_Index() { return m_iCurrent_Index; }
    _bool IsUpdated();

    void Set_Tag(string _strTag);
    void Set_Name(string _strName);
    void Set_StartTag(string _strStartTag);
public:
    // imgui �޺��ڽ��� ���� ���� ���õ� �׸��� ��ȯ�Ѵ�.
    string Tick(FLAG eFlag = NONE, _bool _isImplement = true);

public:
    // ���� : �޺��ڽ��� �׸���� ���ְ� ���ο� ���� �����̳ʷ� ä���.
    // ����
    // vector<string> Items{ "AAAA", "BBBB" };
    // Change_List(Items);
    void Change_Items(vector<string> _Items);

    // ���� : Push_Back("AAAA");
    void Push_Back(const _char* _szItem);

    // ���� : Push_Back( {"AAAA", "BBBB"});
    void Push_Back(initializer_list<string> _Items);

    // ���� : Erase("AAAA");
    void Erase(const _char* _szItem);

    // ���� : Erase({"BBBB", "AAAA"});
    void Erase(initializer_list<string> _Items);

public:
    void Update_Current_Item(_uint _iItemIndex);
    void Update_Current_Item(string strItem);

private:
    string m_strName = "";
    string m_strTag = "";
    string m_strCurrentItem = { "" };
    _uint  m_iCurrent_Index = { 0 };
    vector<string> m_Items;
    _bool m_isUpdated = { false };
public:
    static CComboBox* Create(const _char* _szTag, const _char* szName, initializer_list<string> _Items, const _char* pStartName = nullptr);
    static CComboBox* Create(const _char* _szTag, const _char* szName, vector<string> _Items, const _char* pStartName = nullptr);
    static CComboBox* Create(const _char* _szTag, const _char* szName, const _char** _pItems, _uint iSize, const _char* pStartName = nullptr);
    virtual void Free(void) override;
};
END
