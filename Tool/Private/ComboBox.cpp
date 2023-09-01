#include "ComboBox.h"

CComboBox::CComboBox()
    : CCustomWidget()
{
}

HRESULT CComboBox::Initialize(const _char* _szTag, initializer_list<string> _Items)
{
    Set_Tag(_szTag);
    Push_Back(_Items);

    if (false == m_Items.empty())
        Update_Current_Tag(m_Items[0]);

    return S_OK;
}

void CComboBox::Set_Tag(string _strTag)
{
    m_strTag = _strTag;
}

void CComboBox::Show(FLAG eFlag)
{
    if (TABLE == eFlag)
    {
        ImGui::TableSetColumnIndex(0);
        ImGui::Text(m_strTag.data()); ImGui::TableSetColumnIndex(1);
    }
        
    if (ImGui::BeginCombo(m_strTag.data(), m_strCurrentItem.data())) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < m_Items.size(); n++)
        {
            bool is_selected = (m_strCurrentItem.data() == m_Items[n]); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(m_Items[n].data(), is_selected))
            {
                m_strCurrentItem = m_Items[n].data();
                m_iCurrent_Index = n;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus(); // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
        }
        ImGui::EndCombo();
    }
}

void CComboBox::Bind_From_Current_Index(_uint* pIndex)
{
    *pIndex = m_iCurrent_Index;
}

void CComboBox::Push_Back(const _char* _szItem)
{
    m_Items.push_back(_szItem);
}

void CComboBox::Push_Back(initializer_list<string> _Items)
{
    for(auto String : _Items)
    {
        Push_Back(String.c_str());
    }
}

void CComboBox::Erase(const _char* _szItem)
{
    for (auto iter = m_Items.begin(); iter != m_Items.end();)
    {
        if ((*iter) == _szItem)
        {
            iter = m_Items.erase(iter);
            return;
        }
        else
            ++iter;
    }
}

void CComboBox::Erase(initializer_list<string> _Items)
{
    for (auto String : _Items)
    {
        Erase(String.c_str());
    }
}

void CComboBox::Update_Current_Tag(string _strCurrentTag)
{
    m_strCurrentItem = _strCurrentTag;
}

CComboBox* CComboBox::Create(const _char* _szTag, initializer_list<string> _Items)
{
    CComboBox* pInstance = new CComboBox;

    if (FAILED(pInstance->Initialize(_szTag, _Items)))
    {
        MSG_BOX("Failed to Created ComboBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CComboBox::Free(void)
{
    __super::Free();
    m_Items.clear();
}
