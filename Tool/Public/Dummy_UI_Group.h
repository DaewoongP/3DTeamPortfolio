#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Tool)

class CDummy_UI_Group final : public CGameObject
{
protected:
	explicit CDummy_UI_Group(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDummy_UI_Group(const CDummy_UI_Group& rhs);
	virtual ~CDummy_UI_Group() = default;

public:
	const _tchar* Get_GroupName() const { return m_wszUIGroupName; }
	class CDummy_UI* Get_Parent() { return  m_pParent; }
	vector <class CDummy_UI*>* Get_Childs() { return &m_Childs; }
//	_bool	Get_isSelected() { return m_isSelected; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	void	Set_Parent(class CDummy_UI* pParent)
	{ 
		if (nullptr !=m_pParent)
		{
			MSG_BOX("Parent is full");
			return;
		}
		m_pParent = pParent;
	}
	void	Set_Parent();
	void	Set_Child(class CDummy_UI* pChild) { m_Childs.push_back(pChild); }
//	void	Set_isSelected(_bool isSelected = true) { m_isSelected = isSelected; }
	void	Delete(class CDummy_UI* pDeleteUI);
	void	Clear();

private: // For Imgui
//	_bool	m_isSelected = { false };

private:
	_tchar	m_wszUIGroupName[MAX_STR] = {};

private:
	class CDummy_UI* m_pParent = { nullptr };
	vector <class CDummy_UI*>	m_Childs;

public:
	static CDummy_UI_Group* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _tchar* pGroupName);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


