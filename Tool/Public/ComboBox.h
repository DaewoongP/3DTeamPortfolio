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
    HRESULT Initialize(const _char* _szTag, const _char* szName, initializer_list<string> _Items);

public:
    string Get_Current_Item() { return m_strCurrentItem; };
    _uint Get_Current_Item_Index() { return m_iCurrent_Index; }

    void Set_Tag(string _strTag);
    void Set_Name(string _strName);
public:
    void Show(FLAG eFlag = NONE);
    void Bind_From_Current_Index(_uint* pIndex);

public:
    // 사용법 : Push_Back("AAAA");
    void Push_Back(const _char* _szItem);

    // 사용법 : Push_Back( {"AAAA", "BBBB"});
    void Push_Back(initializer_list<string> _Items);

    // 사용법 : Erase("AAAA");
    void Erase(const _char* _szItem);

    // 사용법 : Erase({"BBBB", "AAAA"});
    void Erase(initializer_list<string> _Items);

private:
    void Update_Current_Tag(string _strCurrentTag);

private:
    string m_strName = "";
    string m_strTag = "";
    string m_strCurrentItem = { "" };
    _uint  m_iCurrent_Index = { 0 };
    vector<string> m_Items;

public:
    static CComboBox* Create(const _char* _szTag, const _char* szName, initializer_list<string> _Items);
    virtual void Free(void) override;
};
END
