#include "ComboBox.h"

CComboBox::CComboBox()
    : CCustomWidget()
{
}

HRESULT CComboBox::Initialize(const _char* _szTag, const _char* _szName, vector<string> _Items, const _char* _pStartName)
{
    Set_Tag(_szTag);
    Set_Name(_szName);

    for (_uint i = 0; i < _Items.size(); ++i)
    {
        Push_Back(_Items[i].data());
    }

    if (false == m_Items.empty())
        Update_Current_Item(0);

    if (nullptr != _pStartName)
        Set_StartTag(_pStartName);

    return S_OK;
}

HRESULT CComboBox::Initialize(const _char* _szTag, const _char* _szName, const _char** _pItems, _uint iSize, const _char* _pStartName)
{
    Set_Tag(_szTag);
    Set_Name(_szName);

    for (_uint i = 0; i < iSize; ++i)
    {
        Push_Back(_pItems[i]);
    }

    if (false == m_Items.empty())
        Update_Current_Item(0);

    if (nullptr != _pStartName)
        Set_StartTag(_pStartName);

    return S_OK;
}

HRESULT CComboBox::Initialize(const _char* _szTag, const _char* _szName, initializer_list<string> _Items, const _char* _pStartName)
{
    Set_Tag(_szTag);
    Set_Name(_szName);

    Push_Back(_Items);

    if (false == m_Items.empty())
        Update_Current_Item(0);

    if(nullptr != _pStartName)
        Set_StartTag(_pStartName);

    return S_OK;
}

_bool CComboBox::IsUpdated()
{
    return m_isUpdated;
}

void CComboBox::Set_Tag(string _strTag)
{
    m_strTag = _strTag;
}

void CComboBox::Set_Name(string _strName)
{
    m_strName = _strName;
}

void CComboBox::Set_StartTag(string _strStartTag)
{
    _uint iIndex = 0;
	for (auto Item : m_Items)
	{
		if (Item == _strStartTag)
		{
			Update_Current_Item(iIndex);
			break;
		}
        ++iIndex;
	}
}

string CComboBox::Tick(FLAG eFlag, _bool _isImplement)
{
    m_isUpdated = false;
    if (TABLE == eFlag)
    {
        ImGui::TableSetColumnIndex(0);
        ImGui::Text(m_strName.data()); ImGui::TableSetColumnIndex(1);
    }
        
    if (ImGui::BeginCombo(m_strTag.data(), m_strCurrentItem.data())) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < m_Items.size(); n++)
        {
            bool is_selected = (m_strCurrentItem.data() == m_Items[n]); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(m_Items[n].data(), is_selected))
            {
                Update_Current_Item(n);
                m_isUpdated = true;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus(); // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
        }    

        ImGui::EndCombo();
    }

    if (false == _isImplement)
    {
        ImGui::SameLine(); ImGui::Text("Not Imple");
    }

    if (TABLE == eFlag)
    {
        ImGui::TableNextRow();
    }

    return Get_Current_Item();
}

void CComboBox::Change_Items(vector<string> _Items)
{
    m_Items.clear();

    for (_uint i = 0; i < _Items.size(); ++i)
        Push_Back(_Items[i].data());

    Update_Current_Item(0);
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

void CComboBox::Update_Current_Item(_uint _iItemIndex)
{
    if (_iItemIndex >= m_Items.size())
        return;
    m_iCurrent_Index = _iItemIndex;
    m_strCurrentItem = m_Items[_iItemIndex];
}

void CComboBox::Update_Current_Item(string strItem)
{
    _uint iIndex = 0;
    auto iter = find_if(m_Items.begin(), m_Items.end(), [&](const string& Item) {
        if (strItem == Item)
            return true;
        else
            ++iIndex;
        });

     if (iter == m_Items.end())
        return;

    m_iCurrent_Index = iIndex;
    m_strCurrentItem = strItem;
}

CComboBox* CComboBox::Create(const _char* _szTag, const _char* _szName, initializer_list<string> _Items, const _char* pStartName)
{
    CComboBox* pInstance = new CComboBox;

    if (FAILED(pInstance->Initialize(_szTag, _szName, _Items, pStartName)))
    {
        MSG_BOX("Failed to Created ComboBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComboBox* CComboBox::Create(const _char* _szTag, const _char* _szName, vector<string> _Items, const _char* pStartName)
{
    CComboBox* pInstance = new CComboBox;

    if (FAILED(pInstance->Initialize(_szTag, _szName, _Items, pStartName)))
    {
        MSG_BOX("Failed to Created ComboBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComboBox* CComboBox::Create(const _char* _szTag, const _char* _szName, const _char** _pItems, _uint iSize, const _char* pStartName)
{
    CComboBox* pInstance = new CComboBox;

    if (FAILED(pInstance->Initialize(_szTag, _szName, _pItems, iSize, pStartName)))
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
